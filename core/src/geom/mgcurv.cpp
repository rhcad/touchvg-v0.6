// mgcurv.cpp: 实现曲线拟和函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "mgcurv.h"
#include "mgbase.h"
#include "mglnrel.h"

GEOMAPI void mgFitBezier(const Point2d* pts, double t, Point2d& ptFit)
{
    double v = 1.0 - t;
    double v2 = v * v;
    double t2 = t * t;
    
    ptFit = (v2 * v) * pts[0] + 3.0 * t * v2 * pts[1]
        + 3.0 * t2 * v * pts[2] + t2 * t * pts[3];
}

GEOMAPI void mgBezier4P(
    const Point2d& pt1, const Point2d& pt2, const Point2d& pt3, 
    const Point2d& pt4, Point2d& ptCtr1, Point2d& ptCtr2)
{
    Point2d ptCtr = (-5 * pt1 + 18 * pt2 -  9 * pt3 + 2 * pt4) / 6;
    ptCtr2      = (-5 * pt4 + 18 * pt3 -  9 * pt2 + 2 * pt1) / 6;
    ptCtr1 = ptCtr;
}

GEOMAPI void mgEllipse90ToBezier(
    const Point2d& ptFrom, const Point2d& ptTo, Point2d& ptCtr1, Point2d& ptCtr2)
{
    double rx = ptFrom.x - ptTo.x;
    double ry = ptTo.y - ptFrom.y;
    const Point2d center (ptTo.x, ptFrom.y);

    const double M = 0.5522847498307933984022516; // 4(sqrt(2)-1)/3
    double dx = rx * M;
    double dy = ry * M;
    
    ptCtr1.x = center.x + rx;
    ptCtr1.y = center.y + dy;
    ptCtr2.x = center.x + dx;
    ptCtr2.y = center.y + ry;
}

GEOMAPI void mgEllipseToBezier(
    Point2d points[13], const Point2d& center, double rx, double ry)
{
    const double M = 0.5522847498307933984022516; // 4(sqrt(2)-1)/3
    double dx = rx * M;
    double dy = ry * M;
    
    points[ 0].x = center.x + rx;  //   .   .   .   .   .
    points[ 0].y = center.y;       //       4   3   2
    points[ 1].x = center.x + rx;  //
    points[ 1].y = center.y + dy;  //   5               1
    points[ 2].x = center.x + dx;  //
    points[ 2].y = center.y + ry;  //   6              0,12
    points[ 3].x = center.x;       //
    points[ 3].y = center.y + ry;  //   7               11
    //
    points[ 4].x = center.x - dx;  //       8   9   10
    points[ 4].y = center.y + ry;
    points[ 5].x = center.x - rx;
    points[ 5].y = center.y + dy;
    points[ 6].x = center.x - rx;
    points[ 6].y = center.y;
    
    points[ 7].x = center.x - rx;
    points[ 7].y = center.y - dy;
    points[ 8].x = center.x - dx;
    points[ 8].y = center.y - ry;
    points[ 9].x = center.x;
    points[ 9].y = center.y - ry;
    
    points[10].x = center.x + dx;
    points[10].y = center.y - ry;
    points[11].x = center.x + rx;
    points[11].y = center.y - dy;
    points[12].x = center.x + rx;
    points[12].y = center.y;
}

GEOMAPI void mgRoundRectToBeziers(
    Point2d points[16], const Box2d& rect, double rx, double ry)
{
    if (2 * rx > rect.width())
        rx = rect.width() / 2;
    if (2 * ry > rect.height())
        ry = rect.height() / 2;

    int i, j;
    double dx = rect.width() / 2 - rx;
    double dy = rect.height() / 2 - ry;

    mgEllipseToBezier(points, rect.center(), rx, ry);

    for (i = 3; i >= 1; i--)
    {
        for (j = 3; j >= 0; j--)
            points[4 * i + j] = points[3 * i + j];
    }
    for (i = 0; i < 4; i++)
    {
        double dx1 = (0 == i || 3 == i) ? dx : -dx;
        double dy1 = (0 == i || 1 == i) ? dy : -dy;
        for (j = 0; j < 4; j++)
            points[0 ].offset(dx1, dy1);
    }
}

