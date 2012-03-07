// GiGraphIos.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "GiGraphIos.h"

class GiGraphIosImpl
{
public:
    GiGraphIos*     pthis;
    CGContextRef    context;
    GiColor         bkColor;
    GiContext       gictx;

    GiGraphIosImpl(GiGraphIos* p) : pthis(p), context(NULL), bkColor(GiColor::White())
    {
    }

    CGFloat toFloat(UInt8 c) const
    {
        return (CGFloat)c / 255.0f;
    }

    bool setPen(const GiContext* ctx)
    {
        if (ctx && !ctx->isNullLine())
        {
            gictx.setLineColor(ctx->getLineColor());
            gictx.setLineWidth(ctx->getLineWidth());
            gictx.setLineStyle(ctx->getLineStyle());
        }
        if (!gictx.isNullLine())
        {
            GiColor color = pthis->calcPenColor(gictx.getLineColor());
            CGContextSetRGBStrokeColor(context, toFloat(color.r), toFloat(color.g),
                                       toFloat(color.b), toFloat(color.a));
            CGContextSetLineWidth(context, (CGFloat)pthis->calcPenWidth(gictx.getLineWidth()));
        }

        return !gictx.isNullLine();
    }

    bool setBrush(const GiContext* ctx)
    {
        if (ctx && ctx->hasFillColor())
        {
            gictx.setFillColor(ctx->getFillColor());
        }
        if (gictx.hasFillColor)
        {
            GiColor color = pthis->calcPenColor(gictx.getFillColor());
            CGContextSetRGBFillColor(context, toFloat(color.r), toFloat(color.g),
                                     toFloat(color.b), toFloat(color.a));
        }

        return gictx.hasFillColor();
    }
};

GiColor giFromCGColor(CGColorRef color)
{
    const CGFloat *rgba = CGColorGetComponents(color);
    return rgba ? GiColor((UInt8)mgRound(rgba[0] * 255),
                          (UInt8)mgRound(rgba[1] * 255),
                          (UInt8)mgRound(rgba[2] * 255),
                          (UInt8)mgRound(rgba[3] * 255)) : GICOLOR_INVALID;
}

GiGraphIos::GiGraphIos(GiTransform& xform)
    : GiGraphics(xform)
{
    m_draw = new GiGraphIosImpl(this);
    xform.setResolution(GiGraphIos::getScreenDpi());
}

GiGraphIos::GiGraphIos(const GiGraphIos& src)
    : GiGraphics(src)
{
    m_draw = new GiGraphIosImpl(this);
    operator=(src);
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

    CGRect rc = CGContextGetClipBoundingBox(context);
    RECT clipBox = { rc.origin.x, rc.origin.y, rc.size.width, rc.size.height };

    m_draw->context = context;
    GiGraphics::beginPaint(&clipBox);

    CGContextSetAllowsAntialiasing(context, isAntiAliasMode());
    CGContextSetShouldAntialias(context, isAntiAliasMode());

    CGContextSetMiterLimit(context, (CGFloat)(1.0 / sin(_M_PI_6)));  // 60 deg.

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
    CGContextClearRect(m_draw->context, 
        CGRectMake(0, 0, xf().getWidth(), xf().getWidth()));
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

int GiGraphIos::getScreenDpi() const
{
    return 132; // How to get the actual DPI?
}

bool GiGraphIos::setClipBox(const RECT* prc)
{
    bool ret = GiGraphics::setClipBox(prc);

    if (ret && m_draw->context)
    {
        RECT clipBox;
        getClipBox(&clipBox);
        
        CGRect rect = CGRectMake(clipBox.left, clipBox.top, 
                                 clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
        CGContextClipToRect(m_draw->context, rect);
    }

    return ret;
}

bool GiGraphIos::setClipWorld(const Box2d& rectWorld)
{
    bool ret = GiGraphics::setClipWorld(rectWorld);

    if (ret && m_draw->context)
    {
        RECT clipBox;
        getClipBox(&clipBox);
        
        CGRect rect = CGRectMake(clipBox.left, clipBox.top, 
                                 clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
        CGContextClipToRect(m_draw->context, rect);
    }

    return ret;
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
        for (int i = 1; i + 2 < count; i += 3)
        {
            CGContextAddCurveToPoint(m_draw->context, 
                lppt[i+0].x, lppt[i+0].y,
                lppt[i+1].x, lppt[i+1].y,
                lppt[i+2].x, lppt[i+2].y);
        }
        CGContextStrokePath(m_draw->context);
    }

    return ret;
}

bool GiGraphIos::rawPolygon(const GiContext* ctx, 
                            const POINT* lppt, int count)
{
    bool ret = false;

    if (count > 1)
    {
        CGContextMoveToPoint(m_draw->context, lppt[0].x, lppt[0].y);
        for (int i = 1; i < count; i++)
        {
            CGContextAddLineToPoint(m_draw->context, lppt[i].x, lppt[i].y);
        }
        CGContextClosePath(m_draw->context);

        if (m_draw->setPen(ctx))
        {
            CGContextStrokePath(m_draw->context);
            ret = true;
        }
        if (m_draw->setBrush(ctx))
        {
            CGContextFillPath(m_draw->context);
            ret = true;
        }
    }

    return ret;
}

bool GiGraphIos::rawRect(const GiContext* ctx, 
                         int x, int y, int w, int h)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextStrokeRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }

    return ret;
}

