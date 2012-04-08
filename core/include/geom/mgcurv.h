//! \file mgcurv.h
//! \brief 定义曲线计算函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_FITCURVE_H_
#define __GEOMETRY_FITCURVE_H_

#include "mgbox.h"

//! 计算三次贝塞尔曲线段的参数点
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] pts 4个点的数组，为贝塞尔曲线段的控制点
    \param[in] t 要计算的参数点的参数，范围为[0, 1]
    \param[out] ptFit 计算出的参数点
    \see mgBezier4P, mgCubicSplines
*/
GEOMAPI void mgFitBezier(const Point2d* pts, double t, Point2d& ptFit);

//! 用线上四点构成三次贝塞尔曲线段
/*! 该贝塞尔曲线段的起点和终点为给定点，中间经过另外两个给定点，
    t=1/3过pt2, t=2/3过pt3。
    计算出的贝塞尔曲线段的起点为pt1、终点为pt4，中间两个控制点为ptCtr1、ptCtr2
    \ingroup GEOMAPI_CURVE
    \param[in] pt1 线的起点
    \param[in] pt2 线上的第一个中间点
    \param[in] pt3 线上的第二个中间点
    \param[in] pt4 线的终点
    \param[out] ptCtr1 中间第一个控制点
    \param[out] ptCtr2 中间第二个控制点
    \see mgEllipse90ToBezier, mgEllipseToBezier, mgAngleArcToBezier
*/
GEOMAPI void mgBezier4P(
    const Point2d& pt1, const Point2d& pt2, const Point2d& pt3, 
    const Point2d& pt4, Point2d& ptCtr1, Point2d& ptCtr2);

//! 用给定的起点和终点构造90度椭圆弧，并转换为一个三次贝塞尔曲线段
/*! 椭圆弧为从起点到终点逆时针转90度。
    计算出的贝塞尔曲线段的起点为pt1、终点为pt4，中间两个控制点为ptCtr1、ptCtr2
    \ingroup GEOMAPI_CURVE
    \param[in] ptFrom 起点
    \param[in] ptTo 终点
    \param[out] ptCtr1 中间第一个控制点
    \param[out] ptCtr2 中间第二个控制点
    \see mgBezier4P, mgEllipseToBezier, mgAngleArcToBezier
*/
GEOMAPI void mgEllipse90ToBezier(
    const Point2d& ptFrom, const Point2d& ptTo, Point2d& ptCtr1, Point2d& ptCtr2);

//! 将一个椭圆转换为4段三次贝塞尔曲线
/*! 4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，
    第一个点和最后一个点重合于+X轴上点(rx, 0)。
    \ingroup GEOMAPI_CURVE
    \param[out] points 贝塞尔曲线的控制点，13个点
    \param[in] center 椭圆心
    \param[in] rx 半长轴的长度
    \param[in] ry 半短轴的长度
    \see mgBezier4P, mgEllipse90ToBezier, mgAngleArcToBezier
*/
GEOMAPI void mgEllipseToBezier(
    Point2d points[13], const Point2d& center, double rx, double ry);

//! 将一个圆角矩形转换为4段三次贝塞尔曲线
/*! 这4段贝塞尔曲线按逆时针方向从第一象限到第四象限，每段4个点，
    第一段的点序号为[0,1,2,3]，其余段递增类推。
    \ingroup GEOMAPI_CURVE
    \param[out] points 贝塞尔曲线的控制点，每段4个点，共16个点
    \param[in] rect 矩形外框，规范化矩形
    \param[in] rx X方向的圆角半径，非负数
    \param[in] ry Y方向的圆角半径，非负数
    \see mgEllipseToBezier
*/
GEOMAPI void mgRoundRectToBeziers(
    Point2d points[16], const Box2d& rect, double rx, double ry);

//! 将一个椭圆弧转换为多段三次贝塞尔曲线
/*! 4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，每一段4个点，
    第一个点和最后一个点重合于+X轴上点(rx, 0)。
    \ingroup GEOMAPI_CURVE
    \param[out] points 贝塞尔曲线的控制点，16个点
    \param[in] center 椭圆心
    \param[in] rx 半长轴的长度
    \param[in] ry 半短轴的长度，为0则取为rx
    \param[in] startAngle 起始角度，弧度，相对于+X轴，逆时针为正
    \param[in] sweepAngle 转角，弧度，相对于起始角度，逆时针为正
    \return 计算后的控制点数，点数小于4则给定参数有错误
    \see mgBezier4P, mgEllipse90ToBezier, mgEllipseToBezier, mgArc3P
*/
GEOMAPI int mgAngleArcToBezier(
    Point2d points[16], const Point2d& center, double rx, double ry,
    double startAngle, double sweepAngle);

