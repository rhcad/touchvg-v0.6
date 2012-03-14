// mglnrel.cpp: 实现直线位置关系函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mglnrel.h"

// 判断点pt是否在有向直线a->b的左边 (开区间)
GEOMAPI bool mgIsLeft(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return (b-a).crossProduct(pt-a) > 0.0;
}

// 判断点pt是否在有向直线a->b的左边
GEOMAPI bool mgIsLeft2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    double dist = (b-a).distanceToVector(pt-a);
    return dist > tol.equalPoint();
}

// 判断点pt是否在有向直线a->b的左边或线上 (闭区间)
GEOMAPI bool mgIsLeftOn(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return (b-a).crossProduct(pt-a) >= 0.0;
}

// 判断点pt是否在有向直线a->b的左边或线上
GEOMAPI bool mgIsLeftOn2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    double dist = (b-a).distanceToVector(pt-a);
    return dist > -tol.equalPoint();
}

// 判断点pt是否在直线a->b的线上
GEOMAPI bool mgIsColinear(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return mgIsZero((b-a).crossProduct(pt-a));
}

// 判断点pt是否在直线a->b的线上
GEOMAPI bool mgIsColinear2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    double dist = (b-a).crossProduct(pt-a);
    return fabs(dist) < tol.equalPoint();
}

// 判断两个线段ab和cd是否相交于线段内部
GEOMAPI bool mgIsIntersectProp(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d)
{
    // Eliminate improper cases
    if (mgIsColinear(a,b,c) || mgIsColinear(a,b,d) 
        || mgIsColinear(c,d,a) || mgIsColinear(c,d,b))
        return false;
    
    return (mgIsLeft(a,b,c) ^ mgIsLeft(a,b,d)) 
        && (mgIsLeft(c,d,a) ^ mgIsLeft(c,d,b));
}

// 判断点pt是否在线段ab上(闭区间)
GEOMAPI bool mgIsBetweenLine(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    if (!mgIsColinear(a, b, pt))
        return false;
    
    // If ab not vertical, check betweenness on x; else on y.
    if (a.x != b.x) 
        return  (a.x <= pt.x && pt.x <= b.x) || (a.x >= pt.x && pt.x >= b.x);
    else
        return  (a.y <= pt.y && pt.y <= b.y) || (a.y >= pt.y && pt.y >= b.y);
}

// 判断点pt是否在线段ab上
GEOMAPI bool mgIsBetweenLine2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    if (!mgIsColinear2(a, b, pt, tol))
        return false;
    
    // If ab not vertical, check betweenness on x; else on y.
    if (a.x != b.x) 
    {
        return ((a.x <= pt.x + tol.equalPoint()) 
            && (pt.x <= b.x + tol.equalPoint())) 
            || ((a.x >= pt.x - tol.equalPoint()) 
            && (pt.x >= b.x - tol.equalPoint()));
    }
    else
    {
        return ((a.y <= pt.y + tol.equalPoint()) 
            && (pt.y <= b.y + tol.equalPoint())) 
            || ((a.y >= pt.y - tol.equalPoint()) 
            && (pt.y >= b.y - tol.equalPoint()));
    }
}

// 已知点pt在直线ab上, 判断点pt是否在线段ab上(闭区间)
GEOMAPI bool mgIsBetweenLine3(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d* ptNear)
{
    bool ret;
    if (a.x != b.x) 
    {
        ret = (a.x <= pt.x && pt.x <= b.x) || (a.x >= pt.x && pt.x >= b.x);
        if (ptNear != NULL)
            *ptNear = fabs(pt.x - a.x) < fabs(pt.x - b.x) ? a : b;
    }
    else
    {
        ret = (a.y <= pt.y && pt.y <= b.y) || (a.y >= pt.y && pt.y >= b.y);
        if (ptNear != NULL)
            *ptNear = fabs(pt.y - a.y) < fabs(pt.y - b.y) ? a : b;
    }
    return ret;
}

// 判断两个线段ab和cd是否相交(交点在线段闭区间内)
GEOMAPI bool mgIsIntersect(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d)
{
    if (mgIsIntersectProp(a, b, c, d))
        return true;
    else if (mgIsBetweenLine(a, b, c) || mgIsBetweenLine(a, b, d) 
        || mgIsBetweenLine(c, d, a) || mgIsBetweenLine(c, d, b))
        return true;
    else
        return false;
}

// 计算点pt到无穷直线ab的距离
GEOMAPI double mgPtToBeeline(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    double dist = (b-a).crossProduct(pt-a);
    return dist;
}

