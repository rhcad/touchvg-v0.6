// mgsplines.cpp: 实现三次参数样条曲线类 MgSplines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>
#include <mgnear.h>
#include <mgcurv.h>

MG_IMPLEMENT_CREATE(MgSplines)

MgSplines::MgSplines() : _knotvs(NULL), _bzcount(0)
{
}

MgSplines::~MgSplines()
{
    if (_knotvs)
        delete[] _knotvs;
}

void MgSplines::_update()
{
    __super::_update();

    if (_bzcount < _count)
    {
        if (_knotvs)
            delete[] _knotvs;
        _bzcount = _maxCount;
        _knotvs = new Vector2d[_bzcount];
    }

    mgCubicSplines(_count, _points, _knotvs, isClosed() ? kMgCubicLoop : 0);
    mgCubicSplinesBox(_extent, _count, _points, _knotvs);
}

float MgSplines::_hitTest(const Point2d& pt, float tol, 
                          Point2d& nearpt, int& segment) const
{
    return mgCubicSplinesHit(_count, _points, _knotvs, isClosed(), 
        pt, tol, nearpt, segment);
}

bool MgSplines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    return mgCubicSplinesIntersectBox(rect, _count, _points, _knotvs, isClosed());
}

bool MgSplines::_draw(int mode, GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;

    if (_count == 2)
        ret = gs.drawLine(&ctx, _points[0], _points[1]);
    else if (isClosed())
        ret = gs.drawClosedSplines(&ctx, _count, _points, _knotvs);
    else
        ret = gs.drawSplines(&ctx, _count, _points, _knotvs);

    return __super::_draw(mode, gs, ctx) || ret;
}

void MgSplines::smooth(float tol)
{
    if (_count < 3)
        return;
    
    Point2d* points = new Point2d[_count];
    Vector2d* knotvs = new Vector2d[_count];
    int* indexMap = new int[_count];
    int n = 0;
    int i, j;
    Point2d nearpt;
    int segment;
    float dist;
    
    points[0] = _points[0];                     // 第一个点不动
    indexMap[0] = 0;
    
    for (i = 1; i + 1 < _count; i++)            // 检查第i点能否去掉，最末点除外
    {
        for (j = 1; i + j < _count; j++)        // 跳过第i点复制后续点到points
            points[n + j] = _points[i + j];
        
        // 新曲线：indexMap[0], indexMap[1], ..., indexMap[n], i+1, i+2, ..., _count-1
        mgCubicSplines(n + _count - i, points, knotvs, isClosed() ? kMgCubicLoop : 0);
        dist = mgCubicSplinesHit(n + _count - i, points, knotvs, isClosed(), _points[i],
                                 tol * 2, nearpt, segment); // 检查第i点到新曲线的距离
        
        bool removed = true;
        if (dist >= tol) {                      // 第i点去掉则偏了，应保留
            removed = false;
        }
        else {
            for (j = 0; j < n + _count - i; j++) {  // 切向变化超过45度时也保留点
                int index = j > n ? i + j - n : indexMap[j];
                if (_knotvs[index].angleTo(knotvs[j]) > _M_PI_4) {
                    removed = false;
                    break;
                }
            }
        }
        if (!removed) {
            points[++n] = _points[i];
            indexMap[n] = i;                    // 新曲线的第j点对应与原indexMap[j]点
        }
    }
    if (points[n].distanceTo(_points[_count - 1]) > tol)
        points[++n] = _points[_count - 1];      // 加上末尾点
    
    if (n + 1 < _count) {
        _count = n + 1;
        for (i = 0; i < _count; i++)
            _points[i] = points[i];
        update();
    }
    
    delete[] points;
    delete[] knotvs;
    delete[] indexMap;
}
