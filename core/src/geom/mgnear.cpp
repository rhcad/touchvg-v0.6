// mgnear.cpp: 实现曲线最近点计算函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgnear.h"
#include "mgcurv.h"
#include "mglnrel.h"

GEOMAPI void mgBeziersBox(
    Box2d& box, Int32 count, const Point2d* points, bool closed)
{
    const int SPLITN = 8;
    Point2d fits[SPLITN + 1];

    box.empty();
    for (Int32 i = 0; i + 3 < count; i += 3)
    {
        for (int j = 1; j < SPLITN; j++)
            mgFitBezier(points + i, 1.f / SPLITN * j, fits[j]);
        fits[0] = points[i];
        fits[SPLITN] = points[i+3];
        box.unionWith(Box2d(SPLITN + 1, fits));
    }
    if (closed && count > 3) {
        Point2d pts[4] = {
            points[count - 1],
            points[count - 1] * 2 - points[count - 2].asVector(),
            points[0] * 2 - points[1].asVector(),
            points[0]
        };
        for (int j = 1; j < SPLITN; j++)
            mgFitBezier(pts, 1.f / SPLITN * j, fits[j]);
        fits[0] = pts[0];
        fits[SPLITN] = pts[3];
        box.unionWith(Box2d(SPLITN + 1, fits));
    }
}

GEOMAPI bool mgBeziersIntersectBox(
    const Box2d& box, Int32 count, const Point2d* points, bool closed)
{
    const int SPLITN = 32;
    Point2d fits[SPLITN + 1];
    
    for (Int32 i = 0; i + 3 < count; i += 3)
    {
        for (int j = 1; j < SPLITN; j++)
            mgFitBezier(points + i, 1.f / SPLITN * j, fits[j]);
        fits[0] = points[i];
        fits[SPLITN] = points[i+3];
        if (box.isIntersect(Box2d(SPLITN + 1, fits)))
            return true;
    }
    if (closed && count > 3) {
        Point2d pts[4] = {
            points[count - 1],
            points[count - 1] * 2 - points[count - 2].asVector(),
            points[0] * 2 - points[1].asVector(),
            points[0]
        };
        for (int j = 1; j < SPLITN; j++)
            mgFitBezier(pts, 1.f / SPLITN * j, fits[j]);
        fits[0] = pts[0];
        fits[SPLITN] = pts[3];
        if (box.isIntersect(Box2d(SPLITN + 1, fits)))
            return true;
    }
    
    return false;
}

static Box2d mgBeziersBox2(const Point2d* points, Int32 count = 4)
{
    Box2d box;
    mgBeziersBox(box, count, points);
    return box;
}

GEOMAPI void mgCubicSplinesBox(
    Box2d& box, Int32 n, const Point2d* knots, 
    const Vector2d* knotvs, bool closed)
{
    Int32 n2 = (closed && n > 1) ? n + 1 : n;

    box.empty();
    for (Int32 i = 0; i + 1 < n2; i++)
    {
        Point2d pts[4] = { knots[i], 
            knots[i] + knotvs[i] / 3.f, 
            knots[(i + 1) % n] - knotvs[(i + 1) % n] / 3.f, 
            knots[(i + 1) % n] };
        box.unionWith(mgBeziersBox2(pts));
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
        if (rect.isIntersect(mgBeziersBox2(pts)))
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