// 计算点pt到无穷直线ab的距离
GEOMAPI double mgPtToBeeline2(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d& ptPerp)
{
    // 两点重合
    if (a == b)
    {
        ptPerp = a;
        return a.distanceTo(pt);
    }
    // 竖直线
    else if (mgIsZero(a.x - b.x))
    {
        ptPerp.set(a.x, pt.y);
        return fabs(a.x - pt.x);
    }
    // 水平线
    else if (mgIsZero(a.y - b.y))
    {
        ptPerp.set(pt.x, a.y);
        return fabs(a.y - pt.y);
    }
    else
    {
        double t1 = ( b.y - a.y ) / ( b.x - a.x );
        double t2 = -1.0 / t1;
        ptPerp.x = ( pt.y - a.y + a.x * t1 - pt.x * t2 ) / ( t1 - t2 );
        ptPerp.y = a.y + (ptPerp.x - a.x) * t1;
        return pt.distanceTo(ptPerp);
    }
}

// 计算点pt到线段ab的最近距离
GEOMAPI double mgPtToLine(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d& ptNear)
{
    Point2d ptTemp;
    double dist = mgPtToBeeline2(a, b, pt, ptNear);
    if (!mgIsBetweenLine3(a, b, ptNear, &ptTemp))
    {
        ptNear = ptTemp;
        dist = pt.distanceTo(ptNear);
    }
    return dist;
}

// 求两条直线(ax+by+c=0)的交点
GEOMAPI bool mgCrossLineAbc(
    double a1, double b1, double c1, double a2, double b2, double c2,
    Point2d& ptCross, const Tol& tolVec)
{
    double sinnum, cosnum;
    
    sinnum = a1*b2 - a2*b1;
    if (mgIsZero(sinnum))
        return false;
    cosnum = a1*a2 + b1*b2;
    if (!mgIsZero(cosnum) && fabs(sinnum / cosnum) < tolVec.equalVector())
        return false;
    
    ptCross.x = (b1*c2 - b2*c1) / sinnum;
    ptCross.y = (a2*c1 - a1*c2) / sinnum;
    
    return true;
}

// 求两条无穷直线的交点
GEOMAPI bool mgCross2Beeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, 
    Point2d& ptCross, double* pu, double* pv, const Tol& tolVec)
{
    double u, v, denom, cosnum;
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom))            // 平行或重合
        return false;
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabs(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    v = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    
    if (pu != NULL) *pu = u;
    if (pv != NULL) *pv = v;
    
    ptCross.x = (1 - u) * a.x + u * b.x;
    ptCross.y = (1 - u) * a.y + u * b.y;
    
    return true;
}

// 求两条线段的交点
// 输入: (a.x,a.y),(b.x,b.y) 第一条线段上的两个点
//         (c.x,c.y),(d.x,d.y) 第二条线段上的两个点
// 输出: (px, py) 交点坐标
// 返回: 有无交点
GEOMAPI bool mgCross2Line(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, const Tol& tolVec)
{
    double u, v, denom, cosnum;
    
    if (mgMin(a.x,b.x) - mgMax(c.x,d.x) > _MGZERO 
        || mgMin(c.x,d.x) - mgMax(a.x,b.x) > _MGZERO
        || mgMin(a.y,b.y) - mgMax(c.y,d.y) > _MGZERO 
        || mgMin(c.y,d.y) - mgMax(a.y,b.y) > _MGZERO)
        return false;
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom))
        return false;
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabs(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    if (u < _MGZERO || u > 1.0 - _MGZERO)
        return false;
    
    v = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    if (v < _MGZERO || v > 1.0 - _MGZERO)
        return false;
    
    ptCross.x = (1 - u) * a.x + u * b.x;
    ptCross.y = (1 - u) * a.y + u * b.y;
    
    return true;
}

// 求线段和直线的交点
GEOMAPI bool mgCrossLineBeeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, double* pv, const Tol& tolVec)
{
    double u, denom, cosnum;
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom))
        return false;
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabs(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    if (u < _MGZERO || u > 1.0 - _MGZERO)
        return false;
    
    if (pv != NULL)
        *pv = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    
    ptCross.x = (1 - u) * a.x + u * b.x;
    ptCross.y = (1 - u) * a.y + u * b.y;
    
    return true;
}

// 线段端点的区域编码:
// 1001  |  1000  |  1010
// 0001  |  0000  |  0010
// 0101  |  0100  |  0110
static inline unsigned ClipCode(Point2d& pt, const Box2d& box)
{
    unsigned code = 0;
    if (pt.y > box.ymax)
        code |= 0x1000;
    else if (pt.y < box.ymin)
        code |= 0x0100;
    if (pt.x < box.xmin)
        code |= 0x0001;
    else if (pt.x > box.xmax)
        code |= 0x0010;
    return code;
}

