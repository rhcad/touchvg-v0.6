//! \file gidrgdi.h
//! \brief 定义用GDI实现的图形系统类 GiGraphGdi
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_DRAWGDI_H_
#define __GEOMETRY_DRAWGDI_H_

#include "graphwin.h"

class GiGraphGdiImpl;

//! 用GDI实现的图形系统类
/*! 本类的 getGraphType() 值为 1
    \ingroup GRAPH_INTERFACE
*/
class GiGraphGdi : public GiGraphWin
{
public:
    GiGraphGdi(GiGraphics* gs);
    virtual ~GiGraphGdi();

public:
    virtual bool beginPaint(HDC hdc, HDC attribDC = NULL, 
        bool buffered = true, bool overlay = false);
    virtual void endPaint(bool draw = true);
    virtual void clearWnd();
    virtual bool drawCachedBitmap(float x = 0, float y = 0, bool secondBmp = false);
    virtual bool drawCachedBitmap2(const GiDrawAdapter* p, float x = 0, float y = 0, bool secondBmp = false);
    virtual void saveCachedBitmap(bool secondBmp = false);
    virtual bool hasCachedBitmap(bool secondBmp = false) const;
    virtual void clearCachedBitmap();
    virtual bool isBufferedDrawing() const;
    virtual int getGraphType() const { return 1; }
    virtual HDC acquireDC();
    virtual void releaseDC(HDC hdc);
    
    virtual GiColor getBkColor() const;
    virtual GiColor setBkColor(const GiColor& color);
    virtual GiColor getNearestColor(const GiColor& color) const;
    virtual const GiContext* getCurrentContext() const;
    virtual void _clipBoxChanged(const RECT2D& clipBox);
    virtual void _antiAliasModeChanged(bool) {}

    virtual bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2);
    virtual bool rawPolyline(const GiContext* ctx, const Point2d* pxs, int count);
    virtual bool rawPolyBezier(const GiContext* ctx, const Point2d* pxs, int count);
    virtual bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count);
    virtual bool rawRect(const GiContext* ctx, float x, float y, float w, float h);
    virtual bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h);
    virtual bool rawPolyDraw(const GiContext* ctx, 
        int count, const Point2d* pxs, const UInt8* types);
    
    virtual bool rawBeginPath();
    virtual bool rawEndPath(const GiContext* ctx, bool fill);
    virtual bool rawMoveTo(float x, float y);
    virtual bool rawLineTo(float x, float y);
    virtual bool rawPolyBezierTo(const Point2d* pxs, int count);
    virtual bool rawCloseFigure();
    
    virtual bool drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
        const Box2d& rectW, bool fast = false);

private:
    GiGraphGdiImpl*   m_draw;
};

#endif // __GEOMETRY_DRAWGDI_H_