static void _mgAngleArcToBezier(
    Point2d points[4], const Point2d& center, double rx, double ry,
    double startAngle, double sweepAngle)
{
    // Compute bezier curve for arc centered along y axis
    // Anticlockwise: (0,-B), (x,-y), (x,y), (0,B)
    double sy = ry / rx;
    ry = rx;
    double B = ry * sin(sweepAngle / 2);
    double C = rx * cos(sweepAngle / 2);
    double A = rx - C;
    
    double X = A * 4 / 3;
    double Y = B - X * (rx-A)/B;
    
    points[0].set(C,   -B);
    points[1].set(C+X, -Y);
    points[2].set(C+X,  Y);
    points[3].set(C,    B);
    
    // rotate to the original angle
    A = startAngle + sweepAngle / 2;
    double s = sin(A);
    double c = cos(A);
    
    for (int i = 0; i < 4; i++)
    {
        points[i].set(center.x + points[i].x * c - points[i].y * s,
            center.y + points[i].x * s * sy + points[i].y * c * sy);
    }
}

static int _mgAngleArcToBezierPlusSweep(
    Point2d points[16], const Point2d& center, double rx, double ry, 
    double startAngle, double sweepAngle)
{
    const double M = 0.5522847498307933984022516;
    double dx = rx * M;
    double dy = ry * M;

    int k, n;
    double endAngle;

    // 计算第一段椭圆弧的终止角度
    if (startAngle < M_PI_2) {              // +Y
        endAngle = M_PI_2;
        k = 1;
    }
    else if (startAngle < M_PI) {           // -X
        endAngle = M_PI;
        k = 2;
    }
    else if (startAngle < 3*M_PI_2) {       // -Y
        endAngle = 3*M_PI_2;
        k = 3;
    }
    else {                                  // +X
        endAngle = _M_2PI;
        k = 0;
    }
    if (endAngle - startAngle > 1e-5)       // 转换第一段椭圆弧
    {
        _mgAngleArcToBezier(points, center, rx, ry,
            startAngle, endAngle - startAngle);
        n = 4;
    }
    else
        n = 1;                              // 第一点在下边循环内设置
    sweepAngle -= (endAngle - startAngle);
    startAngle = endAngle;
    while (sweepAngle >= M_PI_2)            // 增加整90度弧
    {
        if (k == 0)                         // 第一象限
        {
            points[n-1].set(center.x + rx, center.y);
            points[n  ].set(center.x + rx, center.y + dy);
            points[n+1].set(center.x + dx, center.y + ry);
            points[n+2].set(center.x,      center.y + ry);
        }
        else if (k == 1)                    // 第二象限
        {
            points[n-1].set(center.x,      center.y + ry);
            points[n  ].set(center.x - dx, center.y + ry);
            points[n+1].set(center.x - rx, center.y + dy);
            points[n+2].set(center.x - rx, center.y);
        }
        else if (k == 2)                    // 第三象限
        {
            points[n-1].set(center.x - rx, center.y);
            points[n  ].set(center.x - rx, center.y - dy);
            points[n+1].set(center.x - dx, center.y - ry);
            points[n+2].set(center.x,      center.y - ry);
        }
        else                                // 第四象限
        {
            points[n-1].set(center.x,      center.y - ry);
            points[n  ].set(center.x + dx, center.y - ry);
            points[n+1].set(center.x + rx, center.y - dy);
            points[n+2].set(center.x + rx, center.y);
        }
        k = (k + 1) % 4;
        n += 3;
        sweepAngle -= M_PI_2;
        startAngle += M_PI_2;
    }
    if (sweepAngle > 1e-5)                  // 增加余下的弧
    {
        _mgAngleArcToBezier(&points[n-1], center, rx, ry, startAngle, sweepAngle);
        n += 3;
    }

    return n;
}

