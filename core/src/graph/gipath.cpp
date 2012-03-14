// gipath.cpp: 实现路径类GiPath
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "gipath.h"
#include <mgcurv.h>

#include <vector>
using std::vector;

// 返回STL数组(vector)变量的元素个数
template<class T> inline static int getSize(T& arr)
{
    return static_cast<int>(arr.size());
}

#ifndef PT_LINETO
#define PT_CLOSEFIGURE      0x01
#define PT_LINETO           0x02
#define PT_BEZIERTO         0x04
#define PT_MOVETO           0x06
#endif // PT_LINETO

//! GiPath的内部数据类
class GiPath::Data
{
public:
    std::vector<Point2d>    points;         //!< 每个节点的坐标
    std::vector<UInt8>      types;          //!< 每个节点的类型, PT_LINETO 等
    int                     beginIndex;     //!< 新图形的起始节点(即MOVETO节点)的序号
};

GiPath::GiPath()
{
    m_data = new Data();
    m_data->beginIndex = -1;
}

GiPath::GiPath(const GiPath& src)
{
    m_data = new Data();

    UInt32 count = src.m_data->points.size();
    m_data->points.reserve(count);
    m_data->types.reserve(count);
    for (UInt32 i = 0; i < count; i++)
    {
        m_data->points.push_back(src.m_data->points[i]);
        m_data->types.push_back(src.m_data->types[i]);
    }
    m_data->beginIndex = src.m_data->beginIndex;
}

GiPath::GiPath(int count, const Point2d* points, const UInt8* types)
{
    m_data = new Data();
    m_data->beginIndex = -1;

    if (count > 0 && points != NULL && types != NULL)
    {
        m_data->points.reserve(count);
        m_data->types.reserve(count);
        for (int i = 0; i < count; i++)
        {
            m_data->points.push_back(points[i]);
            m_data->types.push_back(types[i]);
        }
    }
}

GiPath::~GiPath()
{
    delete m_data;
}

GiPath& GiPath::operator=(const GiPath& src)
{
    if (this != &src)
    {
        clear();
        UInt32 count = src.m_data->points.size();
        m_data->points.reserve(count);
        m_data->types.reserve(count);
        for (UInt32 i = 0; i < count; i++)
        {
            m_data->points.push_back(src.m_data->points[i]);
            m_data->types.push_back(src.m_data->types[i]);
        }
        m_data->beginIndex = src.m_data->beginIndex;
    }
    return *this;
}

int GiPath::getCount() const
{
    return getSize(m_data->points);
}

const Point2d* GiPath::getPoints() const
{
    return m_data->points.size() > 0 ? &m_data->points.front() : NULL;
}

const UInt8* GiPath::getTypes() const
{
    return m_data->types.size() > 0 ? &m_data->types.front() : NULL;
}

void GiPath::clear()
{
    m_data->points.clear();
    m_data->types.clear();
    m_data->beginIndex = -1;
}

void GiPath::transform(const Matrix2d& mat)
{
    for (UInt32 i = 0; i < m_data->points.size(); i++)
    {
        m_data->points[i] *= mat;
    }
}

void GiPath::startFigure()
{
    m_data->beginIndex = -1;
}

bool GiPath::moveTo(const Point2d& point)
{
    m_data->points.push_back(point);
    m_data->types.push_back(PT_MOVETO);
    m_data->beginIndex = getSize(m_data->points) - 1;

    return true;
}

bool GiPath::lineTo(const Point2d& point)
{
    bool ret = (m_data->beginIndex >= 0);
    if (ret)
    {
        m_data->points.push_back(point);
        m_data->types.push_back(PT_LINETO);
    }

    return ret;
}

bool GiPath::linesTo(int count, const Point2d* points)
{
    bool ret = (m_data->beginIndex >= 0 && count > 0 && points != NULL);
    if (ret)
    {
        for (int i = 0; i < count; i++)
        {
            m_data->points.push_back(points[i]);
            m_data->types.push_back(PT_LINETO);
        }
    }

    return ret;
}

bool GiPath::beziersTo(int count, const Point2d* points)
{
    bool ret = (m_data->beginIndex >= 0 && count > 0 && points != NULL
        && (count % 3) == 0);
    if (ret)
    {
        for (int i = 0; i < count; i++)
        {
            m_data->points.push_back(points[i]);
            m_data->types.push_back(PT_BEZIERTO);
        }
    }

    return ret;
}

