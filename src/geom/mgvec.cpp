// mgvec.cpp: 实现二维矢量类Vector2d
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "mgpnt.h"

_GEOM_BEGIN

// 移动直尺法，以本点向dir方向点沿线为直尺边，在垂直沿线方向偏移yoff
Point2d Point2d::rulerPoint(const Point2d& dir, double yoff) const
{
    double len = distanceTo(dir);
    if (len < _MGZERO)
        return Point2d(x, y + yoff);
    else
    {
        yoff /= len;
        return Point2d(x - (dir.y - y) * yoff, y + (dir.x - x) * yoff);
    }
}

// 移动直尺法，dir方向点，在沿线方向偏移xoff，垂直沿线方向偏移yoff
Point2d Point2d::rulerPoint(const Point2d& dir, double xoff, double yoff) const
{
    double len = distanceTo(dir);
    if (len < _MGZERO)
        return Point2d(x + xoff, y + yoff);
    else
    {
        double dcos = (dir.x - x) / len;
        double dsin = (dir.y - y) / len;
        return Point2d(x + xoff * dcos - yoff * dsin, 
            y + xoff * dsin + yoff * dcos);
    }
}

// 判断两个矢量是否平行
bool Vector2d::isParallelTo(const Vector2d& vec, const Tol& tol) const
{
    double cosfz = dotProduct(vec);
    double sinfz = crossProduct(vec);
    return (fabs(sinfz) <= fabs(cosfz) * tol.equalVector());
}

// 判断两个矢量是否平行
bool Vector2d::isParallelTo(const Vector2d& vec, 
                            const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    double cosfz = dotProduct(vec);
    double sinfz = crossProduct(vec);
    if (fabs(sinfz) <= fabs(cosfz) * tol.equalVector())
    {
        if (mgIsZero(cosfz))
            nonzero = false;
        ret = true;
    }
    return ret;
}

// 判断两个矢量是否同向
bool Vector2d::isCodirectionalTo(const Vector2d& vec, const Tol& tol) const
{
    double cosfz = dotProduct(vec);
    if (cosfz < -_MGZERO)          // 同向则cos(夹角)接近1.0
        return false;
    double sinfz = crossProduct(vec);
    return (fabs(sinfz) <= cosfz * tol.equalVector());
}

// 判断两个矢量是否同向
bool Vector2d::isCodirectionalTo(const Vector2d& vec, 
                                 const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    double cosfz = dotProduct(vec);
    double sinfz = crossProduct(vec);
    if (fabs(sinfz) <= fabs(cosfz) * tol.equalVector())
    {
        if (fabs(cosfz) < _MGZERO)
        {
            nonzero = false;
            ret = true;
        }
        else
            ret = (cosfz >= -_MGZERO);
    }
    return ret;
}

// 判断两个矢量是否反向
bool Vector2d::isOppositeTo(const Vector2d& vec, const Tol& tol) const
{
    double cosfz = dotProduct(vec);
    if (cosfz > -_MGZERO)          // 反向则cos(夹角)接近-1.0
        return false;
    double sinfz = crossProduct(vec);
    return (fabs(sinfz) <= (-cosfz) * tol.equalVector());
}

// 判断两个矢量是否反向
bool Vector2d::isOppositeTo(const Vector2d& vec, 
                            const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    double cosfz = dotProduct(vec);
    double sinfz = crossProduct(vec);
    if (fabs(sinfz) <= fabs(cosfz) * tol.equalVector())
    {
        if (fabs(cosfz) < _MGZERO)
        {
            nonzero = false;
            ret = false;
        }
        else
            ret = cosfz < -_MGZERO;
    }
    return ret;
}

// 判断两个矢量是否垂直
bool Vector2d::isPerpendicularTo(const Vector2d& vec, const Tol& tol) const
{
    double sinfz = fabs(crossProduct(vec));
    if (sinfz < _MGZERO)
        return false;
    double cosfz = fabs(dotProduct(vec));
    return (cosfz <= sinfz * tol.equalVector());
}

// 判断两个矢量是否垂直
bool Vector2d::isPerpendicularTo(const Vector2d& vec, 
                                 const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    double sinfz = fabs(crossProduct(vec));
    double cosfz = fabs(dotProduct(vec));
    if (cosfz <= sinfz * tol.equalVector())
    {
        ret = (sinfz >= _MGZERO);
        nonzero = ret;
    }
    return ret;
}

// 求本矢量投影到矢量xAxis上的垂直距离
// 在xAxis的逆时针方向时返回正值，顺时针则返回负值
double Vector2d::distanceToVector(const Vector2d& xAxis) const
{
    double len = xAxis.length();
    if (len < _MGZERO)
        return length();
    return xAxis.crossProduct(*this) / len;
}

// 求本矢量在矢量xAxis上的投影比例, 投影矢量 = xAxis * 投影比例
double Vector2d::projectScaleToVector(const Vector2d& xAxis) const
{
    double d2 = xAxis.lengthSqrd();
    if (d2 < _MGZERO)
        return 0.0;
    return dotProduct(xAxis) / d2;
}

// 求本矢量在矢量xAxis上的投影矢量和垂直矢量
double Vector2d::projectResolveVector(const Vector2d& xAxis, 
                                      Vector2d& proj, Vector2d& perp) const
{
    double s = projectScaleToVector(xAxis);
    proj = xAxis * s;
    perp = *this - proj;
    return s;
}

// 将本矢量在两个不共线的非零矢量上进行矢量分解, vec = u*uAxis+v*vAxis
bool Vector2d::resolveVector(const Vector2d& uAxis, const Vector2d& vAxis, 
                             double& u, double& v) const
{
    double denom = uAxis.crossProduct(vAxis);
    if (mgIsZero(denom))
    {
        u = 0.0; v = 0.0;
        return false;
    }
    u = crossProduct(vAxis) / denom;
    v = uAxis.crossProduct(*this) / denom;
    return true;
}

_GEOM_END