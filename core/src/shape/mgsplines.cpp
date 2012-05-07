// mgsplines.cpp: 实现三次参数样条曲线类 MgSplines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "mgbasicsp.h"
#include <_mgshape.h>
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

    mgCubicSplines(_count, _points, _knotvs, _closed ? kCubicLoop : 0);
    mgCubicSplinesBox(_extent, _count, _points, _knotvs);
}

float MgSplines::_hitTest(const Point2d& pt, float tol, 
                          Point2d& nearpt, Int32& segment) const
{
    return mgCubicSplinesHit(_count, _points, _knotvs, _closed, 
        pt, tol, nearpt, segment);
}

bool MgSplines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    return mgCubicSplinesIntersectBox(rect, _count, _points, _knotvs, _closed);
}

bool MgSplines::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;

    if (_count == 2)
        ret = gs.drawLine(&ctx, _points[0], _points[1]);
    else if (_closed)
        ret = gs.drawClosedSplines(&ctx, _count, _points, _knotvs);
    else
        ret = gs.drawSplines(&ctx, _count, _points, _knotvs);

    return __super::_draw(gs, ctx) || ret;
}

void MgSplines::smooth(float tol)
{
    if (_bzcount < 3)
        return;
    
    Point2d* points = new Point2d[_bzcount];
    Vector2d* knotvs = new Vector2d[_bzcount];
    UInt32* indexMap = new UInt32[_bzcount];
    UInt32 n = 0;
    UInt32 i, j;
    Point2d nearpt;
    Int32 segment;
    float dist;
    
    points[0] = _points[0];                 // 第一个点不动
    indexMap[0] = 0;
    
    for (i = 1; i + 1 < _count; i++)        // 检查第i点能否去掉，最末点除外
    {
        for (j = 1; i + j < _count; j++)    // 跳过第i点复制后续点到points
            points[n + j] = _points[i + j];
        
        // 新曲线：indexMap[0], indexMap[1], ..., indexMap[n], i+1, i+2, ..., _count-1
        mgCubicSplines(n + _count - i, points, knotvs, _closed ? kCubicLoop : 0);
        dist = mgCubicSplinesHit(n + _count - i, points, knotvs, _closed, _points[i],
                                 tol * 2, nearpt, segment); // 检查第i点到新曲线的距离
        if (dist >= tol) {                  // 第i点去掉则偏了，应保留
            points[++n] = _points[i];
            indexMap[n] = i;                // 新曲线的第j点对应与原indexMap[j]点
        }
        else {
            for (j = 0; j < n + _count - i; j++) {  // 切向变化超过45度时也保留点
                UInt32 index = j > n ? i + j - n : indexMap[j];
                if (_knotvs[index].angleTo(knotvs[j]) > _M_PI_4) {
                    points[++n] = _points[i];
                    indexMap[n] = i;
                    break;
                }
            }
        }
    }
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
