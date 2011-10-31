// GiGraphIos.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "GiGraphIos.h"
#include <CoreGraphics/CGBitmapContext.h>
#include <stdlib.h>

static const CGFloat patDash[]      = { 5, 5 };
static const CGFloat patDot[]       = { 1, 3 };
static const CGFloat patDashDot[]   = { 10, 2, 2, 2 };
static const CGFloat dashDotdot[]   = { 20, 2, 2, 2, 2, 2 };
struct LPatten { int n; const CGFloat* arr; };
static const LPatten lpattens[] = {
    { 0, NULL }, { 2, patDash }, { 2, patDot }, { 4, patDashDot }, { 6, dashDotdot }
};

class GiGraphIosImpl
{
public:
    GiGraphIos*     _this;
    CGContextRef    _context;
    CGContextRef    _buffctx;
    GiColor         _bkcolor;
    GiContext       _gictx;
    bool            _fast;
    CGImageRef      _caches[2];
    static int      _dpi;

    GiGraphIosImpl(GiGraphIos* p) : _this(p), _context(NULL), _buffctx(NULL)
        , _bkcolor(GiColor::White()), _fast(false)
    {
        _caches[0] = NULL;
        _caches[1] = NULL;
    }
    
    CGContextRef getContext() const
    {
        return _buffctx ? _buffctx : _context;
    }

    CGFloat toFloat(UInt8 c) const
    {
        return (CGFloat)c / 255.0f;
    }
    
    void makeLinePattern(CGFloat* dest, const CGFloat* src, int n, int w)
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
            GiColor color = _this->calcPenColor(ctx->getLineColor());
            CGContextSetRGBStrokeColor(getContext(), toFloat(color.r), toFloat(color.g),
                                       toFloat(color.b), toFloat(color.a));
            int w = _this->calcPenWidth(ctx->getLineWidth());
            CGContextSetLineWidth(getContext(), _fast && w > 1 ? w - 1 : w);
            
            int style = ctx->getLineStyle();
            CGFloat pattern[6];
            
            if (style >= 0 && style < sizeof(lpattens)/sizeof(lpattens[0])) {
                if (lpattens[style].arr && !_fast) {
                    makeLinePattern(pattern, lpattens[style].arr, lpattens[style].n, w);
                    CGContextSetLineDash(getContext(), 0, pattern, lpattens[style].n);
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
            GiColor color = _this->calcPenColor(ctx->getFillColor());
            CGContextSetRGBFillColor(getContext(), toFloat(color.r), toFloat(color.g),
                                     toFloat(color.b), toFloat(color.a));
        }

        return ctx->hasFillColor();
    }
    
    void createBufferBitmap(int width, int height);
};

int GiGraphIosImpl::_dpi = 160;

GiColor giFromCGColor(CGColorRef color)
{
    int num = CGColorGetNumberOfComponents(color);
    CGColorSpaceModel space = CGColorSpaceGetModel(CGColorGetColorSpace(color));
    const CGFloat *rgba = CGColorGetComponents(color);
    
    if (space == kCGColorSpaceModelMonochrome && num >= 2) {
        UInt8 c = (UInt8)mgRound(rgba[0] * 255);
        return GiColor(c, c, c, (UInt8)mgRound(rgba[1] * 255));
    }
    return num >= 3 ? GiColor((UInt8)mgRound(rgba[0] * 255),
                              (UInt8)mgRound(rgba[1] * 255),
                              (UInt8)mgRound(rgba[2] * 255),
                              (UInt8)mgRound(CGColorGetAlpha(color) * 255)) : GiColor::Invalid();
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
    clearCachedBitmap();
    delete m_draw;
}

GiGraphIos& GiGraphIos::operator=(const GiGraphIos& src)
{
    GiGraphics::operator=(src);
    return *this;
}

bool GiGraphIos::beginPaint(CGContextRef context, bool buffered, bool fast)
{
    if (isDrawing() || context == NULL)
        return false;

    CGRect rc = CGContextGetClipBoundingBox(context);
    RECT clipBox = { rc.origin.x, rc.origin.y, rc.size.width, rc.size.height };
    GiGraphics::beginPaint(&clipBox);

    m_draw->_context = context;
    m_draw->_fast = fast;
    if (buffered) {
        m_draw->createBufferBitmap(xf().getWidth(), xf().getHeight());
        context = m_draw->getContext();
    }

    CGContextSetAllowsAntialiasing(context, !fast && isAntiAliasMode());
    CGContextSetShouldAntialias(context, !fast && isAntiAliasMode());
    CGContextSetFlatness(context, fast ? 20 : 1);

    CGContextSetMiterLimit(context, (CGFloat)(1.0 / sin(_M_PI_6)));  // 60 deg.

    return true;
}

