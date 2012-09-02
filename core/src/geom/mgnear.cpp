// mgnear.cpp: 实现曲线最近点计算函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgnear.h"
#include "mgcurv.h"
#include "mglnrel.h"

static Box2d computeCubicBox(const Point2d& pt1, const Point2d& pt2, 
                             const Point2d& pt3, const Point2d& pt4);

static inline Box2d computeCubicBox(const Point2d points[4])
{
    return computeCubicBox(points[0], points[1], points[2], points[3]);
}

GEOMAPI void mgBeziersBox(
    Box2d& box, Int32 count, const Point2d* points, bool closed)
{
    box.empty();

    for (Int32 i = 0; i + 3 < count; i += 3) {
        box.unionWith(computeCubicBox(points + i));
    }
    if (closed && count > 3) {
        box.unionWith(computeCubicBox(points[count - 1],
            points[count - 1] * 2 - points[count - 2].asVector(),
            points[0] * 2 - points[1].asVector(), points[0]));
    }
}

GEOMAPI bool mgBeziersIntersectBox(
    const Box2d& box, Int32 count, const Point2d* points, bool closed)
{
    for (Int32 i = 0; i + 3 < count; i += 3) {
        if (box.isIntersect(computeCubicBox(points + i))) {
            return true;
        }
    }
    if (closed && count > 3) {
        if (box.isIntersect(computeCubicBox(points[count - 1],
            points[count - 1] * 2 - points[count - 2].asVector(),
            points[0] * 2 - points[1].asVector(), points[0])))
        {
            return true;
        }
    }
    
    return false;
}

GEOMAPI void mgCubicSplinesBox(
    Box2d& box, Int32 n, const Point2d* knots, 
    const Vector2d* knotvs, bool closed)
{
    Int32 n2 = (closed && n > 1) ? n + 1 : n;

    box.empty();
    for (Int32 i = 0; i + 1 < n2; i++)
    {
        box.unionWith(computeCubicBox(knots[i], 
            knots[i] + knotvs[i] / 3.f, 
            knots[(i + 1) % n] - knotvs[(i + 1) % n] / 3.f, 
            knots[(i + 1) % n]));
    }
}

GEOMAPI bool mgCubicSplinesIntersectBox(
    const Box2d& box, Int32 n, const Point2d* knots, 
    const Vector2d* knotvs, bool closed)
{
    Int32 n2 = (closed && n > 1) ? n + 1 : n;
    
    for (Int32 i = 0; i + 1 < n2; i++)
    {
        Point2d pts[4] = { knots[i], 
            knots[i] + knotvs[i] / 3.f, 
            knots[(i + 1) % n] - knotvs[(i + 1) % n] / 3.f, 
            knots[(i + 1) % n] };
        if (mgBeziersIntersectBox(box, 4, pts, false))
            return true;
    }
    
    return false;
}

GEOMAPI float mgCubicSplinesHit(
    Int32 n, const Point2d* knots, const Vector2d* knotvs, bool closed, 
    const Point2d& pt, float tol, Point2d& nearpt, Int32& segment)
{
    Point2d ptTemp;
    float dDist, dDistMin = _FLT_MAX;
    Point2d pts[4];
    const Box2d rect (pt, 2 * tol, 2 * tol);
    Int32 n2 = (closed && n > 1) ? n + 1 : n;

    segment = -1;
    for (Int32 i = 0; i + 1 < n2; i++)
    {
        mgCubicSplineToBezier(n, knots, knotvs, i, pts);
        if (rect.isIntersect(computeCubicBox(pts)))
        {
            mgNearestOnBezier(pt, pts, ptTemp);
            dDist = pt.distanceTo(ptTemp);
            if (dDist <= tol && dDist < dDistMin)
            {
                dDistMin = dDist;
                nearpt = ptTemp;
                segment = i;
            }
        }
    }

    return dDistMin;
}

GEOMAPI Int32 mgBSplinesToBeziers(
    Point2d points[/*1+n*3*/], Int32 n, const Point2d* ctlpts, bool closed)
{
    Point2d pt1, pt2, pt3, pt4;
    float d6 = 1.f / 6.f;
    int i = 0;
        
    pt1 = ctlpts[0];
    pt2 = ctlpts[1];
    pt3 = ctlpts[2];
    pt4 = ctlpts[3 % n];
    points[i++].set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    points[i++].set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
    points[i++].set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
    points[i++].set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    
    for (int ci = 4; ci < (closed ? n + 3 : n); ci++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = ctlpts[ci % n];
        points[i++].set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
        points[i++].set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
        points[i++].set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    }
    
    return i;
}

GEOMAPI float mgLinesHit(
    Int32 n, const Point2d* points, bool closed, 
    const Point2d& pt, float tol, Point2d& nearpt, Int32& segment)
{
    Point2d ptTemp;
    float dDist, dDistMin = _FLT_MAX;
    const Box2d rect (pt, 2 * tol, 2 * tol);
    Int32 n2 = (closed && n > 1) ? n + 1 : n;

    segment = -1;
    for (Int32 i = 0; i + 1 < n2; i++)
    {
        const Point2d& pt2 = points[(i + 1) % n];
        if (rect.isIntersect(Box2d(points[i], pt2)))
        {
            dDist = mgPtToLine(points[i], pt2, pt, ptTemp);
            if (dDist <= tol && dDist < dDistMin)
            {
                dDistMin = dDist;
                nearpt = ptTemp;
                segment = i;
            }
        }
    }

    return dDistMin;
}

static inline
Point2d RoundRectTan(Int32 nFrom, Int32 nTo, const Box2d& rect, float r)
{
    Point2d pt1, pt2;
    mgGetRectHandle(rect, nFrom, pt1);
    mgGetRectHandle(rect, nTo, pt2);
    return pt1.rulerPoint(pt2, r, 0.f);
}

