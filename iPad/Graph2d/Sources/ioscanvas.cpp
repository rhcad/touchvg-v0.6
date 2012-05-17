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

//! GiCanvasIos 的实现类
class GiCanvasIosImpl
{
public:
    GiCanvasIos*     _gs;               //!< 拥有者
    CGContextRef    _context;           //!< 当前绘图上下文
    CGContextRef    _buffctx;           //!< 缓冲位图上下文
    GiColor         _bkcolor;           //!< 背景色
    GiContext       _gictx;             //!< 当前画笔和画刷
    bool            _ctxused[2];        //!< beginPaint后是否设置过画笔、画刷
    bool            _fast;              //!< 是否粗略显示
    CGImageRef      _caches[2];         //!< 后备缓冲图
    static float    _dpi;               //!< 屏幕每英寸的点数

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
    
    const GiTransform& xf() const
    {
        return owner()->xf();
    }
    
    CGContextRef getContext() const
    {
        return _buffctx ? _buffctx : _context;
    }
    
    long width() const
    {
        return xf().getWidth();
    }
    
    long height() const
    {
        return xf().getHeight();
    }

    CGFloat toFloat(UInt8 c) const
    {
        return (CGFloat)c / 255.0f;
    }
    
    void makeLinePattern(CGFloat* dest, const CGFloat* src, int n, float w)
    {
        for (int i = 0; i < n; i++) {
            dest[i] = src[i] * mgMax(w, 1.f);
        }
    }

