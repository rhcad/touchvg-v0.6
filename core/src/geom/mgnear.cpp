// mgnear.cpp: 实现曲线最近点计算函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "mgnear.h"
#include "mgcurv.h"
#include "mglnrel.h"

GEOMAPI void mgBeziersBox(
    Box2d& box, Int32 count, const Point2d* points, bool /*closed*/)
{
    box.set(count, points); // TODO: 待找到更好的算法
}

GEOMAPI void mgCubicSplinesBox(
    Box2d& box, Int32 n, const Point2d* knots, 
    const Vector2d* knotVectors, bool closed)
{
    box.set(n, knots);
    for (Int32 i = 0; i + 1 < n; i++)
    {
        box.unionWith(knots[i] + knotVectors[i] / 3.0);
        box.unionWith(knots[i + 1] - knotVectors[i + 1] / 3.0);
    }
    if (closed && n > 1)
    {
        box.unionWith(knots[n - 1] + knotVectors[n - 1] / 3.0);
        box.unionWith(knots[0] - knotVectors[0] / 3.0);
    }
}

GEOMAPI double mgCubicSplinesHit(
    Int32 n, const Point2d* knots, const Vector2d* knotVectors, bool closed, 
    const Point2d& pt, double dTol, Point2d& ptNear, Int32& nSegment)
{
    Point2d ptTemp;
    double dDist, dDistMin = _DBL_MAX;
    Point2d pts[4];
    const Box2d rect (pt, 2 * dTol, 2 * dTol);

    nSegment = -1;
    for (Int32 i = 0; i + 1 < n; i++)
    {
        mgCubicSplineToBezier(n, knots, knotVectors, i, pts);
        if (rect.isIntersect(Box2d(4, pts)))
        {
            mgNearestOnBezier(pt, pts, ptTemp);
            dDist = pt.distanceTo(ptTemp);
            if (dDist <= dTol && dDist < dDistMin)
            {
                dDistMin = dDist;
                ptNear = ptTemp;
                nSegment = i;
            }
        }
    }
    if (closed && n > 1)
    {
        mgCubicSplineToBezier(n, knots, knotVectors, n - 1, pts);
        if (rect.isIntersect(Box2d(4, pts)))
        {
            mgNearestOnBezier(pt, pts, ptTemp);
            dDist = pt.distanceTo(ptTemp);
            if (dDist <= dTol && dDist < dDistMin)
            {
                dDistMin = dDist;
                ptNear = ptTemp;
                nSegment = n - 1;
            }
        }
    }

    return dDistMin;
}

GEOMAPI double mgLinesHit(
    Int32 n, const Point2d* points, bool closed, 
    const Point2d& pt, double dTol, Point2d& ptNear, Int32& nSegment)
{
    Point2d ptTemp;
    double dDist, dDistMin = _DBL_MAX;
    const Box2d rect (pt, 2 * dTol, 2 * dTol);

    nSegment = -1;
    for (Int32 i = 0; i + 1 < n; i++)
    {
        if (rect.isIntersect(Box2d(points[i], points[i + 1])))
        {
            dDist = mgPtToLine(points[i], points[i + 1], pt, ptTemp);
            if (dDist <= dTol && dDist < dDistMin)
            {
                dDistMin = dDist;
                ptNear = ptTemp;
                nSegment = i;
            }
        }
    }
    if (closed && n > 1)
    {
        if (rect.isIntersect(Box2d(points[n - 1], points[0])))
        {
            dDist = mgPtToLine(points[n - 1], points[0], pt, ptTemp);
            if (dDist <= dTol && dDist < dDistMin)
            {
                dDistMin = dDist;
                ptNear = ptTemp;
                nSegment = n - 1;
            }
        }
    }

    return dDistMin;
}

static inline
Point2d RoundRectTan(Int32 nFrom, Int32 nTo, const Box2d& rect, double r)
{
    Point2d pt1, pt2;
    mgGetRectHandle(rect, nFrom, pt1);
    mgGetRectHandle(rect, nTo, pt2);
    return pt1.rulerPoint(pt2, r, 0.0);
}

