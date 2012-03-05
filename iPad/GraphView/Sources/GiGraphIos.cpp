// GiGraphIos.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "GiGraphIos.h"

class GiGraphIosImpl
{
public:
    CGContextRef    context;
    GiColor         bkColor;
    GiContext       gictx;

    GiGraphIosImpl() : context(NULL), bkColor(GiColor::White())
    {
    }

    CGFloat toFloat(UInt8 c) const
    {
        return (CGFloat)c / 255.0f;
    }

    bool setPen(const GiContext* ctx)
    {
        if (ctx)
        {
            if (ctx->getLineColor() != gictx.getLineColor())
            {
                gictx.setLineColor(ctx->getLineColor());
                if (!ctx->isNullLine())
                {
                    CGContextSetRGBStrokeColor(context, 
                        toFloat(ctx->getLineColor().r),
                        toFloat(ctx->getLineColor().g),
                        toFloat(ctx->getLineColor().b),
                        toFloat(ctx->getLineColor().a));
                }
            }
            if (ctx->getLineWidth() != gictx.getLineWidth())
            {
                gictx.setLineWidth(ctx->getLineWidth());
                CGContextSetLineWidth(context, (CGFloat)ctx->getLineWidth());
            }
            if (ctx->getLineStyle() != gictx.getLineStyle())
            {
                gictx.setLineStyle(ctx->getLineStyle());
            }
        }

        return !gictx.isNullLine();
    }

    bool setBrush(const GiContext* ctx)
    {
        if (ctx)
        {
            if (ctx->getFillColor() != gictx.getFillColor())
            {
                gictx.setFillColor(ctx->getFillColor());
                if (ctx->hasFillColor)
                {
                    CGContextSetRGBFillColor(context, 
                        toFloat(ctx->getFillColor().r),
                        toFloat(ctx->getFillColor().g),
                        toFloat(ctx->getFillColor().b),
                        toFloat(ctx->getFillColor().a));
                }
            }
        }

        return gictx.hasFillColor();
    }
};

GiColor giFromCGColor(CGColorRef color)
{
    const CGFloat *rgba = CGColorGetComponents(color);
    return GiColor((UInt8)mgRound(rgba[0] * 255),
                   (UInt8)mgRound(rgba[1] * 255),
                   (UInt8)mgRound(rgba[2] * 255),
                   (UInt8)mgRound(rgba[3] * 255));
}

GiGraphIos::GiGraphIos(GiTransform& xform)
    : GiGraphics(xform)
{
    m_draw = new GiGraphIosImpl();
}

GiGraphIos::GiGraphIos(const GiGraphIos& src)
    : GiGraphics(src)
{
    m_draw = new GiGraphIosImpl();
}

GiGraphIos::~GiGraphIos()
{
    delete m_draw;
}

GiGraphIos& GiGraphIos::operator=(const GiGraphIos& src)
{
    GiGraphics::operator=(src);
    return *this;
}

bool GiGraphIos::beginPaint(CGContextRef context)
{
    if (isDrawing() || context == NULL)
        return false;

    GiGraphics::beginPaint();
    m_draw->context = context;

    CGContextSetAllowsAntialiasing(context, isAntiAliasMode());
    CGContextSetShouldAntialias(context, isAntiAliasMode());

    return true;
}

void GiGraphIos::endPaint(bool draw)
{
    if (isDrawing())
    {
        m_draw->context = NULL;
        GiGraphics::endPaint();
    }
}

void GiGraphIos::clearWnd()
{
}

bool GiGraphIos::drawCachedBitmap(int x, int y, bool secondBmp)
{
    return false;
}

bool GiGraphIos::drawCachedBitmap2(const GiGraphics* p, bool secondBmp)
{
    return false;
}

void GiGraphIos::saveCachedBitmap(bool secondBmp)
{
}

bool GiGraphIos::hasCachedBitmap(bool secondBmp) const
{
    return false;
}

void GiGraphIos::clearCachedBitmap()
{
}

bool GiGraphIos::isBufferedDrawing() const
{
    return false;
}

bool GiGraphIos::setClipBox(const RECT* prc)
{
    return GiGraphics::setClipBox(prc);
}

bool GiGraphIos::setClipWorld(const Box2d& rectWorld)
{
    return GiGraphics::setClipWorld(rectWorld);
}

GiColor GiGraphIos::getBkColor() const
{
    return m_draw->bkColor;
}

GiColor GiGraphIos::setBkColor(const GiColor& color)
{
    GiColor old(m_draw->bkColor);
    m_draw->bkColor = color;
    return old;
}

GiColor GiGraphIos::getNearestColor(const GiColor& color) const
{
    return color;
}

const GiContext* GiGraphIos::getCurrentContext() const
{
    return &m_draw->gictx;
}

bool GiGraphIos::rawLine(const GiContext* ctx, 
                         int x1, int y1, int x2, int y2)
{
    bool ret = m_draw->setPen(ctx);

    if (ret)
    {
        CGContextMoveToPoint(m_draw->context, x1, y1);
        CGContextAddLineToPoint(m_draw->context, x2, y2);
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawPolyline(const GiContext* ctx, 
                             const POINT* lppt, int count)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->context, lppt[0].x, lppt[0].y);
        for (int i = 1; i < count; i++)
        {
            CGContextAddLineToPoint(m_draw->context, lppt[i].x, lppt[i].y);
        }
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawPolyBezier(const GiContext* ctx, 
                               const POINT* lppt, int count)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->context, lppt[0].x, lppt[0].y);
        for (int i = 1; i < count; i++)
        {
            CGContextAddLineToPoint(m_draw->context, lppt[i].x, lppt[i].y);
        }
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawPolygon(const GiContext* ctx, 
                            const POINT* lppt, int count)
{
    bool ret = count > 1;

    m_draw->setPen(ctx);
    m_draw->setBrush(ctx);

    if (ret)
    {
        CGContextMoveToPoint(m_draw->context, lppt[0].x, lppt[0].y);
        for (int i = 1; i < count; i++)
        {
            CGContextAddLineToPoint(m_draw->context, lppt[i].x, lppt[i].y);
        }
        CGContextClosePath(m_draw->context);
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawRect(const GiContext* ctx, 
                         int x, int y, int w, int h)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextAddRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }
    if (ret)
    {
        CGContextClosePath(m_draw->context);
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawEllipse(const GiContext* ctx, 
                            int x, int y, int w, int h)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextAddEllipseInRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillEllipseInRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }
    if (ret)
    {
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawPolyDraw(const GiContext* ctx, 
                             int count, const POINT* lppt, const UInt8* types)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->context, lppt[0].x, lppt[0].y);
        for (int i = 1; i < count; i++)
        {
            CGContextAddLineToPoint(m_draw->context, lppt[i].x, lppt[i].y);
        }
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawBeginPath()
{
    return true;
}

bool GiGraphIos::rawEndPath(const GiContext* ctx, bool fill)
{
    bool ret = m_draw->setPen(ctx);
    if (fill)
        ret = m_draw->setBrush(ctx) || ret;

    CGContextStrokePath(m_draw->context);

    return ret;
}

bool GiGraphIos::rawMoveTo(int x, int y)
{
    CGContextMoveToPoint(m_draw->context, x, y);
    return true;
}

bool GiGraphIos::rawLineTo(int x, int y)
{
    CGContextAddLineToPoint(m_draw->context, x, y);
    return true;
}

bool GiGraphIos::rawPolyBezierTo(const POINT* lppt, int count)
{
    return false;
}

bool GiGraphIos::rawCloseFigure()
{
    CGContextClosePath(m_draw->context);
    return true;
}
