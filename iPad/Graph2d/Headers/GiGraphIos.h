//! \file GiGraphIos.h
//! \brief Define the graphics class for iOS: GiGraphIos
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_GRAPHIOS_H_
#define __GEOMETRY_GRAPHIOS_H_

#include <Graph2d/gigraph.h>
#include <CoreGraphics/CGContext.h>

class GiGraphIosImpl;

GiColor giFromCGColor(CGColorRef color);

//! The graphics class for iOS.
/*! getGraphType() of this class returns 10.
    \ingroup GRAPH_IOS
    \see giFromCGColor
*/
class GiGraphIos : public GiDrawAdapter
{
public:
    GiGraphIos(GiGraphics* gs);
    virtual ~GiGraphIos();

public:
    const GiTransform& xf() const;
    bool beginPaint(CGContextRef context, bool buffered = true, bool fast = false);
    void endPaint(bool draw = true);
    static void setScreenDpi(float dpi);

    virtual void clearWnd();
    virtual bool drawCachedBitmap(float x = 0, float y = 0, bool secondBmp = false);
    virtual bool drawCachedBitmap2(const GiDrawAdapter* p, float x = 0, float y = 0, bool secondBmp = false);
    virtual void saveCachedBitmap(bool secondBmp = false);
    virtual bool hasCachedBitmap(bool secondBmp = false) const;
    virtual void clearCachedBitmap();
    virtual bool isBufferedDrawing() const;
    virtual int getGraphType() const { return 10; }
    virtual float getScreenDpi() const;
    
    virtual GiColor getBkColor() const;
    virtual GiColor setBkColor(const GiColor& color);
    virtual GiColor getNearestColor(const GiColor& color) const;
    virtual const GiContext* getCurrentContext() const;
    virtual void _clipBoxChanged(const RECT2D& clipBox);
    virtual void _antiAliasModeChanged(bool antiAlias);

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

private:
    GiGraphIos();
    GiGraphIosImpl*   m_draw;
};

#endif // __GEOMETRY_GRAPHIOS_H_