GEOMAPI int mgAngleArcToBezier(
    Point2d points[16], const Point2d& center, double rx, double ry,
    double startAngle, double sweepAngle)
{
    if (mgIsZero(rx) || fabs(sweepAngle) < 1e-5)
        return 0;
    if (mgIsZero(ry))
        ry = rx;
    if (sweepAngle > _M_2PI)
        sweepAngle = _M_2PI;
    else if (sweepAngle < -_M_2PI)
        sweepAngle = -_M_2PI;
    
    int n = 0;
    
    if (fabs(sweepAngle) < M_PI_2 + 1e-5)
    {
        _mgAngleArcToBezier(points, center, rx, ry, startAngle, sweepAngle);
        n = 4;
    }
    else if (sweepAngle > 0)
    {
        startAngle = mgTo0_2PI(startAngle);
        n = _mgAngleArcToBezierPlusSweep(
            points, center, rx, ry, startAngle, sweepAngle);
    }
    else // sweepAngle < 0
    {
        double endAngle = startAngle + sweepAngle;
        sweepAngle = -sweepAngle;
        startAngle = mgTo0_2PI(endAngle);
        n = _mgAngleArcToBezierPlusSweep(
            points, center, rx, ry, startAngle, sweepAngle);

        for (int i = 0; i < n / 2; i++)
            mgSwap(points[i], points[n - 1 - i]);
    }
    
    return n;
}

GEOMAPI bool mgArc3P(
    const Point2d& start, const Point2d& point, const Point2d& end,
    Point2d& center, double& radius,
    double* startAngle, double* sweepAngle)
{
    double a1, b1, c1, a2, b2, c2;
    
    a1 = end.x - start.x;
    b1 = end.y - start.y;
    c1 = -0.5 * (a1 * (end.x + start.x) + b1 * (end.y + start.y));
    a2 = end.x - point.x;
    b2 = end.y - point.y;
    c2 = -0.5 * (a2 * (end.x + point.x) + b2 * (end.y + point.y));
    if (!mgCrossLineAbc(a1, b1, c1, a2, b2, c2, center, Tol::gTol()))
        return false;
    radius = mgHypot(center.x - start.x, center.y - start.y);
    
    if (startAngle != NULL && sweepAngle != NULL)
    {
        // 分别计算圆心到三点的角度
        a1 = atan2(start.y - center.y, start.x - center.x);
        b1 = atan2(point.y - center.y, point.x - center.x);
        c1 = atan2(end.y - center.y, end.x - center.x);
        
        *startAngle = a1;
        
        // 判断圆弧的方向，计算转角
        if (a1 < c1)
        {
            if (a1 < b1 && b1 < c1)         // 逆时针
                *sweepAngle = c1-a1;
            else
                *sweepAngle = c1-a1-_M_2PI;
        }
        else
        {
            if (a1 > b1 && b1 > c1)         // 顺时针
                *sweepAngle = c1-a1;
            else
                *sweepAngle = _M_2PI-(a1-c1);
        }
    }
    
    return true;
}

GEOMAPI bool mgArcTan(
    const Point2d& start, const Point2d& end, const Vector2d& vecTan,
    Point2d& center, double& radius,
    double* startAngle, double* sweepAngle)
{
    double a, b, c;
    
    // 弦的中垂线方程系数
    a = end.x - start.x;
    b = end.y - start.y;
    c = -0.5 * (a*(end.x + start.x) + b*(end.y + start.y));
    
    // 求中垂线和切线的交点center
    if (!mgCrossLineAbc(a, b, c, vecTan.x, vecTan.y, 
        -vecTan.x * start.x - vecTan.y * start.y, center, Tol::gTol()))
        return false;
    radius = mgHypot(center.x - start.x, center.y - start.y);
    
    if (startAngle != NULL && sweepAngle != NULL)
    {
        a = atan2(start.y - center.y, start.x - center.x);
        b = atan2(end.y - center.y, end.x - center.x);
        *startAngle = a;
        if (vecTan.crossProduct(start - center) > 0.0)
            *sweepAngle = -mgTo0_2PI(a - b);
        else
            *sweepAngle = mgTo0_2PI(b - a);
    }
    
    return true;
}

