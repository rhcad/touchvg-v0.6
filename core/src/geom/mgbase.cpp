// mgbase.cpp: 实现基本计算功能函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "mgbase.h"

// 将数转换到数值范围[tmin, tmax)内
GEOMAPI float mgToRange(float value, float tmin, float tmax)
{
    while (value < tmin)
        value += tmax - tmin;
    while (value >= tmax)
        value -= tmax - tmin;
    return value;
}

// 使角度在[0, 2PI)之间
GEOMAPI float mgTo0_2PI(float angle)
{
    return mgToRange(angle, 0.f, _M_2PI);
}

// 使角度在[-PI, PI)之间
GEOMAPI float mgToPI(float angle)
{
    return mgToRange(angle, -_M_PI, _M_PI);
}

// 角度从度转换到弧度
GEOMAPI float mgDeg2Rad(float deg)
{
    return deg * _M_D2R;
}

// 角度从弧度转换到度
GEOMAPI float mgRad2Deg(float rad)
{
    return rad * _M_R2D;
}

// 度分秒转换到角度(度)
GEOMAPI float mgDms2Deg(float angle)
{
    int nDeg, nMin;
    int sign = 1;
    
    if (angle < 0)
    {
        sign = -1;
        angle = -angle;
    }
    else if (angle == 0)
        return 0.f;
    
    nDeg = static_cast<int>(angle);
    angle -= nDeg;
    angle += 1e-13f;
    angle -= 7e-14f;
    angle *= 100.f;
    nMin = static_cast<int>(angle);
    angle -= nMin;
    angle *= 100.f;
    
    return sign * (nDeg + nMin / 60.f + angle / 3600.f);
}

// 角度(度)转换到度分秒
GEOMAPI float mgDeg2Dms(float angle)
{
    int nDeg, nMin;
    float dSecond;
    int sign = 1;
    
    if (angle < 0)
    {
        sign = -1;
        angle = -angle;
    }
    else if (angle == 0)
        return 0.f;
    
    nDeg = static_cast<int>(angle);
    angle -= nDeg;
    nMin = static_cast<int>(angle * 60.f);
    angle -= nMin / 60.f;
    if (nMin == 60)
    {
        nDeg++;
        nMin = 0;
    }
    dSecond = angle * 3600.f;
    if (dSecond >= 60.f)
    {
        dSecond = 0.f;
        nMin++;
        if (nMin == 60)
        {
            nDeg++;
            nMin = 0;
        }
    }
    return sign * (nDeg + nMin / 100.f + dSecond / 10000.f);
}

// 求两个角度的角平分线角度, [0, 2PI)
GEOMAPI float mgMidAngle(float fromAngle, float toAngle)
{
    fromAngle = mgTo0_2PI(fromAngle);
    toAngle = mgTo0_2PI(toAngle);
    if (!mgIsZero(fromAngle - toAngle))
    {
        if (toAngle < fromAngle)
            return mgTo0_2PI((fromAngle + toAngle + _M_2PI) / 2);
    }
    return (fromAngle + toAngle) / 2;
}

// 求两个角度的夹角, [-PI, PI)
GEOMAPI float mgDiffAngle(float fromAngle, float toAngle)
{
    fromAngle = mgTo0_2PI(fromAngle);
    toAngle = mgTo0_2PI(toAngle);
    if (mgIsZero(fromAngle - toAngle))
        return 0.f;
    if (toAngle < fromAngle)
        toAngle += _M_2PI;
    return mgToPI(toAngle - fromAngle);
}

// 计算最大公约数
GEOMAPI long mgGcd(long x, long y)
{
    while (x != y)
    {
        if (x > y) x -= y;
        else y -= x;
    }
    return x;
}

// 四舍五入. 小数位[-6，7]. eg: mgRoundReal(1.25, 1)=1.3
GEOMAPI double mgRoundReal(double value, int decimal)
{
    if (decimal < -6) decimal = -6;
    else if (decimal > 7) decimal = 7;
    double e10n = (double)pow(10.0L, decimal);
    double l = floor(e10n * value + 0.5);
    return static_cast<double>(l / e10n);
}
