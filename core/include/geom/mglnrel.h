//! \file mglnrel.h
//! \brief 定义图形位置关系函数
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_LINEREL_H_
#define __GEOMETRY_LINEREL_H_

#include "mgbox.h"

//! 判断点pt是否在有向直线a->b的左边 (开区间)
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \return 在直线的左边时返回true，否则返回false
*/
GEOMAPI bool mgIsLeft(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在有向直线a->b的左边
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在直线的左边时返回true，否则返回false
*/
GEOMAPI bool mgIsLeft2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 判断点pt是否在有向直线a->b的左边或线上 (闭区间)
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \return 在直线的左边或线上时返回true，否则返回false
*/
GEOMAPI bool mgIsLeftOn(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在有向直线a->b的左边或线上
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在直线的左边或线上时返回true，否则返回false
*/
GEOMAPI bool mgIsLeftOn2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 判断点pt是否在直线a->b的线上
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \return 在直线上时返回true，否则返回false
*/
GEOMAPI bool mgIsColinear(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在直线a->b的线上
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在直线上时返回true，否则返回false
*/
GEOMAPI bool mgIsColinear2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 判断两个线段ab和cd是否相交于线段内部
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \return 是否相交于线段内部，不包括端点
*/
GEOMAPI bool mgIsIntersectProp(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d);

//! 判断点pt是否在线段ab上(闭区间)
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \return 在线段上时返回true，否则返回false
*/
GEOMAPI bool mgIsBetweenLine(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在线段ab上
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在线段上时返回true，否则返回false
*/
GEOMAPI bool mgIsBetweenLine2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 已知点pt在直线ab上, 判断点pt是否在线段ab上(闭区间)
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \param[out] ptNear 在线段的两个端点中，到给定的点最近的端点，为NULL则忽略该参数
    \return 在线段上时返回true，否则返回false
*/
GEOMAPI bool mgIsBetweenLine3(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d* ptNear = NULL);

//! 判断两个线段ab和cd是否相交(交点在线段闭区间内)
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \return 是否相交于线段闭区间内，包括端点
*/
GEOMAPI bool mgIsIntersect(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d);

//! 计算点pt到无穷直线ab的距离
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 无穷直线的起点
    \param[in] b 无穷直线的终点
    \param[in] pt 给定的测试点
    \return 点到无穷直线的距离
*/
GEOMAPI float mgPtToBeeline(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 计算点pt到无穷直线ab的距离
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 无穷直线的起点
    \param[in] b 无穷直线的终点
    \param[in] pt 给定的测试点
    \param[out] ptPerp 直线上的垂足
    \return 独立点到垂足的距离
*/
GEOMAPI float mgPtToBeeline2(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d& ptPerp);

//! 计算点pt到线段ab的最近距离
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \param[out] ptNear 线段上的最近点
    \return 独立点到最近点的距离
*/
GEOMAPI float mgPtToLine(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d& ptNear);

//! 求两条直线(ax+by+c=0)的交点
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a1 第一条直线的标准方程参数A
    \param[in] b1 第一条直线的标准方程参数B
    \param[in] c1 第一条直线的标准方程参数C
    \param[in] a2 第二条直线的标准方程参数A
    \param[in] b2 第二条直线的标准方程参数B
    \param[in] c2 第二条直线的标准方程参数C
    \param[out] ptCross 交点
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
GEOMAPI bool mgCrossLineAbc(
    double a1, double b1, double c1, double a2, double b2, double c2,
    Point2d& ptCross, const Tol& tolVec = Tol::gTol());

//! 求两条无穷直线的交点
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 第一条直线的起点
    \param[in] b 第一条直线的终点
    \param[in] c 第二条直线的起点
    \param[in] d 第二条直线的终点
    \param[out] ptCross 交点
    \param[out] pu 交点在第一条直线上的参数，如果为NULL则忽略该参数
    \param[out] pv 交点在第二条直线上的参数，如果为NULL则忽略该参数
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
GEOMAPI bool mgCross2Beeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, 
    Point2d& ptCross, double* pu = NULL, double* pv = NULL, 
    const Tol& tolVec = Tol::gTol());

//! 求两条线段的交点
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \param[out] ptCross 交点
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
GEOMAPI bool mgCross2Line(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, const Tol& tolVec = Tol::gTol());

//! 求线段和直线的交点
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] c 直线的起点
    \param[in] d 直线的终点
    \param[out] ptCross 交点
    \param[out] pv 交点在直线上的参数，如果为NULL则忽略该参数
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
GEOMAPI bool mgCrossLineBeeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, double* pv = NULL, 
    const Tol& tolVec = Tol::gTol());

//! 用矩形剪裁线段，Sutherland-Cohen算法
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] pt1 线段的起点
    \param[in] pt2 线段的终点
    \param[in] box 剪裁矩形，必须为非空规范化矩形
    \return 剪裁后是否有处于剪裁矩形内的线段部分
*/
GEOMAPI bool mgClipLine(Point2d& pt1, Point2d& pt2, const Box2d& box);

//! mgPtInArea 的返回值枚举定义
//! \see mgPtInArea
enum PtInAreaRet
{
    kPtInArea,          //!< 在多边形内
    kPtOutArea,         //!< 在多边形外
    kPtOnEdge,          //!< 在第order边上
    kPtAtVertex,        //!< 与第order顶点重合
};

//! 判断一点是否在一多边形范围内
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] pt 给定的测试点
    \param[in] count 多边形的顶点数
    \param[in] vertexs 多边形的顶点数组
    \param[out] order 返回 kPtAtVertex 时，输出顶点号[0, count-1]；\n
        返回 kPtOnEdge 时，输出边号[0, count-1]；\n为NULL则忽略该参数
    \param[in] tol 容差
    \return 为枚举定义 PtInAreaRet 的各种值
    \see PtInAreaRet
*/
GEOMAPI int mgPtInArea(
    const Point2d& pt, Int32 count, const Point2d* vertexs, 
    Int32& order, const Tol& tol = Tol::gTol());

//! 判断多边形是否为凸多边形
/*!
    \ingroup GEOMAPI_LNREL
    \param[in] count 顶点个数
    \param[in] vertexs 顶点数组
    \param[out] pACW 多边形是否为逆时针方向，为NULL则忽略该参数
    \return 是否为凸多边形
*/
GEOMAPI bool mgIsConvex(Int32 count, const Point2d* vertexs, bool* pACW = NULL);

#endif // __GEOMETRY_LINEREL_H_