void GiGraphIos::endPaint(bool draw)
{
    if (isDrawing())
    {
        if (draw && m_draw->_buffctx) {
            CGImageRef image = CGBitmapContextCreateImage(m_draw->_buffctx);
            CGContextDrawImage(m_draw->_context, CGRectMake(0, 0, xf().getWidth(), xf().getHeight()), image);
            CGImageRelease(image);
        }
        if (m_draw->_buffctx) {
            CGContextRelease(m_draw->_buffctx);
            m_draw->_buffctx = NULL;
        }
        m_draw->_context = NULL;
        GiGraphics::endPaint();
    }
}

void GiGraphIosImpl::createBufferBitmap(int width, int height)
{
    _buffctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4,
                                     CGColorSpaceCreateDeviceRGB(), 
                                     kCGImageAlphaPremultipliedLast);
}

void GiGraphIos::clearWnd()
{
    CGContextClearRect(m_draw->getContext(), 
        CGRectMake(0, 0, xf().getWidth(), xf().getWidth()));
}

bool GiGraphIos::drawCachedBitmap(int x, int y, bool secondBmp)
{
    CGImageRef img = m_draw->_caches[secondBmp ? 1 : 0];
    bool ret = false;
    
    if (m_draw->_context && img) {
        CGContextDrawImage(m_draw->getContext(), 
                           CGRectMake(x, y, CGImageGetWidth(img), CGImageGetHeight(img)), 
                           img);
        ret = true;
    }
    
    return ret;
}

bool GiGraphIos::drawCachedBitmap2(const GiGraphics* p, bool secondBmp)
{
    bool ret = false;
    
    if (p && p->getGraphType() == getGraphType()) {
        GiGraphIos* gs = (GiGraphIos*)p;
        CGImageRef img = gs->m_draw->_caches[secondBmp ? 1 : 0];
        
        if (m_draw->_context && img) {
            CGContextDrawImage(m_draw->getContext(), 
                               CGRectMake(0, 0, CGImageGetWidth(img), CGImageGetHeight(img)), 
                               img);
            ret = true;
        }
    }
    
    return ret;
}

void GiGraphIos::saveCachedBitmap(bool secondBmp)
{
    int n = secondBmp ? 1 : 0;
    if (m_draw->_caches[n])
        CGImageRelease(m_draw->_caches[n]);
    m_draw->_caches[n] = CGBitmapContextCreateImage(m_draw->getContext());
}

bool GiGraphIos::hasCachedBitmap(bool secondBmp) const
{
    return !!m_draw->_caches[secondBmp ? 1 : 0];
}

void GiGraphIos::clearCachedBitmap()
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

bool GiGraphIos::isBufferedDrawing() const
{
    return !!m_draw->_buffctx;
}

void GiGraphIos::setScreenDpi(int dpi)
{
    GiGraphIosImpl::_dpi = dpi;
}

int GiGraphIos::getScreenDpi() const
{
    return GiGraphIosImpl::_dpi;
}

bool GiGraphIos::setClipBox(const RECT* prc)
{
    bool ret = GiGraphics::setClipBox(prc);

    if (ret && m_draw->_context)
    {
        RECT clipBox;
        getClipBox(&clipBox);
        
        CGRect rect = CGRectMake(clipBox.left, clipBox.top, 
                                 clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
        CGContextClipToRect(m_draw->getContext(), rect);
    }

    return ret;
}

bool GiGraphIos::setClipWorld(const Box2d& rectWorld)
{
    bool ret = GiGraphics::setClipWorld(rectWorld);

    if (ret && m_draw->_context)
    {
        RECT clipBox;
        getClipBox(&clipBox);
        
        CGRect rect = CGRectMake(clipBox.left, clipBox.top, 
                                 clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
        CGContextClipToRect(m_draw->getContext(), rect);
    }

    return ret;
}

GiColor GiGraphIos::getBkColor() const
{
    return m_draw->_bkcolor;
}

GiColor GiGraphIos::setBkColor(const GiColor& color)
{
    GiColor old(m_draw->_bkcolor);
    m_draw->_bkcolor = color;
    return old;
}

GiColor GiGraphIos::getNearestColor(const GiColor& color) const
{
    return color;
}

const GiContext* GiGraphIos::getCurrentContext() const
{
    return &m_draw->_gictx;
}

bool GiGraphIos::rawLine(const GiContext* ctx, 
                         int x1, int y1, int x2, int y2)
{
    bool ret = m_draw->setPen(ctx);

    if (ret)
    {
        if (x2 == x1 && y2 == y1) {
            float w = calcPenWidth(m_draw->_gictx.getLineWidth());
            float w2 = w / 2.0f;
            CGContextMoveToPoint(m_draw->getContext(), x1 - w2, y1 - w2);
            CGContextAddLineToPoint(m_draw->getContext(), x2 + w - w2 - 1, y2 + w - w2 - 1);
        }
        else {
            CGContextMoveToPoint(m_draw->getContext(), x1, y1);
            CGContextAddLineToPoint(m_draw->getContext(), x2, y2);
        }
        CGContextStrokePath(m_draw->getContext());
    }

    return ret;
}

bool GiGraphIos::rawPolyline(const GiContext* ctx, 
                             const POINT* lppt, int count)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->getContext(), lppt[0].x, lppt[0].y);
        for (int i = 1; i < count; i++) {
            CGContextAddLineToPoint(m_draw->getContext(), lppt[i].x, lppt[i].y);
        }
        CGContextStrokePath(m_draw->getContext());
    }

    return ret;
}

