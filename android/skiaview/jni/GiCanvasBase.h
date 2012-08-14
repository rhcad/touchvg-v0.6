//! \file GiCanvasBase.h
//! \brief The canvas adapter class: GiCanvasBase.
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SWIG_CANVAS_H_
#define __TOUCHVG_SWIG_CANVAS_H_

#include <gicanvas.h>
#include <gigraph.h>
#include <mgvector.h>

//! The canvas adapter class.
/** \ingroup GRAPH_SKIA
 */
class GiCanvasBase : public GiCanvas
{
public:
    GiCanvasBase();
    virtual ~GiCanvasBase();

    GiTransform& xf() { return _xf; }
	GiGraphics& gs() { return _gs; }

    static void setScreenDpi(float dpi) { screenDpi() = dpi; }
    virtual float getScreenDpi() const { return screenDpi(); }
    virtual GiColor getBkColor() const { return _bkcolor; }
    virtual GiColor setBkColor(const GiColor& color);

	bool beginPaint();
	void endPaint();
	virtual void setNeedRedraw();
    virtual void penChanged(const GiContext& ctx, float penWidth);
	virtual void brushChanged(const GiContext& ctx);

    virtual bool drawLine(float x1, float y1, float x2, float y2);
	virtual bool drawLines(const mgvector<float>& pxs);
	virtual bool drawBeziers(const mgvector<float>& pxs);
	virtual bool drawPolygon(const mgvector<float>& pxs, bool stroke, bool fill);
	virtual bool drawRect(float x, float y, float w, float h, bool stroke, bool fill);
	virtual bool drawEllipse(float x, float y, float w, float h, bool stroke, bool fill);
	virtual bool drawPath(const mgvector<float>& pxs, const mgvector<char>& types, bool stroke, bool fill);

    virtual bool beginPath() { return false; }
    virtual bool pathMoveTo(float x, float y);
    virtual bool pathLineTo(float x, float y);
    virtual bool pathBezierTo(const mgvector<float>& pxs);
    virtual bool closePath() { return false; }
    virtual bool endPath(bool stroke, bool fill);

    virtual void clearWindow() {}
    virtual void clearCachedBitmap(bool clearAll = false);
    virtual bool drawCachedBitmap(float x = 0, float y = 0, bool secondBmp = false);
    virtual void saveCachedBitmap(bool secondBmp = false);
    virtual bool hasCachedBitmap(bool secondBmp = false) const;
    virtual bool isBufferedDrawing() const { return false; }

    virtual void clipBoxChanged(float x, float y, float w, float h);
    virtual void antiAliasModeChanged(bool antiAlias);

private:
    bool checkStroke(const GiContext* ctx);
    bool checkFill(const GiContext* ctx);
    virtual const GiContext* getCurrentContext() const { return &_gictx; }
    virtual int getCanvasType() const { return 11; }
    virtual GiColor getNearestColor(const GiColor& color) const { return color; }
    virtual bool drawCachedBitmap2(const GiCanvas* p, float x = 0, float y = 0, bool secondBmp = false);
    virtual void _clipBoxChanged(const RECT_2D& clipBox);
    virtual void _antiAliasModeChanged(bool antiAlias) { antiAliasModeChanged(antiAlias); }

    static float& screenDpi() { static float dpi = 240; return dpi; }

    virtual bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2);
	virtual bool rawLines(const GiContext* ctx, const Point2d* pxs, int count);
	virtual bool rawBeziers(const GiContext* ctx, const Point2d* pxs, int count);
	virtual bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count);
	virtual bool rawRect(const GiContext* ctx, float x, float y, float w, float h);
	virtual bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h);
	virtual bool rawEndPath(const GiContext* ctx, bool fill);
	virtual bool rawBezierTo(const Point2d* pxs, int count);
	virtual bool rawPath(const GiContext* ctx, int count, const Point2d* pxs, const UInt8* types);
	virtual bool rawBeginPath() { return beginPath(); }
	virtual bool rawMoveTo(float x, float y) { return pathMoveTo(x, y); }
	virtual bool rawLineTo(float x, float y) { return pathLineTo(x, y); }
	virtual bool rawClosePath() { return closePath(); }

private:
	GiTransform 	_xf;
	GiGraphics  	_gs;
	GiColor     	_bkcolor;
	GiContext   	_gictx;
	int         	_ctxstatus;
};

#endif // __TOUCHVG_SWIG_CANVAS_H_
