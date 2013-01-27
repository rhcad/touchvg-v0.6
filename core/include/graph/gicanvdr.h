//! \file gicanvdr.h
//! \brief 定义画布图元显示原语接口 GiCanvasDrawing
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_CANVAS_DRAWING_H_
#define __GEOMETRY_CANVAS_DRAWING_H_

#include "gixform.h"
#include "gicontxt.h"

class GiGraphics;
class GiGraphicsImpl;

//! 画布图元显示原语接口
/*! \ingroup GRAPH_INTERFACE
    \interface GiCanvasDrawing
*/
struct GiCanvasDrawing
{
    //! 清除后备缓冲位图，以便重新绘图
    virtual void clearCachedBitmap(bool clearAll = false) = 0;

    //! 返回屏幕分辨率DPI, 常量
    virtual float getScreenDpi() const = 0;

    //! 返回背景色(打印或打印预览时为白色)
    virtual GiColor getBkColor() const = 0;
    
    //! 设置新的背景色，返回原来的背景色
    virtual GiColor setBkColor(const GiColor& color) = 0;

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

    //! 开始一个路径的原语函数
    virtual bool rawBeginPath() = 0;

    //! 结束并显示一个路径的原语函数
    virtual bool rawEndPath(const GiContext* ctx, bool fill) = 0;

    //! 在当前路径中移动到新的位置的原语函数
    virtual bool rawMoveTo(float x, float y) = 0;

    //! 在当前路径中添加画线指令到新的位置的原语函数
    virtual bool rawLineTo(float x, float y) = 0;

    //! 在当前路径中添加画贝塞尔曲线指令的原语函数
    virtual bool rawBezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) = 0;

    //! 在当前路径中添加闭合指令的原语函数
    virtual bool rawClosePath() = 0;
    
    //! 在指定点以指定的字体像素大小居中显示一行文字
    virtual void rawTextCenter(const char* text, float x, float y, float h) {
        if (text && x && y && h) text = NULL;
    }
    
    // 在指定矩形区域显示图像
    virtual bool drawImage(const char* name, float xc, float yc, float w, float h, float angle) {
        return !name && xc && yc && w && h && angle;
    }
};

#endif // __GEOMETRY_CANVAS_DRAWING_H_
