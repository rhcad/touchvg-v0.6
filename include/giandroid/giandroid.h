//! \file giandroid.h
//! \brief Define the graphics class using Android canvas: GiAndroid
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_GRAPH_ANDROID_H_
#define __GEOMETRY_GRAPH_ANDROID_H_

#include <gigraph.h>

_GEOM_BEGIN

class GiAndroidImpl;

//! The graphics class using Android canvas.
/*! getGraphType() of this class returns 11.
    \ingroup _GRAPH_INTERFACE_
*/
class GiAndroid : public GiGraphics
{
public:
    GiAndroid(GiTransform& xform);
    GiAndroid(const GiAndroid& src);
    virtual ~GiAndroid();
    GiAndroid& operator=(const GiAndroid& src);

public:
    bool beginPaint(void* convas, bool buffered = true, bool overlay = false);
    void endPaint(bool draw = true);

    virtual void clearWnd();
    virtual bool drawCachedBitmap(int x = 0, int y = 0, bool secondBmp = false);
    virtual bool drawCachedBitmap2(const GiGraphics* p, bool secondBmp = false);
    virtual void saveCachedBitmap(bool secondBmp = false);
    virtual bool hasCachedBitmap(bool secondBmp = false) const;
    virtual void clearCachedBitmap();
    virtual bool isBufferedDrawing() const;
    virtual int getGraphType() const { return 11; }
    virtual int getScreenDpi() const { return 96; }
    
    virtual bool setClipBox(const RECT* prc);
    virtual bool setClipWorld(const Box2d& rectWorld);
    
    virtual GiColor getBkColor() const;
    virtual GiColor setBkColor(const GiColor& color);
    virtual GiColor getNearestColor(const GiColor& color) const;
    virtual const GiContext* getCurrentContext() const;

    virtual bool rawLine(const GiContext* ctx, 
        int x1, int y1, int x2, int y2);
    virtual bool rawPolyline(const GiContext* ctx, 
        const POINT* lppt, int count);
    virtual bool rawPolyBezier(const GiContext* ctx, 
        const POINT* lppt, int count);
    virtual bool rawPolygon(const GiContext* ctx, 
        const POINT* lppt, int count);
    virtual bool rawRect(const GiContext* ctx, 
        int x, int y, int w, int h);
    virtual bool rawEllipse(const GiContext* ctx, 
        int x, int y, int w, int h);
    virtual bool rawPolyDraw(const GiContext* ctx, 
        int count, const POINT* lppt, const UInt8* types);
    
    virtual bool rawBeginPath();
    virtual bool rawEndPath(const GiContext* ctx, bool fill);
    virtual bool rawMoveTo(int x, int y);
    virtual bool rawLineTo(int x, int y);
    virtual bool rawPolyBezierTo(const POINT* lppt, int count);
    virtual bool rawCloseFigure();

private:
    GiAndroid();
    GiAndroidImpl*   m_draw;
};

_GEOM_END
#endif // __GEOMETRY_GRAPH_ANDROID_H_