    bool setPen(const GiContext* ctx)
    {
        bool changed = !_ctxused[0];
        
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
            _ctxused[0] = true;
            
            GiColor color = ctx->getLineColor();
            if (owner())
                color = owner()->calcPenColor(color);
            CGContextSetRGBStrokeColor(getContext(), 
                                       toFloat(color.r), toFloat(color.g),
                                       toFloat(color.b), toFloat(color.a));
            
            float w = ctx->getLineWidth();
            w = owner() ? owner()->calcPenWidth(w) : (w < 0 ? -w : 1);
            CGContextSetLineWidth(getContext(), _fast && w > 1 ? w - 1 : w); // 不是反走样就细一点
            
            int style = ctx->getLineStyle();
            CGFloat pattern[6];
            
            if (style >= 0 && style < sizeof(lpats)/sizeof(lpats[0])) {
                if (lpats[style].arr && !_fast) {                           // 快速画时不要线型
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
        bool changed = !_ctxused[1];
        
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
            _ctxused[1] = true;
            
            GiColor color = ctx->getFillColor();
            if (owner())
                color = owner()->calcPenColor(color);
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

GiCanvasIos::GiCanvasIos(GiGraphics* gs)
{
    if (gs) {
        gs->_setCanvas(this);
        gs->_xf().setResolution(GiCanvasIos::getScreenDpi());
    }
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

    if (owner()) {
        CGRect rc = CGContextGetClipBoundingBox(context);
        RECT2D clipBox = { rc.origin.x, rc.origin.y, 
            rc.origin.x + rc.size.width, rc.origin.y + rc.size.height };
        owner()->_beginPaint(clipBox);
    }

    m_draw->_context = context;
    m_draw->_fast = fast;
    m_draw->_ctxused[0] = false;
    m_draw->_ctxused[1] = false;
    
    if (buffered && owner()) {
        m_draw->createBufferBitmap(m_draw->width(), m_draw->height());
        context = m_draw->getContext();
    }

    bool antiAlias = !fast && (!owner() || owner()->isAntiAliasMode());
    CGContextSetAllowsAntialiasing(context, antiAlias);
    CGContextSetShouldAntialias(context, antiAlias);
    CGContextSetFlatness(context, fast ? 20 : 1);

    CGContextSetLineCap(context, kCGLineCapRound);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    if (owner())        // 设置最小线宽为0.5像素，使用屏幕放大倍数以便得到实际像素值
        owner()->setMaxPenWidth(-1, 0.5f / CGContextGetCTM(context).a);

    return true;
}

void GiCanvasIos::endPaint(bool draw)
{
    if (m_draw->_context)
    {
        if (draw && m_draw->_buffctx) {
            CGContextRef context = m_draw->_context;
            CGImageRef image = CGBitmapContextCreateImage(m_draw->_buffctx);
            CGRect rect = CGRectMake(0, 0, m_draw->width(), m_draw->height()); // 逻辑宽高点数
            
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
            CGContextConcatCTM(context, af);    // 图像是朝上的，上下文坐标系朝下，上下颠倒显示
            CGContextDrawImage(context, rect, image);
            CGContextConcatCTM(context, CGAffineTransformInvert(af));   // 恢复成坐标系朝下
            
            CGImageRelease(image);
        }
        if (m_draw->_buffctx) {
            CGContextRelease(m_draw->_buffctx);
            m_draw->_buffctx = NULL;
        }
        m_draw->_context = NULL;
        if (owner())
            owner()->_endPaint();
    }
}

CGImageRef GiCanvasIos::cachedBitmap(bool invert)
{
    CGImageRef image = m_draw->_caches[0];
    if (!image || !invert)
        return image;                       // 调用者不能释放图像
    
    size_t w = CGImageGetWidth(image);      // 图像宽度，像素单位，不是点单位
    size_t h = CGImageGetHeight(image);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(NULL, w, h, 8, w * 4,
                                                 colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, h);
    CGContextConcatCTM(context, af);        // 图像是朝上的，上下文坐标系朝下，上下颠倒显示
    CGContextDrawImage(context, CGRectMake(0, 0, w, h), image);
    CGContextConcatCTM(context, CGAffineTransformInvert(af));
    
    CGImageRef newimg = CGBitmapContextCreateImage(context);    // 得到上下颠倒的新图像
    
    CGContextRelease(context);
    
    return newimg;                          // 由调用者释放图像, CGImageRelease
}

void GiCanvasIosImpl::createBufferBitmap(float width, float height)
{
    CGAffineTransform af = CGContextGetCTM(_context);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    width *= fabsf(af.a);       // 点数宽度转为像素宽度
    height *= fabsf(af.d);
    _buffctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4,
                                     colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    // 坐标系改为Y朝下，原点在左上角，这样除了放大倍数为1外，其余就与 _context 坐标系一致
    CGContextTranslateCTM(_buffctx, 0, height);
    CGContextScaleCTM(_buffctx, af.a, af.d);
}

void GiCanvasIos::clearWindow()
{
    if (owner() && m_draw->getContext()) {
        CGContextClearRect(m_draw->getContext(), 
                           CGRectMake(0, 0, m_draw->width(), m_draw->height()));
    }
}

bool GiCanvasIos::drawCachedBitmap(float x, float y, bool secondBmp)
{
    CGImageRef image = m_draw->_caches[secondBmp ? 1 : 0];
    CGContextRef context = m_draw->getContext();
    bool ret = false;
    
    if (context && image) {
        CGRect rect = CGRectMake(x, y, m_draw->width(), m_draw->height());
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
        
        CGContextConcatCTM(context, af);    // 图像是朝上的，上下文坐标系朝下，上下颠倒显示
        CGContextDrawImage(context, rect, image);
        CGContextConcatCTM(context, CGAffineTransformInvert(af));   // 恢复成坐标系朝下
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
            CGRect rect = CGRectMake(x, y, m_draw->width(), m_draw->height());
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
            
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
    if (owner() && m_draw->getContext()) {
        m_draw->_caches[n] = CGBitmapContextCreateImage(m_draw->getContext());
    }
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
        CGContextMoveToPoint(m_draw->getContext(), pxs[0].x, pxs[0].y);
        for (int i = 1; i < count; i++) {
            CGContextAddLineToPoint(m_draw->getContext(), pxs[i].x, pxs[i].y);
        }
        CGContextClosePath(m_draw->getContext());
        
        CGContextDrawPath(m_draw->getContext(), usepen && usebrush ? kCGPathFillStroke
                          : usepen ? kCGPathStroke : kCGPathFill);
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

    bool usepen = m_draw->setPen(ctx);
    bool usebrush = m_draw->setBrush(ctx);
    bool ret = count > 1 && (usepen || usebrush);
    
    if (ret) {
        CGContextDrawPath(m_draw->getContext(), usepen && usebrush ? kCGPathFillStroke
                          : usepen ? kCGPathStroke : kCGPathFill);
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
    bool usepen = m_draw->setPen(ctx);
    bool usebrush = fill && m_draw->setBrush(ctx);
    
    if (usepen || usebrush) {
        CGContextDrawPath(m_draw->getContext(), usepen && usebrush ? kCGPathFillStroke
                          : usepen ? kCGPathStroke : kCGPathFill);
    }

    return usepen || usebrush;
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
