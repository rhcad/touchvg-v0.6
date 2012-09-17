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

bool GiCanvasBase::drawLine(float, float, float, float) { return false; }
bool GiCanvasBase::drawLines(const mgvector<float>&) { return false; }
bool GiCanvasBase::drawBeziers(const mgvector<float>&) { return false; }
bool GiCanvasBase::drawPolygon(const mgvector<float>&, bool, bool) { return false; }
bool GiCanvasBase::drawRect(float, float, float, float, bool, bool) { return false; }
bool GiCanvasBase::drawEllipse(float, float, float, float, bool, bool) { return false; }

bool GiCanvasBase::moveTo(float, float) { return false; }
bool GiCanvasBase::lineTo(float, float) { return false; }
bool GiCanvasBase::bezierTo(float,float,float,float,float,float) { return false; }
bool GiCanvasBase::endPath(bool, bool) { return false; }

void GiCanvasBase::clearCachedBitmap(bool) {}
bool GiCanvasBase::drawCachedBitmap(float, float, bool) { return false; }
void GiCanvasBase::saveCachedBitmap(bool) {}
bool GiCanvasBase::hasCachedBitmap(bool) const { return false; }
void GiCanvasBase::clipBoxChanged(float, float, float, float) {}
void GiCanvasBase::antiAliasModeChanged(bool) {}
bool GiCanvasBase::drawCachedBitmap2(const GiCanvas*, float, float, bool) { return false; }
void GiCanvasBase::penChanged(int, float, int) {}
void GiCanvasBase::brushChanged(int) {}
void GiCanvasBase::setNeedRedraw() {}

bool GiCanvasBase::rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2)
{
    return checkStroke(ctx) && drawLine(x1, y1, x2, y2);
}

bool GiCanvasBase::rawLines(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool ret = checkStroke(ctx) && pxs && count > 0;
    if (ret) {
        mgvector<float> arr(&pxs[0].x, 2 * count);
        ret = drawLines(arr);
    }

    return ret;
}

bool GiCanvasBase::rawBeziers(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool ret = checkStroke(ctx) && pxs && count > 0;
    if (ret) {
        mgvector<float> arr(&pxs[0].x, 2 * count);
        ret = drawBeziers(arr);
    }

    return ret;
}

bool GiCanvasBase::rawPolygon(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool stroke = checkStroke(ctx);
    bool fill = checkFill(ctx);
    bool ret = (stroke || fill) && pxs && count > 0;

    if (ret) {
        mgvector<float> arr(&pxs[0].x, 2 * count);
        ret = drawPolygon(arr, stroke, fill);
    }

    return ret;
}

bool GiCanvasBase::rawRect(const GiContext* ctx, float x, float y, float w, float h)
{
    bool stroke = checkStroke(ctx);
    bool fill = checkFill(ctx);
    return (stroke || fill) && drawRect(x, y, w, h, stroke, fill);
}

bool GiCanvasBase::rawEllipse(const GiContext* ctx, float x, float y, float w, float h)
{
    bool stroke = checkStroke(ctx);
    bool fill = checkFill(ctx);
    return (stroke || fill) && drawEllipse(x, y, w, h, stroke, fill);
}

bool GiCanvasBase::rawEndPath(const GiContext* ctx, bool fill)
{
    bool stroke = checkStroke(ctx);
    fill = fill && checkFill(ctx);
    return (stroke || fill) && endPath(stroke, fill);
}

bool GiCanvasBase::rawBezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    return bezierTo(c1x, c1y, c2x, c2y, x, y);
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
        penChanged(ctx->getLineARGB(), _gs.calcPenWidth(ctx->getLineWidth()), ctx->getLineStyle());
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
        brushChanged(ctx->getFillARGB());
    }

    return ctx->hasFillColor();
}

bool GiCanvasBase::beginPaint()
{
    _ctxstatus = 0;

    RECT_2D clipBox = { 0, 0, (float)_xf.getWidth(), (float)_xf.getHeight() };
    _gs._beginPaint(clipBox);

    return true;
}

void GiCanvasBase::endPaint()
{
    _gs._endPaint();
}