GEOMAPI bool mgArcBulge(
    const Point2d& start, const Point2d& end, double bulge,
    Point2d& center, double& radius,
    double* startAngle, double* sweepAngle)
{
    Point2d point ((start.x + end.x)*0.5, (start.y + end.y)*0.5);
    point = point.rulerPoint(end, bulge);
    return mgArc3P(start, point, end, center, radius, startAngle, sweepAngle);
}

GEOMAPI bool mgTriEquations(
    Int32 n, double *a, double *b, double *c, Vector2d *vs)
{
    if (!a || !b || !c || !vs || n < 2)
        return false;
    
    double w;
    Int32 i;
    
    w = b[0];
    if (mgIsZero(w))
        return false;
    w = 1.0 / w;
    vs[0].x = vs[0].x * w;
    vs[0].y = vs[0].y * w;
    
    for (i = 0; i <= n-2; i++)
    {
        b[i] = c[i] * w;
        w = b[i+1] - a[i] * b[i];
        if (mgIsZero(w))
            return false;
        w = 1.0 / w;
        vs[i+1].x = (vs[i+1].x - a[i] * vs[i].x) * w;
        vs[i+1].y = (vs[i+1].y - a[i] * vs[i].y) * w;
    }
    
    for (i = n-2; i >= 0; i--)
    {
        vs[i].x -= b[i] * vs[i+1].x;
        vs[i].y -= b[i] * vs[i+1].y;
    }
    
    return true;
}

GEOMAPI bool mgGaussJordan(Int32 n, double *mat, Vector2d *vs)
{
    Int32 i, j, k, m;
    double c, t;
    Vector2d tt;
    
    if (!mat || !vs || n < 2)
        return false;
    
    for (k = 0; k < n; k++)
    {
        // 找主元. 即找第k列中第k行以下绝对值最大的元素
        m = k;
        c = mat[k*n+k];
        for (i = k+1; i < n; i++)
        {
            if (fabs(mat[i*n+k]) > fabs(c))
            {
                m = i;
                c = mat[i*n+k];
            }
        }
        // 交换第k行和第m行中第k列以后的元素
        if (m != k)
        {
            for (j = k; j < n; j++) {
                t = mat[m*n+j]; mat[m*n+j] = mat[k*n+j]; mat[k*n+j] = t; 
            }
            tt = vs[m]; vs[m] = vs[k]; vs[k] = tt;
        }
        // 消元. 第k行中第k列以后元素/=mat[k][k]
        c = mat[k*n+k];
        if (mgIsZero(c))
            return false;
        c = 1.0 / c;
        for (j = k; j < n; j++)
            mat[k*n+j] *= c;
        vs[k].x *= c;
        vs[k].y *= c;
        // 从第k+1行以下每一行, 对该行第k列以后各元素-=
        for (i = k+1; i < n; i++)
        {
            c = mat[i*n+k];
            for (j = k; j < n; j++)
                mat[i*n+j] -= mat[k*n+j] * c;
            vs[i].x -= vs[k].x * c;
            vs[i].y -= vs[k].y * c;
        }
    }
    
    // 回代
    for (i = n-2; i >= 0; i--)
    {
        for (j = i; j < n; j++)
        {
            vs[i].x -= mat[i*n+j+1] * vs[j+1].x;
            vs[i].y -= mat[i*n+j+1] * vs[j+1].y;
        }
    }
    
    return true;
}

static bool CalcCubicClosed(
    Int32 n, double* a, Vector2d* vecs, const Point2d* knots)
{
    Int32 i, n1 = n - 1;

    for (i = n*n - 1; i >= 0; i--)
        a[i] = 0.0;
    
    a[n1] = 1.0;
    a[0]  = 4.0;
    a[1]  = 1.0;
    a[n1*n+n1 - 1] = 1.0;
    a[n1*n+n1]   = 4.0;
    a[n1*n+0]    = 1.0;
    vecs[0].x  = 3.0*(knots[1].x-knots[n1].x);
    vecs[0].y  = 3.0*(knots[1].y-knots[n1].y);
    vecs[n1].x = 3.0*(knots[0].x-knots[n1 - 1].x);
    vecs[n1].y = 3.0*(knots[0].y-knots[n1 - 1].y);
    
    for (i = 1; i < n1; i++)
    {
        a[i*n+i-1] = 1.0;
        a[i*n+i]   = 4.0;
        a[i*n+i+1] = 1.0;
        vecs[i].x = 3.0*(knots[i+1].x-knots[i-1].x);
        vecs[i].y = 3.0*(knots[i+1].y-knots[i-1].y);
    }
    
    return mgGaussJordan(n, a, vecs);
}

