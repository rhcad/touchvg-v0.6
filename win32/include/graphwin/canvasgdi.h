//! \file canvasgdi.h
//! \brief 定义用GDI实现的图形显示接口类 GiCanvasGdi
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_DRAWGDI_H_
#define __GEOMETRY_DRAWGDI_H_

#include "canvaswin.h"

class GiCanvasGdiImpl;

//! 用GDI实现的图形显示接口类
/*! 本类的 getCanvasType() 值为 1
    \ingroup GRAPH_WIN
*/
class GiCanvasGdi : public GiCanvasWin
{
public:
    GiCanvasGdi(GiGraphics* gs);
    virtual ~GiCanvasGdi();

    //! 工厂函数
    static GiCanvasWin* Create(GiGraphics* gs) { return new GiCanvasGdi(gs); }

public:
    virtual bool beginPaint(HDC hdc, HDC attribDC = NULL, 
        bool buffered = true, bool overlay = false);
    virtual void endPaint(bool draw = true);
    virtual void clearWindow();
    virtual bool drawCachedBitmap(float x = 0, float y = 0, bool secondBmp = false);
    virtual bool drawCachedBitmap2(const GiCanvas* p, 
        float x = 0, float y = 0, bool secondBmp = false);
    virtual void saveCachedBitmap(bool secondBmp = false);
    virtual bool hasCachedBitmap(bool secondBmp = false) const;
    virtual void clearCachedBitmap(bool clearAll = false);
    virtual bool isBufferedDrawing() const;
    virtual int getCanvasType() const { return 1; }
    virtual HDC acquireDC();
    virtual void releaseDC(HDC hdc);
    
    virtual GiColor getBkColor() const;
    virtual GiColor setBkColor(const GiColor& color);
    virtual GiColor getNearestColor(const GiColor& color) const;
    virtual const GiContext* getCurrentContext() const;
    virtual void _clipBoxChanged(const RECT_2D& clipBox);
    virtual void _antiAliasModeChanged(bool antiAlias);

    virtual bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2);
    virtual bool rawLines(const GiContext* ctx, const Point2d* pxs, int count);
    virtual bool rawBeziers(const GiContext* ctx, const Point2d* pxs, int count);
    virtual bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count);
    virtual bool rawRect(const GiContext* ctx, float x, float y, float w, float h);
    virtual bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h);
    
    virtual bool rawBeginPath();
    virtual bool rawEndPath(const GiContext* ctx, bool fill);
    virtual bool rawMoveTo(float x, float y);
    virtual bool rawLineTo(float x, float y);
    virtual bool rawBezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    virtual bool rawClosePath();
    
    virtual bool drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
        const Box2d& rectW, bool fast = false);

private:
    GiCanvasGdiImpl*   m_draw;
};

#endif // __GEOMETRY_DRAWGDI_H_
