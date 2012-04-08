// mgsplines.cpp: 实现三次参数样条曲线类 MgSplines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "mgbasicsp.h"
#include <_mgshape.h>
#include <mgnear.h>
#include <mgcurv.h>

MG_IMPLEMENT_CREATE(MgSplines)

MgSplines::MgSplines() : _knotVectors(NULL), _bzcount(0)
{
}

MgSplines::~MgSplines()
{
    if (_knotVectors)
        delete[] _knotVectors;
}

void MgSplines::_update()
{
    __super::_update();

    if (_bzcount < _count)
    {
        if (_knotVectors)
            delete[] _knotVectors;
        _bzcount = _maxCount;
        _knotVectors = new Vector2d[_bzcount];
    }

    mgCubicSplines(_count, _points, _knotVectors, _closed ? kCubicLoop : 0);
    mgCubicSplinesBox(_extent, _count, _points, _knotVectors);
}

double MgSplines::_hitTest(const Point2d& pt, double tol, 
                           Point2d& ptNear, Int32& segment) const
{
    return mgCubicSplinesHit(_count, _points, _knotVectors, _closed, 
        pt, tol, ptNear, segment);
}

bool MgSplines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    return mgCubicSplinesIntersectBox(rect, _count, _points, _knotVectors, _closed);
}

bool MgSplines::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;

    if (_closed)
    {
        ret = gs.drawClosedSplines(&ctx, _count, _points, _knotVectors);
    }
    else
    {
        ret = gs.drawSplines(&ctx, _count, _points, _knotVectors);
    }

    return __super::_draw(gs, ctx) || ret;
}

void MgSplines::smooth(double tol)
{
    if (_bzcount < 3)
        return;
    
    Point2d* points = new Point2d[_bzcount];
    Vector2d* knotVectors = new Vector2d[_bzcount];
    UInt32* indexMap = new UInt32[_bzcount];
    UInt32 n = 0;
    UInt32 i, j;
    Point2d ptNear;
    Int32 segment;
    double dist;
    
    points[0] = _points[0];                 // 第一个点不动
    indexMap[0] = 0;
    
    for (i = 1; i + 1 < _count; i++)        // 检查第i点能否去掉，最末点除外
    {
        for (j = 1; i + j < _count; j++)    // 跳过第i点复制后续点到points
            points[n + j] = _points[i + j];
        
        // 新曲线：indexMap[0], indexMap[1], ..., indexMap[n], i+1, i+2, ..., _count-1
        mgCubicSplines(n + _count - i, points, knotVectors, _closed ? kCubicLoop : 0);
        dist = mgCubicSplinesHit(n + _count - i, points, knotVectors, _closed, _points[i],
                                 tol * 2, ptNear, segment); // 检查第i点到新曲线的距离
        if (dist >= tol) {                  // 第i点去掉则偏了，应保留
            points[++n] = _points[i];
            indexMap[n] = i;                // 新曲线的第j点对应与原indexMap[j]点
        }
        else {
            for (j = 0; j < n + _count - i; j++) {  // 切向变化超过45度时也保留点
                UInt32 index = j > n ? i + j - n : indexMap[j];
                if (_knotVectors[index].angleTo(knotVectors[j]) > M_PI_4) {
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
    delete[] knotVectors;
    delete[] indexMap;
}
