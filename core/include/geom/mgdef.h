//! \file mgdef.h
//! \brief 定义数学几何库的常量和宏
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGDEF_H_
#define __GEOMETRY_MGDEF_H_

#include <math.h>
#include "mgtype.h"

#define GEOMAPI         extern "C"

#ifndef NULL
#define NULL  0
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4514)  // unreferenced inlines are common
#pragma warning(disable: 4710)  // private constructors are disallowed
#pragma warning(disable: 4251)  // using non-exported as public in exported
#endif

//! 定义可以视为零的极小数
const double _MGZERO    = 1e-12;

//! 判断一个浮点数是否可以认为是零
/*! 判断两个浮点数是否相等不能直接比较相等，应该用mgIsZero(d1-d2)判断
    \ingroup _GEOMAPI_BASIC_
    \see _MGZERO
*/
inline bool mgIsZero(double v)
{
    return fabs(v) < _MGZERO;
}

// 定义常用的数学常数
#ifndef M_PI
const double M_E        = 2.71828182845904523536028747135266;      //!< e
const double M_LOG2E    = 1.4426950408889634073599246810019;       //!< log2(e)
const double M_LOG10E   = 0.434294481903251827651128918916605;     //!< log(e)
const double M_LN2      = 0.693147180559945309417232121458177;     //!< ln(2)
const double M_LN10     = 2.30258509299404568401799145468436;      //!< ln(10)
const double M_PI       = 3.14159265358979323846264338327950288419716939937511;  //!< 圆周率PI，180°
const double M_PI_2     = 1.57079632679489661923132169163975144209858469968756;  //!< PI/2，90°
const double M_PI_4     = 0.785398163397448309615660845819875721049292349843778; //!< PI/4，45°
const double M_1_PI     = 0.318309886183790671537767526745028724068919291480913; //!< 1/PI
const double M_2_PI     = 0.636619772367581343075535053490057448137838582961826; //!< 2/PI
const double M_2_SQRTPI = 1.12837916709551257389615890312155;                    //!< 2/sqrt(PI)
const double M_SQRT2    = 1.41421356237309504880168872420969807856967187537695;  //!< sqrt(2)
const double M_SQRT1_2  = 0.707106781186547524400844362104849039284835937688474; //!< sqrt(2)/2
#endif
const double _M_2PI     = 6.28318530717958647692528676655900576839433879875022;  //!< 2PI，360°
const double _M_PI_3    = 1.04719755119659774615421446109316762806572313312504;  //!< PI/3，60°
const double _M_PI_6    = 0.523598775598298873077107230546583814032861566562518; //!< PI/6，30°
const double _M_D2R     = 0.0174532925199432957692369076848861271344287188854173; //!< 度到弧度的转换常数，PI/180
const double _M_R2D     = 57.2957795130823208767981548141051703324054724665643;  //!< 弧度到度的转换常数，180/PI
const double _M_1_SQRPI = 0.564189583547756286948079451560773;                   //!< 1/sqrt(PI)

const double _DBL_MIN   = 2.2250738585072014e-308;  //!< 双精度浮点数最小绝对值
const double _DBL_MAX   = 1.7976931348623158e+308;  //!< 双精度浮点数最大绝对值
const float  _FLT_MIN   = 1.175494351e-38F;         //!< 单精度浮点数最小绝对值
const float  _FLT_MAX   = 3.402823466e+38F;         //!< 单精度浮点数最大绝对值

//! 返回两个同类型的数中较小的数的引用
/*!
    \ingroup _GEOMAPI_BASIC_
*/
template<class _T> inline
const _T& mgMin(const _T& t1, const _T& t2)
{
    return (t1 < t2) ? t1 : t2;
}

//! 返回两个同类型的数中较大的数的引用
/*!
    \ingroup _GEOMAPI_BASIC_
*/
template<class _T> inline
const _T& mgMax(const _T& t1, const _T& t2)
{
    return (t1 > t2) ? t1 : t2;
}

//! 交换两个同类型的数
/*!
    \ingroup _GEOMAPI_BASIC_
*/
template<class _T> inline
void mgSwap(_T& t1, _T& t2)
{
    _T tmp = t1; t1 = t2; t2 = tmp;
}

//! 求直角三角形的斜边长度
/*! 不用math.h中的hypot函数是因为在不溢出的情况下该函数执行快些
    \ingroup _GEOMAPI_BASIC_
*/
inline double mgHypot(double x, double y)
{
    return sqrt(x*x + y*y);
}

//! 求两个数的平方和
/*!
    \ingroup _GEOMAPI_BASIC_
*/
inline double mgSquare(double x, double y)
{
    return (x*x + y*y);
}

//! 对一个浮点数四舍五入到整数
/*! 对负数是按其绝对值进行舍入的
    \ingroup _GEOMAPI_BASIC_
    \see mgRoundReal
*/
inline long mgRound(double d)
{
    return d < 0 ? -(long)(-d + 0.5) : (long)(d + 0.5);
}

#endif // __GEOMETRY_MGDEF_H_
