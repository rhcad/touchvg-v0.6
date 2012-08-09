// GiCanvasBase.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiCanvasBase.h"

GiCanvasBase::GiCanvasBase() : _gs(&_xf), _bkcolor(GiColor::White()), _ctxstatus(0)
{
    _gs._setCanvas(this);
    _xf.setResolution(screenDpi());
}

GiCanvasBase::~GiCanvasBase()
{
}

bool GiCanvasBase::rawLine(float, float, float, float) { return false; }
bool GiCanvasBase::rawLines(const mgvector<float>&) { return false; }
bool GiCanvasBase::rawBeziers(const mgvector<float>&) { return false; }
bool GiCanvasBase::rawPolygon(const mgvector<float>&, bool, bool) { return false; }
bool GiCanvasBase::rawRect(float, float, float, float, bool, bool) { return false; }
bool GiCanvasBase::rawEllipse(float, float, float, float, bool, bool) { return false; }
bool GiCanvasBase::rawEndPath(bool, bool) { return false; }
bool GiCanvasBase::rawBezierTo(const mgvector<float>&) { return false; }
bool GiCanvasBase::rawPath(const mgvector<float>&, const mgvector<char>&, bool, bool) { return false; }

bool GiCanvasBase::rawMoveTo(float, float) { return false; }
bool GiCanvasBase::rawLineTo(float, float) { return false; }
void GiCanvasBase::clearCachedBitmap(bool) {}
bool GiCanvasBase::drawCachedBitmap(float, float, bool) { return false; }
void GiCanvasBase::saveCachedBitmap(bool) {}
bool GiCanvasBase::hasCachedBitmap(bool) const { return false; }
void GiCanvasBase::clipBoxChanged(float, float, float, float) {}
void GiCanvasBase::antiAliasModeChanged(bool) {}
bool GiCanvasBase::drawCachedBitmap2(const GiCanvas*, float, float, bool) { return false; }
void GiCanvasBase::penChanged(const GiContext&) {}
void GiCanvasBase::brushChanged(const GiContext&) {}

bool GiCanvasBase::rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2)
{
	return checkStroke(ctx) && rawLine(x1, y1, x2, y2);
}

bool GiCanvasBase::rawLines(const GiContext* ctx, const Point2d* pxs, int count)
{
	bool ret = checkStroke(ctx) && pxs && count > 0;
	if (ret) {
		mgvector<float> arr(&pxs[0].x, 2 * count);
		ret = rawLines(arr);
	}

	return ret;
}

bool GiCanvasBase::rawBeziers(const GiContext* ctx, const Point2d* pxs, int count)
{
	bool ret = checkStroke(ctx);
	if (ret) {
		mgvector<float> arr(&pxs[0].x, 2 * count);
		ret = rawBeziers(arr);
	}

	return ret;
}

bool GiCanvasBase::rawPolygon(const GiContext* ctx, const Point2d* pxs, int count)
{
	bool stroke = checkStroke(ctx);
	bool fill = checkFill(ctx);
	bool ret = stroke || fill;

	if (ret) {
		mgvector<float> arr(&pxs[0].x, 2 * count);
		ret = rawPolygon(arr, stroke, fill);
	}

	return ret;
}

bool GiCanvasBase::rawRect(const GiContext* ctx, float x, float y, float w, float h)
{
	bool stroke = checkStroke(ctx);
	bool fill = checkFill(ctx);
	return (stroke || fill) && rawRect(x, y, w, h, stroke, fill);
}

bool GiCanvasBase::rawEllipse(const GiContext* ctx, float x, float y, float w, float h)
{
	bool stroke = checkStroke(ctx);
	bool fill = checkFill(ctx);
	return (stroke || fill) && rawEllipse(x, y, w, h, stroke, fill);
}

bool GiCanvasBase::rawEndPath(const GiContext* ctx, bool fill)
{
	bool stroke = checkStroke(ctx);
	fill = fill && checkFill(ctx);
	return (stroke || fill) && rawEndPath(stroke, fill);
}

bool GiCanvasBase::rawBezierTo(const Point2d* pxs, int count)
{
	mgvector<float> arr(&pxs[0].x, 2 * count);
	return rawBezierTo(arr);
}

bool GiCanvasBase::rawPath(const GiContext* ctx, int count, const Point2d* pxs, const UInt8* types)
{
	bool stroke = checkStroke(ctx);
	bool fill = checkFill(ctx);
	bool ret = stroke || fill;

	if (ret) {
		mgvector<float> arr(&pxs[0].x, 2 * count);
		mgvector<char> arr2((const char*)types, count);
		ret = rawPath(arr, arr2, stroke, fill);
	}

	return ret;
}

void GiCanvasBase::_clipBoxChanged(const RECT_2D& clipBox)
{
	clipBoxChanged(clipBox.left, clipBox.top, clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
}

GiColor GiCanvasBase::setBkColor(const GiColor& color)
{
	GiColor old(_bkcolor); _bkcolor = color; return old;
}

bool GiCanvasBase::checkStroke(const GiContext* ctx)
{
	bool changed = !(_ctxstatus & 1);

	if (ctx && !ctx->isNullLine())
	{
		if (_gictx.getLineColor() != ctx->getLineColor()) {
			_gictx.setLineColor(ctx->getLineColor());
			changed = true;
		}
		if (_gictx.getLineWidth() != ctx->getLineWidth()) {
			_gictx.setLineWidth(ctx->getLineWidth());
			changed = true;
		}
		if (_gictx.getLineStyle() != ctx->getLineStyle()) {
			_gictx.setLineStyle(ctx->getLineStyle());
			changed = true;
		}
	}

	if (!ctx) ctx = &_gictx;
	if (!ctx->isNullLine() && changed)
	{
		_ctxstatus |= 1;
		penChanged(*ctx);
	}

	return !ctx->isNullLine();
}

bool GiCanvasBase::checkFill(const GiContext* ctx)
{
	bool changed = !(_ctxstatus & 2);

	if (ctx && ctx->hasFillColor())
	{
		if (_gictx.getFillColor() != ctx->getFillColor()) {
			_gictx.setFillColor(ctx->getFillColor());
			changed = true;
		}
	}
	if (!ctx) ctx = &_gictx;
	if (ctx->hasFillColor() && changed)
	{
		_ctxstatus |= 2;
		penChanged(*ctx);
	}

	return ctx->hasFillColor();
}

bool GiCanvasBase::beginPaint()
{
	_ctxstatus = 0;
	return true;
}

void GiCanvasBase::endPaint()
{
}