static bool CalcCubicUnclosed(
    UInt32 flag, Int32 n, const Point2d* knots, 
    double* a, double* b, double* c, Vector2d* vecs)
{
    if (flag & kCubicTan1)          // 起始夹持端
    {
        b[0] = 1.0;
        c[0] = 0.0;
        //vecs[0] = xp0;            // vecs[0]必须指定切矢量
    }
    else if (flag & kCubicArm1)     // 起始悬臂端
    {
        b[0] = 1.0;
        c[0] = 1.0;
        vecs[0].x = 2.0*(knots[1].x-knots[0].x);
        vecs[0].y = 2.0*(knots[1].y-knots[0].y);
    }
    else                            // 起始自由端
    {
        b[0] = 1.0;
        c[0] = 0.5;
        vecs[0].x = 1.5*(knots[1].x-knots[0].x);
        vecs[0].y = 1.5*(knots[1].y-knots[0].y);
    }
    
    if (flag & kCubicTan2)          // 终止夹持端
    {
        a[n - 2] = 0.0;
        b[n - 1] = 1.0;
        //vecs[n - 1] = xpn;            // vecs[n-1]必须指定切矢量
    }
    else if (flag & kCubicArm2)     // 终止悬臂端
    {
        a[n - 2] = 1.0;
        b[n - 1] = 1.0;
        vecs[n - 1].x = 2.0*(knots[n - 1].x-knots[n - 2].x);
        vecs[n - 1].y = 2.0*(knots[n - 1].y-knots[n - 2].y);
    }
    else                            // 终止自由端
    {
        a[n - 2] = 0.5;
        b[n - 1] = 1.0;
        vecs[n - 1].x = 1.5*(knots[n - 1].x-knots[n - 2].x);
        vecs[n - 1].y = 1.5*(knots[n - 1].y-knots[n - 2].y);
    }
    
    for (int i = 1; i < n - 1; i++)
    {
        a[i-1] = 1.0;
        b[i] = 4.0;
        c[i] = 1.0;
        vecs[i].x = 3.0*(knots[i+1].x-knots[i-1].x);
        vecs[i].y = 3.0*(knots[i+1].y-knots[i-1].y);
    }
    
    return mgTriEquations(n, a, b, c, vecs);
}

GEOMAPI bool mgCubicSplines(
    Int32 n, const Point2d* knots, Vector2d* knotVectors,
    UInt32 flag, double tension)
{
    bool ret = false;
    
    if (!knots || !knotVectors || n < 2)
        return false;
    
    if (flag & kCubicLoop)              // 闭合
    {
        if (n > 256)
            return false;
        double* a = new double[n * n];
        ret = a && CalcCubicClosed(n, a, knotVectors, knots);
        delete[] a;
    }
    else
    {
        double* a = new double[n * 3];
        ret = a && CalcCubicUnclosed(flag, n, knots, 
            a, a+n, a+2*n, knotVectors);
        delete[] a;
    }
    
    if (!mgIsZero(tension - 1.0))
    {
        for (int i = 0; i < n; i++)
        {
            knotVectors[i].x *= tension;
            knotVectors[i].y *= tension;
        }
    }

    return ret;
}

GEOMAPI void mgFitCubicSpline(
                              Int32 n, const Point2d* knots, const Vector2d* knotVectors,
                              Int32 i, double t, Point2d& fitPt)
{
    double b2, b3;
    int i1 = i % n;
    int i2 = (i+1) % n;
    
    b2 = 3*(knots[i2].x - knots[i1].x) - 2*knotVectors[i1].x - knotVectors[i2].x;
    b3 = 2*(knots[i1].x - knots[i2].x) + knotVectors[i1].x + knotVectors[i2].x;
    fitPt.x = knots[i1].x + (knotVectors[i1].x + (b2 + b3*t)*t)*t;
    
    b2 = 3*(knots[i2].y - knots[i1].y) - 2*knotVectors[i1].y - knotVectors[i2].y;
    b3 = 2*(knots[i1].y - knots[i2].y) + knotVectors[i1].y + knotVectors[i2].y;
    fitPt.y = knots[i1].y + (knotVectors[i1].y + (b2 + b3*t)*t)*t;
}