static void _RoundRectHit(
    const Box2d& rect, double rx, double ry, 
    const Point2d& pt, double dTol, const Box2d &rectTol, 
    Point2d* pts, double& dDistMin, 
    Point2d& ptNear, Int32& nSegment)
{
    Point2d ptsBezier[13], ptTemp;
    Vector2d vec;
    double dx = rect.width() * 0.5 - rx;
    double dy = rect.height() * 0.5 - ry;
    
    // 按逆时针方向从第一象限到第四象限连接的四段
    mgEllipseToBezier(ptsBezier, rect.center(), rx, ry);
    
    pts[3] = ptsBezier[0];
    for (int i = 0; i < 4; i++)
    {
        pts[0] = pts[3];
        pts[1] = ptsBezier[3 * i];
        pts[2] = ptsBezier[3 * i + 1];
        pts[3] = ptsBezier[3 * i + 2];
        
        switch (i)
        {
        case 0: vec.set(dx, dy); break;
        case 1: vec.set(-dx, dy); break;
        case 2: vec.set(-dx, -dy); break;
        case 3: vec.set(dx, -dy); break;
        }
        
        for (int j = 0; j < 4; j++)
            pts[j] += vec;
        
        if (rectTol.isIntersect(Box2d(4, pts)))
        {
            mgNearestOnBezier(pt, pts, ptTemp);
            double dDist = pt.distanceTo(ptTemp);
            if (dDist <= dTol && dDist < dDistMin)
            {
                dDistMin = dDist;
                ptNear = ptTemp;
                nSegment = (5 - i) % 4;
            }
        }
        
        pts[3] -= vec;
    }
}

GEOMAPI double mgRoundRectHit(
    const Box2d& rect, double rx, double ry, 
    const Point2d& pt, double dTol, Point2d& ptNear, Int32& nSegment)
{
    rx = fabs(rx);
    if (ry < _MGZERO)
        ry = rx;
    rx = mgMin(rx, rect.width() * 0.5);
    ry = mgMin(ry, rect.height() * 0.5);
    nSegment = -1;
    
    Point2d ptTemp, ptTemp2;
    double dDist, dDistMin = _DBL_MAX;
    Point2d pts[8];
    const Box2d rectTol (pt, 2 * dTol, 2 * dTol);
    
    // 顶边上的两个圆弧切点，左，右
    pts[0] = RoundRectTan(0, 1, rect, rx);
    pts[1] = RoundRectTan(1, 0, rect, rx);
    
    // 右边上的两个圆弧切点，上，下
    pts[2] = RoundRectTan(1, 2, rect, ry);
    pts[3] = RoundRectTan(2, 1, rect, ry);
    
    // 底边上的两个圆弧切点，右，左
    pts[4] = RoundRectTan(2, 3, rect, rx);
    pts[5] = RoundRectTan(3, 2, rect, rx);
    
    // 左边上的两个圆弧切点，下，上
    pts[6] = RoundRectTan(3, 0, rect, ry);
    pts[7] = RoundRectTan(0, 3, rect, ry);
    
    for (int i = 0; i < 4; i++)
    {
        Box2d rcLine (pts[2 * i], pts[2 * i + 1]);
        if (rcLine.isEmpty() || rectTol.isIntersect(rcLine))
        {
            dDist = mgPtToLine(pts[2 * i], pts[2 * i + 1], pt, ptTemp);
            if (dDist <= dTol && dDist < dDistMin)
            {
                dDistMin = dDist;
                ptNear = ptTemp;
                nSegment = 4 + i;
            }
        }
    }
    
    if (rx > _MGZERO && ry > _MGZERO)
    {
        _RoundRectHit(rect, rx, ry, pt, dTol, rectTol, pts, dDistMin, ptNear, nSegment);
    }

    return dDistMin;
}

GEOMAPI void mgGetRectHandle(const Box2d& rect, Int32 nHandle, Point2d& pt)
{
    switch (nHandle)
    {
    case 0: pt = rect.leftTop(); break;
    case 1: pt = rect.rightTop(); break;
    case 2: pt = rect.rightBottom(); break;
    case 3: pt = rect.leftBottom(); break;
    case 4: pt = Point2d(rect.center().x, rect.ymax); break;
    case 5: pt = Point2d(rect.xmax, rect.center().y); break;
    case 6: pt = Point2d(rect.center().x, rect.ymin); break;
    case 7: pt = Point2d(rect.xmin, rect.center().y); break;
    default: pt = rect.center(); break;
    }
}

GEOMAPI void mgMoveRectHandle(Box2d& rect, Int32 nHandle, const Point2d& pt)
{
    Point2d pts[4];

    for (int i = 0; i < 4; i++)
        mgGetRectHandle(rect, nHandle / 4 * 4 + i, pts[i]);
    pts[nHandle % 4] = pt;

    if (nHandle >= 0 && nHandle < 4)
    {
        if (nHandle % 2 == 0)
        {
            pts[(nHandle + 1) % 4].y = pt.y;
            pts[(nHandle + 3) % 4].x = pt.x;
        }
        else
        {
            pts[(nHandle + 1) % 4].x = pt.x;
            pts[(nHandle + 3) % 4].y = pt.y;
        }
        rect.set(4, pts);
    }
    else if (nHandle >= 4 && nHandle < 8)
    {
        rect.set(pts[3].x, pts[2].y, pts[1].x, pts[0].y);
    }
}
