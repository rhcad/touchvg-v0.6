// ioscanvas.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "ioscanvas.h"
#include <gigraph.h>
#include <CoreGraphics/CGBitmapContext.h>

static const CGFloat patDash[]      = { 5, 5 };
static const CGFloat patDot[]       = { 1, 3 };
static const CGFloat patDashDot[]   = { 10, 2, 2, 2 };
static const CGFloat dashDotdot[]   = { 20, 2, 2, 2, 2, 2 };

struct LPatten { int n; const CGFloat* arr; };
static const LPatten lpats[] = {
    { 0, NULL }, { 2, patDash }, { 2, patDot },
    { 4, patDashDot }, { 6, dashDotdot }
};

class GiCanvasIosImpl
{
public:
    GiCanvasIos*     _gs;
    CGContextRef    _context;
    CGContextRef    _buffctx;
    GiColor         _bkcolor;
    GiContext       _gictx;
    bool            _fast;
    CGImageRef      _caches[2];
    static float    _dpi;

    GiCanvasIosImpl(GiCanvasIos* p) : _gs(p), _context(NULL), _buffctx(NULL)
        , _bkcolor(GiColor::White()), _fast(false)
    {
        _caches[0] = NULL;
        _caches[1] = NULL;
    }

    const GiGraphics* owner() const
    {
        return _gs->owner();
    }
    
    CGContextRef getContext() const
    {
        return _buffctx ? _buffctx : _context;
    }

    CGFloat toFloat(UInt8 c) const
    {
        return (CGFloat)c / 255.0f;
    }
    
    void makeLinePattern(CGFloat* dest, const CGFloat* src, int n, float w)
    {
        for (int i = 0; i < n; i++) {
            dest[i] = src[i] * w;
        }
    }

    bool setPen(const GiContext* ctx)
    {
        if (ctx && !ctx->isNullLine())
        {
            _gictx.setLineColor(ctx->getLineColor());
            _gictx.setLineWidth(ctx->getLineWidth());
            _gictx.setLineStyle(ctx->getLineStyle());
        }
        if (!ctx) ctx = &_gictx;
        if (!ctx->isNullLine())
        {
            GiColor color = owner()->calcPenColor(ctx->getLineColor());
            CGContextSetRGBStrokeColor(getContext(), 
                                       toFloat(color.r), toFloat(color.g),
                                       toFloat(color.b), toFloat(color.a));
            
            float w = owner()->calcPenWidth(ctx->getLineWidth());
            CGContextSetLineWidth(getContext(), _fast && w > 1 ? w - 1 : w);
            
            int style = ctx->getLineStyle();
            CGFloat pattern[6];
            
            if (style >= 0 && style < sizeof(lpats)/sizeof(lpats[0])) {
                if (lpats[style].arr && !_fast) {
                    makeLinePattern(pattern, lpats[style].arr, lpats[style].n, w);
                    CGContextSetLineDash(getContext(), 0, pattern, lpats[style].n);
                }
                else {
                    CGContextSetLineDash(getContext(), 0, NULL, 0);
                }
            }
        }

        return !ctx->isNullLine();
    }

    bool setBrush(const GiContext* ctx)
    {
        if (ctx && ctx->hasFillColor())
        {
            _gictx.setFillColor(ctx->getFillColor());
        }
        if (!ctx) ctx = &_gictx;
        if (ctx->hasFillColor())
        {
            GiColor color = owner()->calcPenColor(ctx->getFillColor());
            CGContextSetRGBFillColor(getContext(), 
                                     toFloat(color.r), toFloat(color.g),
                                     toFloat(color.b), toFloat(color.a));
        }

        return ctx->hasFillColor();
    }
    
    void createBufferBitmap(float width, float height);
};

float GiCanvasIosImpl::_dpi = 132;

GiColor giFromCGColor(CGColorRef color)
{
    int num = CGColorGetNumberOfComponents(color);
    CGColorSpaceModel space = CGColorSpaceGetModel(CGColorGetColorSpace(color));
    const CGFloat *rgba = CGColorGetComponents(color);
    
    if (space == kCGColorSpaceModelMonochrome && num >= 2) {
        UInt8 c = (UInt8)mgRound(rgba[0] * 255);
        return GiColor(c, c, c, (UInt8)mgRound(rgba[1] * 255));
    }
    if (num < 3) {
        return GiColor::Invalid();
    }

    return GiColor((UInt8)mgRound(rgba[0] * 255),
                   (UInt8)mgRound(rgba[1] * 255),
                   (UInt8)mgRound(rgba[2] * 255),
                   (UInt8)mgRound(CGColorGetAlpha(color) * 255));
}