// 功能: 用矩形剪裁线段
// 参数: [in, out] pt1 线段起点坐标
//       [in, out] pt2 线段终点坐标
//       [in] box 剪裁矩形
// 返回: 剪裁后是否有处于剪裁矩形内的线段部分
GEOMAPI bool mgClipLine(Point2d& pt1, Point2d& pt2, const Box2d& _box)
{
    Box2d box (_box);
    box.normalize();
    
    unsigned code1, code2;
    code1 = ClipCode(pt1, box);
    code2 = ClipCode(pt2, box);
    
    for ( ; ; )
    {
        if (!(code1 | code2))       // 完全在矩形内
            return true;
        if (code1 & code2)          // 完全在矩形外
            return false;
        
        double x = 0.0, y = 0.0;
        unsigned code;
        
        if (code1)                  // 起点不在矩形内
            code = code1;
        else                        // 终点不在矩形内
            code = code2;
        if (code & 0x1000)          // 上
        {
            x = pt1.x + (pt2.x - pt1.x) * (box.ymax - pt1.y) / (pt2.y - pt1.y);
            y = box.ymax;
        }
        else if (code & 0x0100)     // 下
        {
            x = pt1.x + (pt2.x - pt1.x) * (box.ymin - pt1.y) / (pt2.y - pt1.y);
            y = box.ymin;
        }
        else if (code & 0x0001)     // 左
        {
            y = pt1.y + (pt2.y - pt1.y) * (box.xmin - pt1.x) / (pt2.x - pt1.x);
            x = box.xmin;
        }
        else if (code & 0x0010)     // 右
        {
            y = pt1.y + (pt2.y - pt1.y) * (box.xmax - pt1.x) / (pt2.x - pt1.x);
            x = box.xmax;
        }
        
        if (code == code1)
        {
            pt1.x = x;
            pt1.y = y;
            code1 = ClipCode(pt1, box);
        }
        else
        {
            pt2.x = x;
            pt2.y = y;
            code2 = ClipCode(pt2, box);
        }
    }
}

static bool PtInArea_Edge(int &odd, const Point2d& pt, const Point2d& p1, 
                          const Point2d& p2, const Point2d& p0)
{
    // 如果从X方向上P不在边[P1,P2)上，则没有交点. 竖直边也没有
    if (!((p2.x > p1.x) && (pt.x >= p1.x) && (pt.x < p2.x)) &&
        !((p1.x > p2.x) && (pt.x <= p1.x) && (pt.x > p2.x)) )
    {
        return false;
    }
    
    // 求从Y负无穷大向上到P的射线和该边的交点(pt.x, yy)
    double yy = p1.y + (pt.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
    if (pt.y > yy)      // 相交
    {
        if (mgIsZero(pt.x - p1.x))    // 交点是顶点, 则比较P[i+1]和P[i-1]是否在pt.x同侧
        {
            if (((p0.x > pt.x) && (p2.x > pt.x)) ||
                ((p0.x < pt.x) && (p2.x < pt.x)) )      // 同侧
            {
                return false;
            }
        }
        odd = 1 - odd;      // 增加一个交点, 奇偶切换
    }
    
    return true;
}

// 功能: 判断一点是否在一多边形范围内
GEOMAPI int mgPtInArea(
    const Point2d& pt, Int32 count, const Point2d* vertexs, 
    Int32& order, const Tol& tol)
{
    Int32 i;
    int odd = 1;    // 1: 交点数为偶数, 0: 交点数为奇数
    
    for (i = 0; i < count; i++)
    {
        // P与某顶点重合. 返回 kPtAtVertex, order = 顶点号 [0, count-1]
        if (pt.isEqualTo(vertexs[i], tol))
        {
            order = i;
            return kPtAtVertex;
        }
    }
    
    for (i = 0; i < count; i++)
    {
        const Point2d& p1 = vertexs[i];
        const Point2d& p2 = (i+1 < count) ? vertexs[i+1] : vertexs[0];
        
        // P在某条边上. 返回 kPtOnEdge, order = 边号 [0, count-1]
        if (mgIsBetweenLine2(p1, p2, pt, tol))
        {
            order = i;
            return kPtOnEdge;
        }

        if (!PtInArea_Edge(odd, pt, p1, p2, 
            i > 0 ? vertexs[i-1] : vertexs[count-1]))
            continue;
    }

    // 如果射线和多边形的交点数为偶数, 则 p==1, P在区外, 返回 kPtOutArea
    // 为奇数则p==0, P在区内, 返回 kPtInArea
    return 0 == odd ? kPtInArea : kPtOutArea;
}

// 判断多边形是否为凸多边形
GEOMAPI bool mgIsConvex(Int32 count, const Point2d* vs, bool* pACW)
{
    if (count < 3 || vs == NULL)
        return true;
    bool z0 = (vs[count-1].x - vs[count-2].x) * (vs[1].y - vs[0].y)
            > (vs[count-1].y - vs[count-2].y) * (vs[1].x - vs[0].x);
    for (int i = 0; i < count; i++)
    {
        if (z0 != ((vs[i].x - vs[i-1].x) * (vs[i+1].y - vs[i].y)
                 > (vs[i].y - vs[i-1].y) * (vs[i+1].x - vs[i].x)))
            return false;
    }
    if (pACW != NULL)
        *pACW = z0;
    return true;
}