//! 给定起点、弧上一点和终点，计算圆弧参数
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] start 圆弧的起点
    \param[in] point 弧上的一点
    \param[in] end 圆弧的终点
    \param[out] center 圆心
    \param[out] radius 半径
    \param[out] startAngle 填充起始角度，为NULL则忽略该参数
    \param[out] sweepAngle 填充圆弧转角，逆时针为正，为NULL则忽略该参数
    \return 是否计算成功
    \see mgArcTan, mgArcBulge, mgAngleArcToBezier
*/
GEOMAPI bool mgArc3P(
    const Point2d& start, const Point2d& point, const Point2d& end,
    Point2d& center, double& radius,
    double* startAngle = NULL, double* sweepAngle = NULL);

//! 给定起点、终点和起点切向，计算圆弧参数
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] start 圆弧的起点
    \param[in] end 圆弧的终点
    \param[in] vecTan 圆弧的起点处的切向矢量
    \param[out] center 圆心
    \param[out] radius 半径
    \param[out] startAngle 填充起始角度，为NULL则忽略该参数
    \param[out] sweepAngle 填充圆弧转角，逆时针为正，为NULL则忽略该参数
    \return 是否计算成功
    \see mgArc3P, mgArcBulge, mgAngleArcToBezier
*/
GEOMAPI bool mgArcTan(
    const Point2d& start, const Point2d& end, const Vector2d& vecTan,
    Point2d& center, double& radius,
    double* startAngle = NULL, double* sweepAngle = NULL);

//! 给定弦和拱高计算圆弧参数
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] start 圆弧的起点
    \param[in] end 圆弧的终点
    \param[in] bulge 拱高，正数为逆时针方向，负数为顺时针方向
    \param[out] center 圆心
    \param[out] radius 半径
    \param[out] startAngle 填充起始角度，为NULL则忽略该参数
    \param[out] sweepAngle 填充圆弧转角，逆时针为正，为NULL则忽略该参数
    \return 是否计算成功
    \see mgArc3P, mgArcTan, mgAngleArcToBezier
*/
GEOMAPI bool mgArcBulge(
    const Point2d& start, const Point2d& end, double bulge,
    Point2d& center, double& radius,
    double* startAngle = NULL, double* sweepAngle = NULL);

//! 求解三对角线方程组
/*! 三对角线方程组如下所示: \n
    　　　| b0　　　c0　　　　　　| \n
    A　=　| a0　　　b1　　　c1　　| \n
    　　　|　　..　　　..　　.. 　| \n
    　　　|　　　a[n-2]　　b[n-1] | \n
    A * (x,y) = (rx,ry)

    \ingroup GEOMAPI_BASIC
    \param[in] n 方程组阶数，最小为2
    \param[in] a 系数矩阵中的左对角线元素数组，a[0..n-2]
    \param[in,out] b 系数矩阵中的中对角线元素数组，b[0..n-1]，会被修改
    \param[in] c 系数矩阵中的右对角线元素数组，c[0..n-2]
    \param[in,out] vs 输入方程组等号右边的已知n个矢量，输出求解出的未知矢量
    \return 是否求解成功，失败原因可能是参数错误或因系数矩阵非主角占优而出现除零
    \see mgGaussJordan
*/
GEOMAPI bool mgTriEquations(
    Int32 n, double *a, double *b, double *c, Vector2d *vs);

//! Gauss-Jordan法求解线性方程组
/*!
    \ingroup GEOMAPI_BASIC
    \param[in] n 方程组阶数，最小为2
    \param[in,out] mat 系数矩阵，n维方阵，会被修改
    \param[in,out] vs 输入方程组等号右边的已知n个矢量，输出求解出的未知矢量
    \return 是否求解成功，失败原因可能是参数错误或因系数矩阵非主角占优而出现除零
    \see mgTriEquations
*/
GEOMAPI bool mgGaussJordan(Int32 n, double *mat, Vector2d *vs);

//! 三次参数样条曲线的端点条件
//! \see mgCubicSplines
enum kCubicSplinesFlags
{
    kCubicTan1 = 1,         //!< 起始夹持端
    kCubicArm1 = 2,         //!< 起始悬臂端
    kCubicTan2 = 4,         //!< 终止夹持端
    kCubicArm2 = 8,         //!< 终止悬臂端
    kCubicLoop = 16,        //!< 闭合, 有该值时忽略其他组合值
};