GEOMAPI void mgCubicSplineToBezier(
                                   Int32 n, const Point2d* knots, const Vector2d* knotVectors,
                                   Int32 i, Point2d points[4])
{
    int i1 = i % n;
    int i2 = (i+1) % n;
    points[0] = knots[i1];
    points[1] = knots[i1] + knotVectors[i1] / 3.0;
    points[2] = knots[i2] - knotVectors[i2] / 3.0;
    points[3] = knots[i2];
}

static Int32 RemoveSamePoint(Int32 &n, Point2d* knots, double tol)
{
    for (int i = 0; i < n - 1; i++)
    {
        if (mgHypot(knots[i].x - knots[i+1].x, knots[i].y - knots[i+1].y) < tol)
        {
            for (int j = i + 2; j < n; j++)
                knots[j-1] = knots[j];
            i--;
            n--;
        }
    }
    return n;
}

static void CalcClampedS1n(
    Int32 n1, const Point2d* knots, bool closed, double &len1, 
    double &sx1, double &sy1, double &sxn, double &syn)
{
    double dx, dy, len;

    dx = knots[1].x - knots[0].x;
    dy = knots[1].y - knots[0].y;
    len1 = mgHypot(dx, dy);
    sx1 = dx / len1;
    sy1 = dy / len1;
    if (closed)
    {
        sxn = sx1;
        syn = sy1;
    }
    else
    {
        dx = knots[n1].x - knots[n1 - 1].x;
        dy = knots[n1].y - knots[n1 - 1].y;
        len = mgHypot(dx, dy);
        sxn = dx / len;
        syn = dy / len;
    }
}

static double CalcClampedHp(
    Int32 n1, const Point2d* knots, double* hp, Vector2d* vecs, 
    double len1, double sx1, double sy1, double sxn, double syn)
{
    double dx, dy, dx1, dy1, dx2, dy2, len, s;

    dx1 = sx1;
    dy1 = sy1;
    vecs[0].x = 0.;
    vecs[0].y = 0.;
    hp[0] = len1;
    s = len1;

    for (int i = 1; i < n1; i++)
    {
        dx = knots[i+1].x - knots[i].x;
        dy = knots[i+1].y - knots[i].y;
        len = mgHypot(dx, dy);
        dx2 = dx / len;          // 分段弦斜率
        dy2 = dy / len;
        vecs[i].x = dx2 - dx1;
        vecs[i].y = dy2 - dy1;
        hp[i] = len;
        s += len;
        dx1 = dx2;
        dy1 = dy2;
    }
    vecs[n1].x = sxn - dx1;
    vecs[n1].y = syn - dy1;

    return s;
}

static bool CalcClampedVecs(
    double sigma, bool closed, Int32 n1, const double* hp, 
    double* a, double* b, double* c, Vector2d* vecs)
{
    int i;
    double w, ds, d1, d2;

    ds = sigma * hp[0];
    d1 = sigma * cosh(ds) / sinh(ds) - 1. / hp[0];
    for (i = 0; i < n1; i++)
    {
        ds = sigma * hp[i];
        d2 = sigma * cosh(ds) / sinh(ds) - 1. / hp[i];
        c[i] = 1. / hp[i] - sigma / sinh(ds);
        a[i] = c[i];
        b[i] = d1 + d2;
        d1 = d2;
    }
    if (closed)
        b[n1] = d1;
    else
        b[n1] = d1 + d1;
    
    w = b[0];
    vecs[0].x /= w;
    vecs[0].y /= w;
    for (i = 0; i < n1; i++)
    {
        b[i] = c[i] / w;
        w = b[i+1] - a[i] * b[i];
        if (mgIsZero(w))
        {
            return false;
        }
        vecs[i+1].x = (vecs[i+1].x - a[i] * vecs[i].x) / w;
        vecs[i+1].y = (vecs[i+1].y - a[i] * vecs[i].y) / w;
    }
    for (i = n1 - 1; i >= 0; i--)
    {
        vecs[i].x -= b[i] * vecs[i+1].x;
        vecs[i].y -= b[i] * vecs[i+1].y;
    }

    return true;
}

