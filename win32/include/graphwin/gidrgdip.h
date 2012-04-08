//! \file gidrgdip.h
//! \brief 定义用GDI+实现的图形系统类 GiGraphGdip
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_DRAWGDIP_H_
#define __GEOMETRY_DRAWGDIP_H_

#include "graphwin.h"

//! 用GDI+实现的图形系统类
/*! 本类的 getGraphType() 值为 2
    \ingroup GRAPH_INTERFACE
*/
class GiGraphGdip : public GiGraphWin
{
public:
    GiGraphGdip(GiTransform* xform);
    GiGraphGdip(const GiGraphGdip& src);
    virtual ~GiGraphGdip();
    GiGraphGdip& operator=(const GiGraphGdip& src);

public:
    //! 返回当前绘图输出对象(Gdiplus::Graphics*)
    void* GetGraphics();

public:
    virtual bool beginPaint(HDC hdc, HDC attribDC = NULL, 
        bool buffered = true, bool overlay = false);
    virtual void endPaint(bool draw = true);
    virtual void clearWnd();
    virtual bool drawCachedBitmap(int x = 0, int y = 0, bool secondBmp = false);
    virtual bool drawCachedBitmap2(const GiGraphics* p, bool secondBmp = false);
    virtual void saveCachedBitmap(bool secondBmp = false);
    virtual bool hasCachedBitmap(bool secondBmp = false) const;
    virtual void clearCachedBitmap();
    virtual bool isBufferedDrawing() const;
    virtual int getGraphType() const { return 2; }
    virtual HDC acquireDC();
    virtual void releaseDC(HDC hdc);
    
    virtual bool setClipBox(const RECT* prc);
    virtual bool setClipWorld(const Box2d& rectWorld);
    
    virtual GiColor getBkColor() const;
    virtual GiColor setBkColor(const GiColor& color);
    virtual GiColor getNearestColor(const GiColor& color) const;
    virtual void setAntiAliasMode(bool antiAlias);
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
    
    virtual bool drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
        const Box2d& rectW, bool fast = false);
    bool drawGdipImage(long hmWidth, long hmHeight, LPVOID pBmp, 
        const Box2d& rectW, bool fast = false);

private:
    class DrawImpl;
    DrawImpl*   m_draw;
};

#endif // __GEOMETRY_DRAWGDIP_H_
