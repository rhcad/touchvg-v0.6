//! \file GiCanvasBase.h
//! \brief The canvas adapter class: GiCanvasBase.
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SWIG_CANVAS_H_
#define __TOUCHVG_SWIG_CANVAS_H_

#include <gigraph.h>
#include "mgvector.h"

class GiCanvasBase_;

//! The canvas adapter class.
/** \ingroup GRAPH_SKIA
 *  \see GiCanvas
 */
class GiCanvasBase
{
public:
    GiCanvasBase();
    virtual ~GiCanvasBase();

    GiTransform& xf();
	GiGraphics& gs();

    static void setScreenDpi(float dpi) { screenDpi() = dpi; }
    virtual float getScreenDpi() const { return screenDpi(); }
    virtual GiColor getBkColor() const;
    virtual GiColor setBkColor(const GiColor& color);

    virtual void penChanged(const GiContext& ctx);
	virtual void brushChanged(const GiContext& ctx);
    virtual bool rawLine(float x1, float y1, float x2, float y2);
	virtual bool rawLines(const mgvector<float>& pxs);
	virtual bool rawBeziers(const mgvector<float>& pxs);
	virtual bool rawPolygon(const mgvector<float>& pxs, bool stroke, bool fill);
	virtual bool rawRect(float x, float y, float w, float h, bool stroke, bool fill);
	virtual bool rawEllipse(float x, float y, float w, float h, bool stroke, bool fill);
	virtual bool rawEndPath(bool stroke, bool fill);
	virtual bool rawBezierTo(const mgvector<float>& pxs);
	virtual bool rawPath(const mgvector<float>& pxs, const mgvector<char>& types, bool stroke, bool fill);

    virtual bool rawBeginPath() { return false; }
    virtual bool rawMoveTo(float x, float y);
    virtual bool rawLineTo(float x, float y);
    virtual bool rawClosePath() { return false; }

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
    virtual const GiContext* getCurrentContext() const;
    virtual int getCanvasType() const { return 11; }
    virtual GiColor getNearestColor(const GiColor& color) const { return color; }
    virtual bool drawCachedBitmap2(const GiCanvas* p, float x = 0, float y = 0, bool secondBmp = false);
    virtual void _clipBoxChanged(const RECT_2D& clipBox);

    static float& screenDpi() { static float dpi = 120; return dpi; }

    virtual bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2);
	virtual bool rawLines(const GiContext* ctx, const Point2d* pxs, int count);
	virtual bool rawBeziers(const GiContext* ctx, const Point2d* pxs, int count);
	virtual bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count);
	virtual bool rawRect(const GiContext* ctx, float x, float y, float w, float h);
	virtual bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h);
	virtual bool rawEndPath(const GiContext* ctx, bool fill);
	virtual bool rawBezierTo(const Point2d* pxs, int count);
	virtual bool rawPath(const GiContext* ctx, int count, const Point2d* pxs, const UInt8* types);

private:
	friend class GiCanvasBase_;
	GiCanvasBase_*	impl;
};

#endif // __TOUCHVG_SWIG_CANVAS_H_