const GiTransform& GiCanvasIos::xf() const
{
    return m_owner->xf();
}

GiCanvasIos::GiCanvasIos(GiGraphics* gs)
{
    gs->_setCanvas(this);
    gs->_xf().setResolution(GiCanvasIos::getScreenDpi());
    m_draw = new GiCanvasIosImpl(this);
}

GiCanvasIos::~GiCanvasIos()
{
    clearCachedBitmap();
    delete m_draw;
}

bool GiCanvasIos::beginPaint(CGContextRef context, bool fast, bool buffered)
{
    if (m_draw->_context || context == NULL)
        return false;

    CGRect rc = CGContextGetClipBoundingBox(context);
    RECT2D clipBox = { rc.origin.x, rc.origin.y, 
        rc.origin.x + rc.size.width, rc.origin.y + rc.size.height };
    m_owner->_beginPaint(clipBox);

    m_draw->_context = context;
    m_draw->_fast = fast;
    if (buffered) {
        m_draw->createBufferBitmap(xf().getWidth(), xf().getHeight());
        context = m_draw->getContext();
    }

    CGContextSetAllowsAntialiasing(context, !fast && m_owner->isAntiAliasMode());
    CGContextSetShouldAntialias(context, !fast && m_owner->isAntiAliasMode());
    CGContextSetFlatness(context, fast ? 20 : 1);

    CGContextSetLineCap(context, kCGLineCapRound);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    m_owner->setMaxPenWidth(-1, 0.5f / CGContextGetCTM(context).a);

    return true;
}

void GiCanvasIos::endPaint(bool draw)
{
    if (m_owner->isDrawing())
    {
        if (draw && m_draw->_buffctx) {
            CGContextRef context = m_draw->_context;
            CGImageRef image = CGBitmapContextCreateImage(m_draw->_buffctx);
            CGRect rect = CGRectMake(0, 0, xf().getWidth(), xf().getHeight());
            
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, xf().getHeight());
            CGContextConcatCTM(context, af);
            CGContextDrawImage(context, rect, image);
            CGContextConcatCTM(context, CGAffineTransformInvert(af));
            
            CGImageRelease(image);
        }
        if (m_draw->_buffctx) {
            CGContextRelease(m_draw->_buffctx);
            m_draw->_buffctx = NULL;
        }
        m_draw->_context = NULL;
        m_owner->_endPaint();
    }
}

CGContextRef GiCanvasIos::bitmapContext()
{
    return m_draw->_buffctx;
}

void GiCanvasIosImpl::createBufferBitmap(float width, float height)
{
    CGAffineTransform af = CGContextGetCTM(_context);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    width *= fabsf(af.a);
    height *= fabsf(af.d);
    _buffctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4,
                                     colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    CGContextTranslateCTM(_buffctx, 0, height);
    CGContextScaleCTM(_buffctx, af.a, af.d);
}

void GiCanvasIos::clearWindow()
{
    CGContextClearRect(m_draw->getContext(), 
        CGRectMake(0, 0, xf().getWidth(), xf().getHeight()));
}

bool GiCanvasIos::drawCachedBitmap(float x, float y, bool secondBmp)
{
    CGImageRef image = m_draw->_caches[secondBmp ? 1 : 0];
    CGContextRef context = m_draw->getContext();
    bool ret = false;
    
    if (context && image) {
        CGRect rect = CGRectMake(x, y, xf().getWidth(), xf().getHeight());
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, xf().getHeight());
        
        CGContextConcatCTM(context, af);
        CGContextDrawImage(context, rect, image);
        CGContextConcatCTM(context, CGAffineTransformInvert(af));
        ret = true;
    }
    
    return ret;
}