//! 计算三次参数样条曲线的型值点的切矢量
/*! 三次参数样条曲线的分段曲线方程为：\n
    P[i](t) = knots[i] + knotVectors[i] * t \n
    　　　+ (3*(knots[i+1] - knots[i]) - 2 * knotVectors[i] - knotVectors[i+1]) * t^2 \n
    　　　+ (2*(knots[i] - knots[i+1]) + knotVectors[i] + knotVectors[i+1]) * t^3 \n
    其中 0 ≤ t ≤ 1

    \ingroup GEOMAPI_CURVE
    \param[in] n 型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[out] knotVectors 型值点的切矢量数组，元素个数为n，由外界分配内存
    \param[in] flag 曲线边界条件，由 kCubicSplinesFlags 各种值组合而成。\n
        指定 kCubicTan1 时, knotVectors[0]必须指定有效的切矢量；\n
        指定 kCubicTan2 时, knotVectors[n-1]必须指定有效的切矢量。\n
        指定 kCubicLoop 时，knots的首末型值点不必重合，计算中将首末型值点视为任意两点。
    \param[in] tension 张力系数，0≤coeff≤1, 为1时C2阶连续, 为0时成折线
    \return 是否计算成功
    \see kCubicSplinesFlags, mgFitCubicSpline, mgCubicSplinesBox
*/
GEOMAPI bool mgCubicSplines(
    Int32 n, const Point2d* knots, Vector2d* knotVectors,
    UInt32 flag = 0, double tension = 1.0);

//! 在三次样条曲线的一条弦上插值得到拟和点坐标
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotVectors 型值点的切矢量数组，元素个数为n
    \param[in] i 分段曲线序号，在0到(n-2)之间，如果曲线是闭合条件，则可取到(n-1)
    \param[in] t 分段曲线函数参数，在0到1之间
    \param[out] fitPt 拟和点坐标，第i段曲线上参数t对应的曲线坐标
    \see mgCubicSplines, mgCubicSplineToBezier
*/
GEOMAPI void mgFitCubicSpline(
    Int32 n, const Point2d* knots, const Vector2d* knotVectors,
    Int32 i, double t, Point2d& fitPt);

//! 得到三次样条曲线的分段贝塞尔曲线段控制点
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotVectors 型值点的切矢量数组，元素个数为n
    \param[in] i 分段曲线序号，在0到(n-2)之间，如果曲线是闭合条件，则可取到(n-1)
    \param[out] points 贝塞尔曲线段的控制点，4个点
    \see mgCubicSplines, mgFitCubicSpline
*/
GEOMAPI void mgCubicSplineToBezier(
    Int32 n, const Point2d* knots, const Vector2d* knotVectors,
    Int32 i, Point2d points[4]);

//! 得到三次B样条曲线的分段贝塞尔曲线段控制点
/*!
    \ingroup GEOMAPI_CURVE
    \param[out] points 贝塞尔曲线的控制点，要预先分配(1+n*3)个点的空间
    \param n B样条曲线控制点的点数，至少为4
    \param controlPoints B样条曲线控制点坐标数组，点数为n
    \param closed 三次B样条曲线是否为闭合曲线
    \return 实际转换的贝塞尔曲线控制点的个数
*/
GEOMAPI Int32 mgBSplinesToBeziers(
    Point2d points[/*1+n*3*/], Int32 n, const Point2d* controlPoints, bool closed);

//! 计算张力样条曲线的型值点参数和弦长
/*!
    \ingroup GEOMAPI_CURVE
    \param[in,out] n 型值点的点数，如果有重合点则输出实际点数
    \param[in,out] knots 型值点坐标数组，元素个数为n，有重合点则会删除点坐标
    \param[in] sgm 控制参数，>0，一般取1.5
    \param[in] tol 长度容差值，用于判断重合点
    \param[out] sigma 规范化张力系数，= 控制参数 / 平均弦长
    \param[out] hp 弦长数组，元素个数为n-1或n，由外界分配内存
    \param[out] knotVectors 型值点的f"(x_i)/sigma^2，元素个数为n，由外界分配内存
    \return 是否计算成功
    \see mgFitClampedSpline
*/
GEOMAPI bool mgClampedSplines(
    Int32& n, Point2d* knots, double sgm, double tol, double& sigma,
    double* hp, Vector2d* knotVectors);

//! 在张力样条曲线的一条弦上插值得到拟和点坐标
/*!
    \ingroup GEOMAPI_CURVE
    \param[in] knots 型值点坐标数组
    \param[in] i 分段曲线序号，在0到(型值点的点数-1)之间
    \param[in] t 分段曲线函数参数，在0到hp[i]之间
    \param[in] sigma 规范化张力系数
    \param[in] hp 弦长数组
    \param[in] knotVectors 型值点的f"(x_i)/sigma^2数组
    \param[out] fitPt 拟和点坐标，第i段曲线上参数t对应的曲线坐标
    \see mgClampedSplines
*/
GEOMAPI void mgFitClampedSpline(
    const Point2d* knots, Int32 i, double t, double sigma,
    const double* hp, const Vector2d* knotVectors, Point2d& fitPt);

#endif // __GEOMETRY_FITCURVE_H_
