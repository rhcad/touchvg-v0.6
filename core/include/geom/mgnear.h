//! \file mgnear.h
//! \brief 定义求曲线上最近点的函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_CURVENEAR_H_
#define __GEOMETRY_CURVENEAR_H_

#include "mgrect.h"

//! 计算一点到三次贝塞尔曲线段上的最近点
/*!
    \ingroup _GEOMAPI_CURVE_
    \param[in] pt 曲线段外给定的点
    \param[in] pts 三次贝塞尔曲线段的控制点，4个点
    \param[out] ptNear 曲线段上的最近点
*/
GEOMAPI void mgNearestOnBezier(
    const Point2d& pt, const Point2d* pts, Point2d& ptNear);

//! 计算贝塞尔曲线的绑定框
/*!
    \ingroup _GEOMAPI_CURVE_
    \param[out] box 绑定框
    \param[in] count 点的个数，至少为4，必须为3的倍数加1
    \param[in] points 控制点和端点的数组，点数为count
    \param[in] closed 是否为闭合曲线
    \see mgCubicSplines
*/
GEOMAPI void mgBeziersBox(
    Box2d& box, Int32 count, const Point2d* points, bool closed = false);

//! 计算三次样条曲线的绑定框
/*!
    \ingroup _GEOMAPI_CURVE_
    \param[out] box 绑定框
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotVectors 型值点的切矢量数组，元素个数为n
    \param[in] closed 是否为闭合曲线
    \see mgCubicSplines
*/
GEOMAPI void mgCubicSplinesBox(
    Box2d& box, Int32 n, const Point2d* knots, 
    const Vector2d* knotVectors, bool closed = false);

//! 计算点到三次样条曲线的最近距离
/*!
    \ingroup _GEOMAPI_CURVE_
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotVectors 型值点的切矢量数组，元素个数为n
    \param[in] closed 是否为闭合曲线
    \param[in] pt 曲线段外给定的点
    \param[in] dTol 距离公差，正数，超出则不计算最近点
    \param[out] ptNear 曲线上的最近点
    \param[out] nSegment 最近点所在曲线段的序号，[0,n-2]，闭合时为[0,n-1]，负数表示失败
    \return 给定的点到最近点的距离，失败时为极大数
    \see mgCubicSplines
*/
GEOMAPI double mgCubicSplinesHit(
    Int32 n, const Point2d* knots, const Vector2d* knotVectors, bool closed, 
    const Point2d& pt, double dTol, Point2d& ptNear, Int32& nSegment);

//! 计算点到折线或多边形的最近距离
/*!
    \ingroup _GEOMAPI_LNREL_
    \param[in] n 顶点数
    \param[in] points 顶点坐标数组，元素个数为n
    \param[in] closed 是否为多边形
    \param[in] pt 图形外给定的点
    \param[in] dTol 距离公差，正数，超出则不计算最近点
    \param[out] ptNear 图形上的最近点
    \param[out] nSegment 最近点所在线段的序号，[0,n-2]，闭合时为[0,n-1]，负数表示失败
    \return 给定的点到最近点的距离，失败时为极大数
*/
GEOMAPI double mgLinesHit(
    Int32 n, const Point2d* points, bool closed, 
    const Point2d& pt, double dTol, Point2d& ptNear, Int32& nSegment);

//! 计算点到圆角矩形的最近距离
/*!
    \ingroup _GEOMAPI_LNREL_
    \param[in] rect 外接矩形，必须为规范化矩形
    \param[in] rx 水平方向的圆角半径，非负数
    \param[in] ry 垂直方向的圆角半径，为0则取为rx
    \param[in] pt 图形外给定的点
    \param[in] dTol 距离公差，正数，超出则不计算最近点
    \param[out] ptNear 图形上的最近点
    \param[out] nSegment 最近点所在段的序号。负数表示失败；
        0到3为从左上角起顺时针的四个圆角；4到7为顶右底左
    \return 给定的点到最近点的距离，失败时为极大数
*/
GEOMAPI double mgRoundRectHit(
    const Box2d& rect, double rx, double ry, 
    const Point2d& pt, double dTol, Point2d& ptNear, Int32& nSegment);

//! 得到矩形的8个控制手柄坐标
/*!
    \ingroup _GEOMAPI_LNREL_
    \param[in] rect 矩形，必须为规范化矩形
    \param[in] nHandle 控制手柄的序号，0到7，
        0到3为从左上角起顺时针的四个角点；4到7为顶右底左的中点；其余为中心点
    \param[out] pt 控制手柄的坐标
*/
GEOMAPI void mgGetRectHandle(const Box2d& rect, Int32 nHandle, Point2d& pt);

//! 移动矩形的一个控制手柄
/*!
    \ingroup _GEOMAPI_LNREL_
    \param[in,out] rect 矩形，必须为规范化矩形
    \param[in] nHandle 控制手柄的序号，0到7，
        0到3为从左上角起顺时针的四个角点；4到7为顶右底左的中点；其余为中心点
    \param[in] pt 控制手柄的新坐标
*/
GEOMAPI void mgMoveRectHandle(Box2d& rect, Int32 nHandle, const Point2d& pt);

#endif // __GEOMETRY_CURVENEAR_H_