bool GiGraphIos::rawEllipse(const GiContext* ctx, 
                            int x, int y, int w, int h)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextStrokeEllipseInRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillEllipseInRect(m_draw->context, CGRectMake(x, y, w, h));
        ret = true;
    }

    return ret;
}

#ifndef PT_LINETO
#define PT_CLOSEFIGURE      0x01
#define PT_LINETO           0x02
#define PT_BEZIERTO         0x04
#define PT_MOVETO           0x06
#endif // PT_LINETO

bool GiGraphIos::rawPolyDraw(const GiContext* ctx, 
                             int count, const POINT* lppt, const UInt8* types)
{
    for (int i = 0; i < count; i++)
    {
        switch (types[i] & ~PT_CLOSEFIGURE)
        {
        case PT_MOVETO:
            CGContextMoveToPoint(m_draw->context, lppt[i].x, lppt[i].y);
            break;

        case PT_LINETO:
            CGContextAddLineToPoint(m_draw->context, lppt[i].x, lppt[i].y);
            break;

        case PT_BEZIERTO:
            if (i + 2 >= count)
                return false;
            CGContextAddCurveToPoint(m_draw->context, 
                lppt[i+0].x, lppt[i+0].y,
                lppt[i+1].x, lppt[i+1].y,
                lppt[i+2].x, lppt[i+2].y);
            i += 2;
            break;

        default:
            return false;
        }
        if (types[i] & PT_CLOSEFIGURE)
            CGContextClosePath(m_draw->context);
    }

    bool ret = false;

    if (count > 1 && m_draw->setPen(ctx))
    {
        CGContextStrokePath(m_draw->context);
        ret = true;
    }
    if (count > 1 && m_draw->setBrush(ctx))
    {
        CGContextFillPath(m_draw->context);
        ret = true;
    }

    return ret;
}

bool GiGraphIos::rawBeginPath()
{
    CGContextBeginPath(m_draw->context);
    return true;
}

bool GiGraphIos::rawEndPath(const GiContext* ctx, bool fill)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextStrokePath(m_draw->context);
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillPath(m_draw->context);
        ret = true;
    }

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
    bool ret = lppt && count > 2;

    for (int i = 0; i + 2 < count; i += 3)
    {
        CGContextAddCurveToPoint(m_draw->context, 
            lppt[i+0].x, lppt[i+0].y,
            lppt[i+1].x, lppt[i+1].y,
            lppt[i+2].x, lppt[i+2].y);
    }

    return ret;
}

bool GiGraphIos::rawCloseFigure()
{
    CGContextClosePath(m_draw->context);
    return true;
}
