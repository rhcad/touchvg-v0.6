//! \file gicanvas.h
//! \brief 定义抽象画布接口类 GiCanvas
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_DRAWADAPTER_H_
#define __GEOMETRY_DRAWADAPTER_H_

#include "gixform.h"
#include "gicontxt.h"

class GiGraphics;
class GiGraphicsImpl;

//! 抽象画布接口类
/*! 本类是图形显示适配接口类，定义图元显示原语。
    图元显示原语由派生类实现，例如采用GDI/GDI+/SVG/PDF等实现。
    \ingroup GRAPH_INTERFACE
*/
class GiCanvas
{
public:
    GiCanvas() {}
    virtual ~GiCanvas() {}

    //! 供GiGraphics调用
    void _init(GiGraphics* owner, GiGraphicsImpl* impl)
    {
        m_owner = owner;
        m_impl = impl;
    }

    //! 返回图形系统对象，拥有者
    const GiGraphics* owner() const
    {
        return m_owner;
    }

public:
    //! 用当前背景色清除背景
    /*! 用背景色填充全部显示区域。打印或打印预览时调用无效。
        \see getBkColor
    */
    virtual void clearWindow() = 0;

    //! 显示后备缓冲位图
    /*! 后备缓冲位图是由函数 saveCachedBitmap 保存创建的
        \param x 显示偏移X，像素
        \param y 显示偏移Y，像素
        \param secondBmp 使用是否为第二个后备缓冲位图
        \return 是否绘制成功
        \see saveCachedBitmap
    */
    virtual bool drawCachedBitmap(float x = 0, float y = 0, bool secondBmp = false) = 0;

    //! 显示后备缓冲位图
    /*! 将另一个画布的后备缓冲位图显示到本对象的显示设备上，
        后备缓冲位图是由函数 saveCachedBitmap 保存创建的。\n
        另一个画布的作用是后台显示，其本身并不显示图形到设备上，即调用了 endPaint(false)
        \param p 另一个画布，该对象有后备缓冲位图
        \param x 显示偏移X，像素
        \param y 显示偏移Y，像素
        \param secondBmp 使用是否为第二个后备缓冲位图
        \return 是否绘制成功
        \see saveCachedBitmap
    */
    virtual bool drawCachedBitmap2(const GiCanvas* p, 
        float x = 0, float y = 0, bool secondBmp = false) = 0;

    //! 保存显示内容到后备缓冲位图
    /*! 将当前绘图目标(可能是绘图缓冲)的内容保存为一个位图对象，
        该位图的大小为显示窗口大小。\n
        当显示窗口大小改变或显示放缩后，将自动清除后备缓冲位图。
        \param secondBmp 是否存到第二个后备缓冲位图
    */
    virtual void saveCachedBitmap(bool secondBmp = false) = 0;

    //! 返回是否有后备缓冲位图
    /*!
        \param secondBmp 检查的是否为第二个后备缓冲位图
        \return 是否有该缓冲位图
    */
    virtual bool hasCachedBitmap(bool secondBmp = false) const = 0;

    //! 清除后备缓冲位图
    /*! 当显示窗口大小改变或显示放缩后，调用 beginPaint() 时将自动清除后备缓冲位图。
    */
    virtual void clearCachedBitmap() = 0;

    //! 返回是否正在绘图缓冲上绘图
    /*! 在派生类的 beginPaint 函数中指定是否使用绘图缓冲
        \return 是否正在绘图缓冲上绘图
        \see beginPaint
    */
    virtual bool isBufferedDrawing() const = 0;

    //! 返回画布类型
    virtual int getCanvasType() const = 0;

    //! 返回屏幕分辨率DPI, 常量
    virtual float getScreenDpi() const = 0;

    //! 返回背景色
    /*!
        \return 背景色，RGB色，如果是打印或打印预览，则为白色
    */
    virtual GiColor getBkColor() const = 0;

    //! 设置背景色
    /*! 如果正处于绘图状态中，该函数将同时设置绘图设备的背景色
        \param color 背景色，RGB色
        \return 原来的背景色
    */
    virtual GiColor setBkColor(const GiColor& color) = 0;

    //! 返回和模拟设备的调色板相近的颜色
    /*! 打印或打印预览时本函数才有效果，此时模拟设备为打印机，否则为当前显示设备。
        \param color 普通的RGB颜色
        \return 符合模拟设备的RGB颜色
    */
    virtual GiColor getNearestColor(const GiColor& color) const = 0;
    
    //! 返回当前绘图参数
    virtual const GiContext* getCurrentContext() const = 0;

    //! 剪裁框改变后的通知，仅由 GiGraphics 调用
    /*! 调用 GiGraphics::setClipBox() 或 setClipWorld() 后会调用本函数。
        \param[in] clipBox 新剪裁框的逻辑坐标矩形
    */
    virtual void _clipBoxChanged(const RECT2D& clipBox) = 0;

    //! 反走样模式已设置的通知，仅由 GiGraphics 调用
    virtual void _antiAliasModeChanged(bool antiAlias) = 0;


    //! 绘制直线段的原语函数，像素坐标，不剪裁
    virtual bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2) = 0;

    //! 绘制折线的原语函数，像素坐标，不剪裁
    virtual bool rawLines(const GiContext* ctx, const Point2d* pxs, int count) = 0;

    //! 绘制多条贝塞尔曲线的原语函数，像素坐标，不剪裁
    virtual bool rawBeziers(const GiContext* ctx, const Point2d* pxs, int count) = 0;

    //! 绘制多边形的原语函数，像素坐标，不剪裁
    virtual bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count) = 0;

    //! 绘制矩形的原语函数，像素坐标，不剪裁
    virtual bool rawRect(const GiContext* ctx, float x, float y, float w, float h) = 0;

    //! 绘制椭圆的原语函数，像素坐标，不剪裁
    virtual bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h) = 0;

    //! 绘制多样线的原语函数，像素坐标，不剪裁
    virtual bool rawPath(const GiContext* ctx, 
        int count, const Point2d* pxs, const UInt8* types) = 0;


    //! 开始一个路径的原语函数
    virtual bool rawBeginPath() = 0;

    //! 结束并显示一个路径的原语函数
    virtual bool rawEndPath(const GiContext* ctx, bool fill) = 0;

    //! 在当前路径中移动到新的位置的原语函数
    virtual bool rawMoveTo(float x, float y) = 0;

    //! 在当前路径中添加画线指令到新的位置的原语函数
    virtual bool rawLineTo(float x, float y) = 0;

    //! 在当前路径中添加画贝塞尔曲线指令的原语函数
    virtual bool rawBezierTo(const Point2d* pxs, int count) = 0;

    //! 在当前路径中添加闭合指令的原语函数
    virtual bool rawClosePath() = 0;

protected:
    GiGraphics*     m_owner;        //!< 图形系统对象，拥有者
    GiGraphicsImpl* m_impl;         //!< GiGraphics内部实现
};

#endif // __GEOMETRY_DRAWADAPTER_H_
