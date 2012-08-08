// GiCanvasBase.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiCanvasBase.h"
#include <gicanvas.h>

class GiCanvasBase_ : public GiCanvas
{
public:
	GiCanvasBase*	owner;
	GiTransform 	xf;
	GiGraphics  	gs;
	GiColor     	bkcolor;
	GiContext   	gictx;
	int         	ctxstatus;

	GiCanvasBase_(GiCanvasBase* p) : owner(p), gs(&xf), bkcolor(GiColor::White()), ctxstatus(0) {}
	virtual ~GiCanvasBase_() {}

private:
	virtual float getScreenDpi() const { return owner->getScreenDpi(); }
	virtual GiColor getBkColor() const { return owner->getBkColor(); }
	virtual GiColor setBkColor(const GiColor& color) { return owner->setBkColor(color); }

	virtual bool rawBeginPath() { return owner->rawBeginPath(); }
	virtual bool rawMoveTo(float x, float y) { return owner->rawMoveTo(x, y); }
	virtual bool rawLineTo(float x, float y) { return owner->rawLineTo(x, y); }
	virtual bool rawClosePath() { return owner->rawClosePath(); }

	virtual void clearWindow() { owner->clearWindow(); }
	virtual void clearCachedBitmap(bool clearAll = false) { owner->clearCachedBitmap(clearAll); }
	virtual bool drawCachedBitmap(float x = 0, float y = 0, bool secondBmp = false) {
		return owner->drawCachedBitmap(x, y, secondBmp); }
	virtual bool drawCachedBitmap2(const GiCanvas* p, float x = 0, float y = 0, bool secondBmp = false) {
		return owner->getScreenDpi(); }
	virtual void saveCachedBitmap(bool secondBmp = false) { owner->saveCachedBitmap(secondBmp); }
	virtual bool hasCachedBitmap(bool secondBmp = false) const { return owner->hasCachedBitmap(secondBmp); }
	virtual bool isBufferedDrawing() const { return owner->isBufferedDrawing(); }

	virtual const GiContext* getCurrentContext() const { return owner->getCurrentContext(); }
	virtual int getCanvasType() const { return owner->getCanvasType(); }
	virtual GiColor getNearestColor(const GiColor& color) const { return owner->getNearestColor(color); }
	virtual void _clipBoxChanged(const RECT_2D& clipBox) { return owner->_clipBoxChanged(clipBox); }
	virtual void _antiAliasModeChanged(bool antiAlias) { return owner->antiAliasModeChanged(antiAlias); }

	virtual bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2) {
		return owner->rawLine(ctx, x1, y1, x2, y2); }
	virtual bool rawLines(const GiContext* ctx, const Point2d* pxs, int count) {
		return owner->rawLines(ctx, pxs, count); }
	virtual bool rawBeziers(const GiContext* ctx, const Point2d* pxs, int count) {
		return owner->rawBeziers(ctx, pxs, count); }
	virtual bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count) {
		return owner->rawPolygon(ctx, pxs, count); }
	virtual bool rawRect(const GiContext* ctx, float x, float y, float w, float h) {
		return owner->rawRect(ctx, x, y, w, h); }
	virtual bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h) {
		return owner->rawEllipse(ctx, x, y, w, h); }
	virtual bool rawEndPath(const GiContext* ctx, bool fill) { return owner->rawEndPath(ctx, fill); }
	virtual bool rawBezierTo(const Point2d* pxs, int count) { return owner->rawBezierTo(pxs, count); }
	virtual bool rawPath(const GiContext* ctx, int count, const Point2d* pxs, const UInt8* types) {
		return owner->rawPath(ctx, count, pxs, types); }
};

GiCanvasBase::GiCanvasBase()
{
	impl = new GiCanvasBase_(this);
    impl->gs._setCanvas(impl);
    impl->xf.setResolution(screenDpi());
}

GiCanvasBase::~GiCanvasBase()
{
	delete impl;
}

GiTransform& GiCanvasBase::xf() { return impl->xf; }
GiGraphics& GiCanvasBase::gs() { return impl->gs; }
const GiContext* GiCanvasBase::getCurrentContext() const { return &impl->gictx; }
GiColor GiCanvasBase::getBkColor() const { return impl->bkcolor; }
GiColor GiCanvasBase::setBkColor(const GiColor& color) {
	GiColor old(impl->bkcolor); impl->bkcolor = color; return old;
}