bool GiCanvasIos::drawCachedBitmap2(const GiCanvas* p, float x, float y, bool secondBmp)
{
    bool ret = false;
    
    if (p && p->getCanvasType() == getCanvasType()) {
        GiCanvasIos* gs = (GiCanvasIos*)p;
        CGImageRef image = gs->m_draw->_caches[secondBmp ? 1 : 0];
        CGContextRef context = m_draw->getContext();
        
        if (context && image) {
            CGRect rect = CGRectMake(x, y, xf().getWidth(), xf().getHeight());
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, xf().getHeight());
            
            CGContextConcatCTM(context, af);
            CGContextDrawImage(context, rect, image);
            CGContextConcatCTM(context, CGAffineTransformInvert(af));
            ret = true;
        }
    }
    
    return ret;
}

void GiCanvasIos::saveCachedBitmap(bool secondBmp)
{
    int n = secondBmp ? 1 : 0;
    if (m_draw->_caches[n])
        CGImageRelease(m_draw->_caches[n]);
    m_draw->_caches[n] = CGBitmapContextCreateImage(m_draw->getContext());
}

bool GiCanvasIos::hasCachedBitmap(bool secondBmp) const
{
    return !!m_draw->_caches[secondBmp ? 1 : 0];
}

void GiCanvasIos::clearCachedBitmap()
{
    if (m_draw->_caches[0]) {
        CGImageRelease(m_draw->_caches[0]);
        m_draw->_caches[0] = NULL;
    }
    if (m_draw->_caches[1]) {
        CGImageRelease(m_draw->_caches[1]);
        m_draw->_caches[1] = NULL;
    }
}

bool GiCanvasIos::isBufferedDrawing() const
{
    return !!m_draw->_buffctx;
}

void GiCanvasIos::setScreenDpi(float dpi)
{
    GiCanvasIosImpl::_dpi = dpi;
}

float GiCanvasIos::getScreenDpi() const
{
    return GiCanvasIosImpl::_dpi;
}

void GiCanvasIos::_clipBoxChanged(const RECT2D& clipBox)
{
    if (m_draw->_context)
    {
        CGRect rect = CGRectMake(clipBox.left, clipBox.top, 
                                 clipBox.right - clipBox.left, 
                                 clipBox.bottom - clipBox.top);
        CGContextClipToRect(m_draw->getContext(), rect);
    }
}

GiColor GiCanvasIos::getBkColor() const
{
    return m_draw->_bkcolor;
}

GiColor GiCanvasIos::setBkColor(const GiColor& color)
{
    GiColor old(m_draw->_bkcolor);
    m_draw->_bkcolor = color;
    return old;
}

GiColor GiCanvasIos::getNearestColor(const GiColor& color) const
{
    return color;
}

void GiCanvasIos::_antiAliasModeChanged(bool antiAlias)
{
    if (m_draw->_context) {
        antiAlias = !m_draw->_fast && antiAlias;
        CGContextSetAllowsAntialiasing(m_draw->getContext(), antiAlias);
        CGContextSetShouldAntialias(m_draw->getContext(), antiAlias);
    }
}

const GiContext* GiCanvasIos::getCurrentContext() const
{
    return &m_draw->_gictx;
}

bool GiCanvasIos::rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2)
{
    bool ret = m_draw->setPen(ctx);

    if (ret)
    {
        CGContextMoveToPoint(m_draw->getContext(), x1, y1);
        CGContextAddLineToPoint(m_draw->getContext(), x2, y2);
        CGContextStrokePath(m_draw->getContext());
    }

    return ret;
}

bool GiCanvasIos::rawLines(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->getContext(), pxs[0].x, pxs[0].y);
        for (int i = 1; i < count; i++) {
            CGContextAddLineToPoint(m_draw->getContext(), pxs[i].x, pxs[i].y);
        }
        CGContextStrokePath(m_draw->getContext());
    }

    return ret;
}

bool GiCanvasIos::rawBeziers(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->getContext(), pxs[0].x, pxs[0].y);
        for (int i = 1; i + 2 < count; i += 3)
        {
            CGContextAddCurveToPoint(m_draw->getContext(), 
                pxs[i+0].x, pxs[i+0].y,
                pxs[i+1].x, pxs[i+1].y,
                pxs[i+2].x, pxs[i+2].y);
        }
        CGContextStrokePath(m_draw->getContext());
    }

    return ret;
}