GEOMAPI bool mgClampedSplines(
    Int32& n, Point2d* knots, 
    double sgm, double tol, double& sigma, double* hp, Vector2d* knotVectors)
{
    Int32 n1;
    double len1, sx1, sy1, sxn, syn, s;
    bool closed;
    
    if (!knots || !knotVectors || !hp || n < 2)
        return false;
    
    closed = (mgHypot(knots[0].x-knots[n-1].x, knots[0].y-knots[n-1].y) < tol);
    
    if (RemoveSamePoint(n, knots, tol) < 2)
        return false;
    
    n1 = n - 1;
    
    // 计算首末端两点之间的斜率(sx1, sy1, sxn, syn)
    CalcClampedS1n(n1, knots, closed, len1, sx1, sy1, sxn, syn);
    
    // 计算累加弦长s、分段弦长hp和方程组右边向量
    s = CalcClampedHp(n1, knots, hp, knotVectors, len1, sx1, sy1, sxn, syn);

    // 规范化张力系数 = 控制参数 / 平均弦长
    sigma = sgm * n1 / s;
    
    double* a = new double[n * 3];
    bool ret = (a != NULL);
    if (ret)
    {
        ret = CalcClampedVecs(sigma, closed, n1, hp, 
            a, a+n, a+2*n, knotVectors);
        delete[] a;
    }
    
    return ret;
}

// 在张力样条曲线的一条弦上插值得到拟和点坐标
// 原理：         x"(s_i)   sinh( sigma * (s_(i+1) - s))
//       x(s) =  ------- * ----------------------------
//                sigma^2   sinh(sigma * h_i)
//
//               x"(s_(i+1))   sinh( sigma * (s - s_i))
//            +  ----------- * ------------------------
//                sigma^2         sinh(sigma * h_i)
//
//            + (x_i - x"(s_i)/sigma^2 ) * (s_(i+1) - s) / h_i
//            + (x_(i+1) - x"(s_(i+1))/sigma^2 ) * (s - s_i) / h_i
//       即 x(t) = xp[i] * sinh(sigma*(hp[i]-t)) / sinh(sigma*hp[i])
//               + xp[i+1] * sinh(sigma*t) / sinh(sigma*hp[i])
//               + (x[i] - xp[i]) * (hp[i]-t) / hp[i]
//               + (x[i+1] - xp[i+1]) * t / hp[i]
    //       s_i <= s <= s_(i+1), 0 <= t <= hp[i]
    //       s_0 = 0, s_i = s_(i-1) + h_i, h_i = | P[i+1]P[i] |
GEOMAPI void mgFitClampedSpline(
                                const Point2d* knots, 
                                Int32 i, double t, double sigma,
                                const double* hp, const Vector2d* knotVectors, Point2d& fitPt)
{
    double s1, s2, s3, tx1, ty1, tx2, ty2;
    double div_hp0 = 1.0 / hp[i];
    
    tx1 = (knots[i].x - knotVectors[i].x)  * div_hp0;
    tx2 = (knots[i+1].x - knotVectors[i+1].x) * div_hp0;
    ty1 = (knots[i].y - knotVectors[i].y)  * div_hp0;
    ty2 = (knots[i+1].y - knotVectors[i+1].y) * div_hp0;
    
    s1 = sinh(sigma * (hp[i] - t));
    s2 = sinh(sigma * t);
    s3 = 1.0 / sinh(sigma * hp[i]);
    
    fitPt.x = (knotVectors[i].x * s1 + knotVectors[i+1].x * s2) *s3 + tx1*(hp[i] - t) + tx2*t;
    fitPt.y = (knotVectors[i].y * s1 + knotVectors[i+1].y * s2) *s3 + ty1*(hp[i] - t) + ty2*t;
}