static void _RoundRectHit(
    const Box2d& rect, float rx, float ry, 
    const Point2d& pt, float tol, const Box2d &rectTol, 
    Point2d* pts, float& dDistMin, 
    Point2d& nearpt, Int32& segment)
{
    Point2d ptsBezier[13], ptTemp;
    Vector2d vec;
    float dx = rect.width() * 0.5f - rx;
    float dy = rect.height() * 0.5f - ry;
    
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
            float dDist = pt.distanceTo(ptTemp);
            if (dDist <= tol && dDist < dDistMin)
            {
                dDistMin = dDist;
                nearpt = ptTemp;
                segment = (5 - i) % 4;
            }
        }
        
        pts[3] -= vec;
    }
}

GEOMAPI float mgRoundRectHit(
    const Box2d& rect, float rx, float ry, 
    const Point2d& pt, float tol, Point2d& nearpt, Int32& segment)
{
    rx = fabs(rx);
    if (ry < _MGZERO)
        ry = rx;
    rx = mgMin(rx, rect.width() * 0.5f);
    ry = mgMin(ry, rect.height() * 0.5f);
    segment = -1;
    
    Point2d ptTemp, ptTemp2;
    float dDist, dDistMin = _FLT_MAX;
    Point2d pts[8];
    const Box2d rectTol (pt, 2 * tol, 2 * tol);
    
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
            if (dDist <= tol && dDist < dDistMin)
            {
                dDistMin = dDist;
                nearpt = ptTemp;
                segment = 4 + i;
            }
        }
    }
    
    if (rx > _MGZERO && ry > _MGZERO)
    {
        _RoundRectHit(rect, rx, ry, pt, tol, rectTol, pts, dDistMin, nearpt, segment);
    }

    return dDistMin;
}

GEOMAPI void mgGetRectHandle(const Box2d& rect, Int32 index, Point2d& pt)
{
    switch (index)
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

GEOMAPI void mgMoveRectHandle(Box2d& rect, Int32 index, const Point2d& pt)
{
    Point2d pts[4];

    for (int i = 0; i < 4; i++)
        mgGetRectHandle(rect, index / 4 * 4 + i, pts[i]);
    pts[index % 4] = pt;

    if (index >= 0 && index < 4)
    {
        if (index % 2 == 0)
        {
            pts[(index + 1) % 4].y = pt.y;
            pts[(index + 3) % 4].x = pt.x;
        }
        else
        {
            pts[(index + 1) % 4].x = pt.x;
            pts[(index + 3) % 4].y = pt.y;
        }
        rect.set(4, pts);
    }
    else if (index >= 4 && index < 8)
    {
        rect.set(pts[3].x, pts[2].y, pts[1].x, pts[0].y);
    }
}

// computeCubicBaseValue, computeCubicFirstDerivativeRoots, computeCubicBox
// © 2011 Mike "Pomax" Kamermans of nihongoresources.com
// http://processingjs.nihongoresources.com/bezierinfo/#bounds

// compute the value for the cubic bezier function at time=t
static float computeCubicBaseValue(float t, float a, float b, float c, float d) {
    float v = 1-t, v2 = v*v, t2 = t*t;
    return v2*v*a + 3*v2*t*b + 3*v*t2*c + t2*t*d;
}

// compute the value for the first derivative of the cubic bezier function at time=t
static Point2d computeCubicFirstDerivativeRoots(float a, float b, float c, float d) {
    Point2d ret (-1, -1);
    float tl = -a+2*b-c;
    float tr = -(float)sqrt(-a*(c-d) + b*b - b*(c+d) +c*c);
    float dn = -a+3*b-3*c+d;
    if (dn != 0) {
        ret.x = (tl+tr)/dn;
        ret.y = (tl-tr)/dn;
    }
    return ret;
}

// Compute the bounding box based on the straightened curve, for best fit
Box2d computeCubicBox(const Point2d& pt1, const Point2d& pt2, 
                      const Point2d& pt3, const Point2d& pt4)
{
    Box2d bbox(pt1, pt4);

    // find the zero point for x and y in the derivatives
    Point2d ts = computeCubicFirstDerivativeRoots(pt1.x, pt2.x, pt3.x, pt4.x);
    if (ts.x>=0 && ts.x<=1) {
        float x = computeCubicBaseValue(ts.x, pt1.x, pt2.x, pt3.x, pt4.x);
        if (x < bbox.xmin) bbox.xmin = x;
        if (x > bbox.xmax) bbox.xmax = x;
    }
    if (ts.y>=0 && ts.y<=1) {
        float x = computeCubicBaseValue(ts.y, pt1.x, pt2.x, pt3.x, pt4.x);
        if (x < bbox.xmin) bbox.xmin = x;
        if (x > bbox.xmax) bbox.xmax = x;
    }
    ts = computeCubicFirstDerivativeRoots(pt1.y, pt2.y, pt3.y, pt4.y);
    if (ts.x>=0 && ts.x<=1) {
        float y = computeCubicBaseValue(ts.x, pt1.y, pt2.y, pt3.y, pt4.y);
        if (y < bbox.ymin) bbox.ymin = y;
        if (y > bbox.ymax) bbox.ymax = y;
    }
    if (ts.y>=0 && ts.y<=1) {
        float y = computeCubicBaseValue(ts.y, pt1.y, pt2.y, pt3.y, pt4.y);
        if (y < bbox.ymin) bbox.ymin = y;
        if (y > bbox.ymax) bbox.ymax = y;
    }

    return bbox;
}