bool GiGraphIos::rawPolyBezier(const GiContext* ctx, 
                               const POINT* lppt, int count)
{
    bool ret = m_draw->setPen(ctx) && count > 1;

    if (ret)
    {
        CGContextMoveToPoint(m_draw->getContext(), lppt[0].x, lppt[0].y);
        for (int i = 1; i + 2 < count; i += 3)
        {
            CGContextAddCurveToPoint(m_draw->getContext(), 
                lppt[i+0].x, lppt[i+0].y,
                lppt[i+1].x, lppt[i+1].y,
                lppt[i+2].x, lppt[i+2].y);
        }
        CGContextStrokePath(m_draw->getContext());
    }

    return ret;
}

bool GiGraphIos::rawPolygon(const GiContext* ctx, 
                            const POINT* lppt, int count)
{
    bool usepen = m_draw->setPen(ctx);
    bool usebrush = m_draw->setBrush(ctx);
    bool ret = count > 1 && (usepen || usebrush);

    if (ret)
    {
        if (usepen) {
            CGContextMoveToPoint(m_draw->getContext(), lppt[0].x, lppt[0].y);
            for (int i = 1; i < count; i++) {
                CGContextAddLineToPoint(m_draw->getContext(), lppt[i].x, lppt[i].y);
            }
            CGContextClosePath(m_draw->getContext());
            CGContextStrokePath(m_draw->getContext());
        }
        if (usebrush) {
            CGContextMoveToPoint(m_draw->getContext(), lppt[0].x, lppt[0].y);
            for (int i = 1; i < count; i++) {
                CGContextAddLineToPoint(m_draw->getContext(), lppt[i].x, lppt[i].y);
            }
            CGContextClosePath(m_draw->getContext());
            CGContextFillPath(m_draw->getContext());
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

bool GiGraphIos::rawEllipse(const GiContext* ctx, 
                            int x, int y, int w, int h)
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

#ifndef PT_LINETO
#define PT_CLOSEFIGURE      0x01
#define PT_LINETO           0x02
#define PT_BEZIERTO         0x04
#define PT_MOVETO           0x06
#endif // PT_LINETO

bool GiGraphIos::rawPolyDraw(const GiContext* ctx, 
                             int count, const POINT* lppt, const UInt8* types)
{
    CGContextBeginPath(m_draw->getContext());
    
    for (int i = 0; i < count; i++)
    {
        switch (types[i] & ~PT_CLOSEFIGURE)
        {
        case PT_MOVETO:
            CGContextMoveToPoint(m_draw->getContext(), lppt[i].x, lppt[i].y);
            break;

        case PT_LINETO:
            CGContextAddLineToPoint(m_draw->getContext(), lppt[i].x, lppt[i].y);
            break;

        case PT_BEZIERTO:
            if (i + 2 >= count)
                return false;
            CGContextAddCurveToPoint(m_draw->getContext(), 
                lppt[i+0].x, lppt[i+0].y,
                lppt[i+1].x, lppt[i+1].y,
                lppt[i+2].x, lppt[i+2].y);
            i += 2;
            break;

        default:
            return false;
        }
        if (types[i] & PT_CLOSEFIGURE)
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

bool GiGraphIos::rawBeginPath()
{
    CGContextBeginPath(m_draw->getContext());
    return true;
}

bool GiGraphIos::rawEndPath(const GiContext* ctx, bool fill)
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

bool GiGraphIos::rawMoveTo(int x, int y)
{
    CGContextMoveToPoint(m_draw->getContext(), x, y);
    return true;
}

bool GiGraphIos::rawLineTo(int x, int y)
{
    CGContextAddLineToPoint(m_draw->getContext(), x, y);
    return true;
}

bool GiGraphIos::rawPolyBezierTo(const POINT* lppt, int count)
{
    bool ret = lppt && count > 2;

    for (int i = 0; i + 2 < count; i += 3)
    {
        CGContextAddCurveToPoint(m_draw->getContext(), 
            lppt[i+0].x, lppt[i+0].y,
            lppt[i+1].x, lppt[i+1].y,
            lppt[i+2].x, lppt[i+2].y);
    }

    return ret;
}

bool GiGraphIos::rawCloseFigure()
{
    CGContextClosePath(m_draw->getContext());
    return true;
}