bool GiCanvasIos::rawPolygon(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool usepen = m_draw->setPen(ctx);
    bool usebrush = m_draw->setBrush(ctx);
    bool ret = count > 1 && (usepen || usebrush);

    if (ret)
    {
        if (usepen) {
            CGContextMoveToPoint(m_draw->getContext(), pxs[0].x, pxs[0].y);
            for (int i = 1; i < count; i++) {
                CGContextAddLineToPoint(m_draw->getContext(), pxs[i].x, pxs[i].y);
            }
            CGContextClosePath(m_draw->getContext());
            CGContextStrokePath(m_draw->getContext());
        }
        if (usebrush) {
            CGContextMoveToPoint(m_draw->getContext(), pxs[0].x, pxs[0].y);
            for (int i = 1; i < count; i++) {
                CGContextAddLineToPoint(m_draw->getContext(), pxs[i].x, pxs[i].y);
            }
            CGContextClosePath(m_draw->getContext());
            CGContextFillPath(m_draw->getContext());
        }
    }

    return ret;
}

bool GiCanvasIos::rawRect(const GiContext* ctx, float x, float y, float w, float h)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextStrokeRect(m_draw->getContext(), CGRectMake(x, y, w, h));
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillRect(m_draw->getContext(), CGRectMake(x, y, w, h));
        ret = true;
    }

    return ret;
}

bool GiCanvasIos::rawEllipse(const GiContext* ctx, float x, float y, float w, float h)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextStrokeEllipseInRect(m_draw->getContext(), CGRectMake(x, y, w, h));
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillEllipseInRect(m_draw->getContext(), CGRectMake(x, y, w, h));
        ret = true;
    }

    return ret;
}

bool GiCanvasIos::rawPath(const GiContext* ctx, 
                          int count, const Point2d* pxs, const UInt8* types)
{
    CGContextBeginPath(m_draw->getContext());
    
    for (int i = 0; i < count; i++)
    {
        switch (types[i] & ~kGiCloseFigure)
        {
        case kGiMoveTo:
            CGContextMoveToPoint(m_draw->getContext(), pxs[i].x, pxs[i].y);
            break;

        case kGiLineTo:
            CGContextAddLineToPoint(m_draw->getContext(), pxs[i].x, pxs[i].y);
            break;

        case kGiBeziersTo:
            if (i + 2 >= count)
                return false;
            CGContextAddCurveToPoint(m_draw->getContext(), 
                pxs[i+0].x, pxs[i+0].y,
                pxs[i+1].x, pxs[i+1].y,
                pxs[i+2].x, pxs[i+2].y);
            i += 2;
            break;

        default:
            return false;
        }
        if (types[i] & kGiCloseFigure)
            CGContextClosePath(m_draw->getContext());
    }

    bool ret = false;

    if (count > 1 && m_draw->setPen(ctx))
    {
        CGContextStrokePath(m_draw->getContext());
        ret = true;
    }
    if (count > 1 && m_draw->setBrush(ctx))
    {
        CGContextFillPath(m_draw->getContext());
        ret = true;
    }

    return ret;
}

bool GiCanvasIos::rawBeginPath()
{
    CGContextBeginPath(m_draw->getContext());
    return true;
}

bool GiCanvasIos::rawEndPath(const GiContext* ctx, bool fill)
{
    bool ret = false;

    if (m_draw->setPen(ctx))
    {
        CGContextStrokePath(m_draw->getContext());
        ret = true;
    }
    if (m_draw->setBrush(ctx))
    {
        CGContextFillPath(m_draw->getContext());
        ret = true;
    }

    return ret;
}

bool GiCanvasIos::rawMoveTo(float x, float y)
{
    CGContextMoveToPoint(m_draw->getContext(), x, y);
    return true;
}

bool GiCanvasIos::rawLineTo(float x, float y)
{
    CGContextAddLineToPoint(m_draw->getContext(), x, y);
    return true;
}

bool GiCanvasIos::rawBezierTo(const Point2d* pxs, int count)
{
    bool ret = pxs && count > 2;

    for (int i = 0; i + 2 < count; i += 3)
    {
        CGContextAddCurveToPoint(m_draw->getContext(), 
            pxs[i+0].x, pxs[i+0].y,
            pxs[i+1].x, pxs[i+1].y,
            pxs[i+2].x, pxs[i+2].y);
    }

    return ret;
}

bool GiCanvasIos::rawClosePath()
{
    CGContextClosePath(m_draw->getContext());
    return true;
}