bool GiCanvasBase::rawLine(float, float, float, float) { return false; }
bool GiCanvasBase::rawLines(const std::vector<float>&) { return false; }
bool GiCanvasBase::rawBeziers(const std::vector<float>&) { return false; }
bool GiCanvasBase::rawPolygon(const std::vector<float>&, bool, bool) { return false; }
bool GiCanvasBase::rawRect(float, float, float, float, bool, bool) { return false; }
bool GiCanvasBase::rawEllipse(float, float, float, float, bool, bool) { return false; }
bool GiCanvasBase::rawEndPath(bool, bool) { return false; }
bool GiCanvasBase::rawBezierTo(const std::vector<float>&) { return false; }
bool GiCanvasBase::rawPath(const std::vector<float>&, const char*, bool, bool) { return false; }

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
	bool ret = checkStroke(ctx);
	if (ret) {
		std::vector<float> arr(2 * count);

		for (int i = 0; i < count; i++) {
			arr[2*i] = pxs[i].x;
			arr[2*i+1] = pxs[i].y;
		}
		ret = rawLines(arr);
	}

	return ret;
}

bool GiCanvasBase::rawBeziers(const GiContext* ctx, const Point2d* pxs, int count)
{
	bool ret = checkStroke(ctx);
	if (ret) {
		std::vector<float> arr(2 * count);

		for (int i = 0; i < count; i++) {
			arr[2*i] = pxs[i].x;
			arr[2*i+1] = pxs[i].y;
		}
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
		std::vector<float> arr(2 * count);

		for (int i = 0; i < count; i++) {
			arr[2*i] = pxs[i].x;
			arr[2*i+1] = pxs[i].y;
		}
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
	std::vector<float> arr(2 * count);
	for (int i = 0; i < count; i++) {
		arr[2*i] = pxs[i].x;
		arr[2*i+1] = pxs[i].y;
	}
	return rawBezierTo(arr);
}

bool GiCanvasBase::rawPath(const GiContext* ctx, int count, const Point2d* pxs, const UInt8* types)
{
	bool stroke = checkStroke(ctx);
	bool fill = checkFill(ctx);
	bool ret = stroke || fill;

	if (ret) {
		std::vector<float> arr(2 * count);

		for (int i = 0; i < count; i++) {
			arr[2*i] = pxs[i].x;
			arr[2*i+1] = pxs[i].y;
		}
		ret = rawPath(arr, (const char*)types, stroke, fill);
	}

	return ret;
}

void GiCanvasBase::_clipBoxChanged(const RECT_2D& clipBox)
{
	clipBoxChanged(clipBox.left, clipBox.top, clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
}

bool GiCanvasBase::checkStroke(const GiContext* ctx)
{
	bool changed = !(impl->ctxstatus & 1);

	if (ctx && !ctx->isNullLine())
	{
		if (impl->gictx.getLineColor() != ctx->getLineColor()) {
			impl->gictx.setLineColor(ctx->getLineColor());
			changed = true;
		}
		if (impl->gictx.getLineWidth() != ctx->getLineWidth()) {
			impl->gictx.setLineWidth(ctx->getLineWidth());
			changed = true;
		}
		if (impl->gictx.getLineStyle() != ctx->getLineStyle()) {
			impl->gictx.setLineStyle(ctx->getLineStyle());
			changed = true;
		}
	}

	if (!ctx) ctx = &impl->gictx;
	if (!ctx->isNullLine() && changed)
	{
		impl->ctxstatus |= 1;
		penChanged(*ctx);
	}

	return !ctx->isNullLine();
}

bool GiCanvasBase::checkFill(const GiContext* ctx)
{
	bool changed = !(impl->ctxstatus & 2);

	if (ctx && ctx->hasFillColor())
	{
		if (impl->gictx.getFillColor() != ctx->getFillColor()) {
			impl->gictx.setFillColor(ctx->getFillColor());
			changed = true;
		}
	}
	if (!ctx) ctx = &impl->gictx;
	if (ctx->hasFillColor() && changed)
	{
		impl->ctxstatus |= 2;
		penChanged(*ctx);
	}

	return ctx->hasFillColor();
}

GiCanvasBase ddd;
