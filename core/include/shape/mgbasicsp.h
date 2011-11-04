//! \file mgbasicsp.h
//! \brief 定义基本图形类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_BASICSHAPE_H_
#define __GEOMETRY_BASICSHAPE_H_

#include "mgshape.h"

//! 线段图形类
/*! \ingroup _GEOM_SHAPE_
*/
class MgLine : public MgBaseShape
{
    MG_DECLARE_CREATE(MgLine, MgBaseShape, 10)
public:
    //! 返回起点
    const Point2d& startPoint() const { return _points[0]; }

    //! 返回终点
    const Point2d& endPoint() const { return _points[1]; }
    
    //! 返回终点
    Point2d center() const { return (_points[0] + _points[1]) / 2; }
    
protected:
    UInt32 _getHandleCount() const;
    Point2d _getHandlePoint(UInt32 index) const;
    bool _setHandlePoint(UInt32 index, const Point2d& pt, double tol);

private:
    Point2d     _points[2];
};

//! 矩形图形基类
/*! \ingroup _GEOM_SHAPE_
*/
class MgBaseRect : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgBaseRect, MgBaseShape)
public:
    //! 返回本对象的类型
    static UInt32 Type() { return 4; }

    //! 返回中心点
    Point2d getCenter() const;

    //! 返回矩形框，是本对象未旋转时的形状
    Box2d getRect() const;

    //! 返回宽度
    double getWidth() const;

    //! 返回高度
    double getHeight() const;

    //! 返回倾斜角度
    double getAngle() const;

    //! 返回是否为空矩形
    bool isEmpty(double minDist) const;

    //! 返回是否为水平矩形
    bool isOrtho() const;

    //! 设置矩形
    void setRect(const Box2d& rect, double angle = 0.0);

    //! 设置四个角点
    void setRect(const Point2d points[4]);

    //! 设置中心点
    void setCenter(const Point2d& pt);

protected:
    MgBaseRect();
    UInt32 _getPointCount() const;
    Point2d _getPoint(UInt32 index) const;
    void _setPoint(UInt32 index, const Point2d& pt);
    bool _isClosed() const;
    void _copy(const MgBaseRect& src);
    bool _equals(const MgBaseRect& src) const;
    bool _isKindOf(UInt32 type) const;
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    double _hitTest(const Point2d& pt, double tol, Point2d& ptNear, Int32& segment) const;
    UInt32 _getHandleCount() const;
    Point2d _getHandlePoint(UInt32 index) const;
    bool _setHandlePoint(UInt32 index, const Point2d& pt, double tol);
    bool _hitTestBox(const Box2d& rect) const;

protected:
    Point2d     _points[4]; // 从左上角起顺时针的四个角点
};

//! 矩形图形类
/*! \ingroup _GEOM_SHAPE_
*/
class MgRect : public MgBaseRect
{
    MG_INHERIT_CREATE(MgRect, MgBaseRect, 11)
};

//! 椭圆图形类
/*! \ingroup _GEOM_SHAPE_
*/
class MgEllipse : public MgBaseRect
{
    MG_INHERIT_CREATE(MgEllipse, MgBaseRect, 12)
public:
    //! 返回X半轴长度
    double getRadiusX() const;

    //! 返回Y半轴长度
    double getRadiusY() const;

    //! 设置半轴长度
    void setRadius(double rx, double ry = 0.0);

protected:
    void _update();
    double _hitTest(const Point2d& pt, double tol, Point2d& ptNear, Int32& segment) const;
    bool _hitTestBox(const Box2d& rect) const;

protected:
    Point2d     _bzpts[13];
};

//! 圆角矩形类
/*! \ingroup _GEOM_SHAPE_
*/
class MgRoundRect : public MgBaseRect
{
    MG_INHERIT_CREATE(MgRoundRect, MgBaseRect, 13)
public:
    //! 返回X圆角半径
    double getRadiusX() const { return _rx; }

    //! 返回Y圆角半径
    double getRadiusY() const { return _ry; }

    //! 设置圆角半径
    void setRadius(double rx, double ry = 0.0);

protected:
    void _copy(const MgRoundRect& src);
    bool _equals(const MgRoundRect& src) const;
    void _clear();
    double _hitTest(const Point2d& pt, double tol, Point2d& ptNear, Int32& segment) const;

protected:
    double      _rx;
    double      _ry;
};

//! 折线基类
/*! \ingroup _GEOM_SHAPE_
*/
class MgBaseLines : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgBaseLines, MgBaseShape)
public:
    //! 返回本对象的类型
    static UInt32 Type() { return 5; }

    //! 设置是否闭合
    void setClosed(bool closed);

    //! 返回终点
    Point2d endPoint() const;

    //! 改变顶点数
    void resize(UInt32 count);

    //! 添加一个顶点
    void addPoint(const Point2d& pt);
    
    //! 在指定段插入一个顶点
    void insertPoint(Int32 segment, const Point2d& pt);

    //! 删除一个顶点
    void removePoint(UInt32 index);

protected:
    MgBaseLines();
    virtual ~MgBaseLines();
    UInt32 _getPointCount() const;
    Point2d _getPoint(UInt32 index) const;
    void _setPoint(UInt32 index, const Point2d& pt);
    bool _isClosed() const;
    void _copy(const MgBaseLines& src);
    bool _equals(const MgBaseLines& src) const;
    bool _isKindOf(UInt32 type) const;
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    bool _setHandlePoint(UInt32 index, const Point2d& pt, double tol);
    double _hitTest(const Point2d& pt, double tol, Point2d& ptNear, Int32& segment) const;

protected:
    Point2d*    _points;
    UInt32      _maxCount;
    UInt32      _count;
    bool        _closed;
};

//! 折线图形类
/*! \ingroup _GEOM_SHAPE_
*/
class MgLines : public MgBaseLines
{
    MG_INHERIT_CREATE(MgLines, MgBaseLines, 15)
protected:
    bool _hitTestBox(const Box2d& rect) const;
};

//! 三次参数样条曲线类
/*! \ingroup _GEOM_SHAPE_
*/
class MgSplines : public MgBaseLines
{
    MG_INHERIT_CREATE(MgSplines, MgBaseLines, 16)
public:
    //! 去掉多余点，同时仍然光滑
    void smooth(double tol);
    
protected:
    void _update();
    double _hitTest(const Point2d& pt, double tol, Point2d& ptNear, Int32& segment) const;
    bool _hitTestBox(const Box2d& rect) const;

protected:
    Vector2d*   _knotVectors;
    UInt32      _bzcount;
};

#endif // __GEOMETRY_BASICSHAPE_H_