bool GiPath::arcTo(const Point2d& point)
{
    bool ret = false;

    if (m_data->beginIndex >= 0 
        && getSize(m_data->points) >= m_data->beginIndex + 2
        && m_data->points.size() == m_data->types.size())
    {
        Point2d start = m_data->points[m_data->points.size() - 1];
        Vector2d vecTan = start - m_data->points[m_data->points.size() - 2];
        Point2d center;
        double radius, startAngle, sweepAngle;

        if (mgArcTan(start, point, vecTan, center, radius, &startAngle, &sweepAngle))
        {
            Point2d pts[16];
            int n = mgAngleArcToBezier(pts, center, radius, radius, 
                startAngle, sweepAngle);
            if (n >= 4)
            {
                ret = true;
                for (int i = 0; i < n; i++)
                {
                    m_data->points.push_back(pts[i]);
                    m_data->types.push_back(PT_BEZIERTO);
                }
            }
        }
    }

    return ret;
}

bool GiPath::arcTo(const Point2d& point, const Point2d& end)
{
    bool ret = false;

    if (m_data->beginIndex >= 0 
        && getSize(m_data->points) >= m_data->beginIndex + 1
        && m_data->points.size() == m_data->types.size())
    {
        Point2d start = m_data->points[m_data->points.size() - 1];
        Point2d center;
        double radius, startAngle, sweepAngle;

        if (mgArc3P(start, point, end, center, radius, &startAngle, &sweepAngle))
        {
            Point2d pts[16];
            int n = mgAngleArcToBezier(pts, center, radius, radius, 
                startAngle, sweepAngle);
            if (n >= 4)
            {
                ret = true;
                for (int i = 0; i < n; i++)
                {
                    m_data->points.push_back(pts[i]);
                    m_data->types.push_back(PT_BEZIERTO);
                }
            }
        }
    }

    return ret;
}

bool GiPath::closeFigure()
{
    bool ret = false;

    if (m_data->beginIndex >= 0 
        && getSize(m_data->points) >= m_data->beginIndex + 3
        && m_data->points.size() == m_data->types.size())
    {
        UInt8 type = m_data->types[m_data->types.size() - 1];
        if (type == PT_LINETO || type == PT_BEZIERTO)
        {
            m_data->types[m_data->types.size() - 1] |= PT_CLOSEFIGURE;
            m_data->beginIndex = -1;
            ret = true;
        }
    }

    return ret;
}

static int AngleToBezier(Point2d* pts, double radius)
{
    const Vector2d vec1 (pts[1] - pts[0]);      // 第一条边
    const Vector2d vec2 (pts[2] - pts[1]);      // 第二条边

    const double dHalfAngle = 0.5 * fabs(vec1.angleTo2(vec2));  // 夹角的一半
    if (dHalfAngle < 1e-4 || fabs(dHalfAngle - M_PI_2) < 1e-4)  // 两条边平行
        return 0;

    const double dDist1 = 0.5 * vec1.length();
    const double dDist2 = 0.5 * vec2.length();
    double dArc = radius / tan(dHalfAngle);    // 圆弧在边上的投影长度
    if (dArc > dDist1 || dArc > dDist2)
    {
        double dArcOld = dArc;
        dArc = mgMin(dDist1, dDist2);
        if (dArc < dArcOld * 0.5)
            return 3;
    }

    int count = 0;
    Point2d ptCenter, ptStart, ptEnd;
    double startAngle, sweepAngle;

    ptStart = pts[1].rulerPoint(pts[0], dArc, 0);
    ptEnd = pts[1].rulerPoint(pts[2], dArc, 0);
    if (mgArcTan(ptStart, ptEnd, pts[1] - ptStart, 
        ptCenter, radius, &startAngle, &sweepAngle))
    {
        count = mgAngleArcToBezier(
            pts, ptCenter, radius, radius, startAngle, sweepAngle);
    }

    return count;
}

bool GiPath::genericRoundLines(int count, const Point2d* points, 
                               double radius, bool closed)
{
    clear();

    if (count < 3 || NULL == points || radius < _MGZERO)
        return false;

    Point2d ptsBzr[16];
    int nBzrCnt;

    if (closed)
    {
        ptsBzr[0] = points[count - 1];
        ptsBzr[1] = points[0];
        ptsBzr[2] = points[1];
        nBzrCnt = AngleToBezier(ptsBzr, radius);
        if (nBzrCnt < 4)
        {
            this->moveTo(points[0]);
        }
        else
        {
            this->moveTo(ptsBzr[0]);
            this->beziersTo(nBzrCnt - 1, ptsBzr + 1);
        }
    }
    else
    {
        this->moveTo(points[0]);
    }

    for (int i = 1; i < (closed ? count : count - 1); i++)
    {
        ptsBzr[0] = points[i - 1];
        ptsBzr[1] = points[i];
        ptsBzr[2] = points[(i + 1) % count];
        nBzrCnt = AngleToBezier(ptsBzr, radius);
        if (nBzrCnt < 4)
        {
            this->lineTo(points[i]);
        }
        else
        {
            this->lineTo(ptsBzr[0]);
            this->beziersTo(nBzrCnt - 1, ptsBzr + 1);
        }
    }

    if (closed)
        this->closeFigure();
    else
        this->lineTo(points[count - 1]);

    return true;
}
