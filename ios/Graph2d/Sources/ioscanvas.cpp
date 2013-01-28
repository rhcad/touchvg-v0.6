// ioscanvas.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "ioscanvas.h"
#include <gigraph.h>
#include <CoreGraphics/CGBitmapContext.h>

static const CGFloat patDash[]      = { 5, 5 };
static const CGFloat patDot[]       = { 1, 2 };
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
    volatile long   _cacheserr[2];      //!< 后备缓冲图无效标志
    static float    _dpi;               //!< 屏幕每英寸的点数
    static float    _scale;             //!< UIKit屏幕放大倍数

    GiCanvasIosImpl(GiCanvasIos* p) : _gs(p), _context(NULL), _buffctx(NULL)
        , _bkcolor(GiColor::White()), _fast(false)
    {
        _caches[0] = NULL;
        _caches[1] = NULL;
        _cacheserr[0] = 0;
        _cacheserr[1] = 0;
    }
    
    ~GiCanvasIosImpl()
    {
        if (_caches[0]) {
            CGImageRelease(_caches[0]);
        }
        if (_caches[1]) {
            CGImageRelease(_caches[1]);
        }
    }

    const GiGraphics* gs() const
    {
        return _gs->gs();
    }
    
    const GiTransform& xf() const
    {
        return gs()->xf();
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
                _gictx.setLineWidth(ctx->getLineWidth(), ctx->isAutoScale());
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
            if (gs())
                color = gs()->calcPenColor(color);
            CGContextSetRGBStrokeColor(getContext(), 
                                       toFloat(color.r), toFloat(color.g),
                                       toFloat(color.b), toFloat(color.a));
            
            float w = ctx->getLineWidth();
            w = gs() ? gs()->calcPenWidth(w, ctx->isAutoScale()) : (w < 0 ? -w : 1);
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
                CGContextSetLineCap(getContext(), style > 0 ? kCGLineCapButt : kCGLineCapRound);
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
            if (gs())
                color = gs()->calcPenColor(color);
            CGContextSetRGBFillColor(getContext(), 
                                     toFloat(color.r), toFloat(color.g),
                                     toFloat(color.b), toFloat(color.a));
        }

        return ctx->hasFillColor();
    }
    
    bool createBufferBitmap(float width, float height, float scale);
};

float GiCanvasIosImpl::_dpi = 132;
float GiCanvasIosImpl::_scale = 1;

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

GiCanvasIos::GiCanvasIos(GiGraphics* gs, float dpi)
{
    if (gs) {
        gs->_setCanvas(this);
        gs->_xf().setResolution(dpi > 0.1f ? dpi : GiCanvasIos::getScreenDpi());
    }
    m_draw = new GiCanvasIosImpl(this);
}

GiCanvasIos::~GiCanvasIos()
{
    delete m_draw;
}

bool GiCanvasIos::beginPaint(CGContextRef context, bool fast, bool buffered)
{
    if (m_draw->getContext() || !context)
        return false;

    if (gs()) {
        CGRect rc = CGContextGetClipBoundingBox(context);
        RECT_2D clipBox = { rc.origin.x, rc.origin.y, 
            rc.origin.x + rc.size.width, rc.origin.y + rc.size.height };
        owner()->_beginPaint(clipBox);
    }

    m_draw->_context = context;
    m_draw->_fast = fast;
    m_draw->_ctxused[0] = false;
    m_draw->_ctxused[1] = false;
    
    if (buffered && gs()) {
        m_draw->createBufferBitmap(m_draw->width(), m_draw->height(), m_draw->_scale);
        context = m_draw->getContext();
    }

    bool antiAlias = !fast && (!gs() || gs()->isAntiAliasMode());
    CGContextSetAllowsAntialiasing(context, antiAlias);
    CGContextSetShouldAntialias(context, antiAlias);
    CGContextSetFlatness(context, fast ? 20 : 1);

    CGContextSetLineCap(context, kCGLineCapRound);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    if (owner())        // 设置最小线宽为0.5像素，使用屏幕放大倍数以便得到实际像素值
        owner()->setMaxPenWidth(-1, 0.5f / m_draw->_scale);

    return true;
}

bool GiCanvasIos::beginPaintBuffered(bool fast, bool autoscale)
{
    if (m_draw->getContext() || !gs()
        || !m_draw->createBufferBitmap(m_draw->width(), m_draw->height(),
                                       autoscale ? m_draw->_scale : 1.f)) {
        return false;
    }
    
    RECT_2D clipBox = { 0, 0, m_draw->width(), m_draw->height() };
    owner()->_beginPaint(clipBox);
    
    m_draw->_fast = fast;
    m_draw->_ctxused[0] = false;
    m_draw->_ctxused[1] = false;
    
    CGContextRef context = m_draw->getContext();
    bool antiAlias = !fast && gs()->isAntiAliasMode();
    
    CGContextSetAllowsAntialiasing(context, antiAlias);
    CGContextSetShouldAntialias(context, antiAlias);
    CGContextSetFlatness(context, fast ? 20 : 1);
    
    CGContextSetLineCap(context, kCGLineCapRound);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    owner()->setMaxPenWidth(-1, 0.5f / m_draw->_scale);
    
    return true;
}

void GiCanvasIos::endPaint(bool draw)
{
    if (m_draw->getContext())
    {
        if (draw && m_draw->_buffctx && m_draw->_context) {
            CGContextRef context = m_draw->_context;
            CGImageRef image = CGBitmapContextCreateImage(m_draw->_buffctx);
            CGRect rect = CGRectMake(0, 0, m_draw->width(), m_draw->height()); // 逻辑宽高点数
            
            if (image) {
                CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
                CGContextConcatCTM(context, af);    // 图像是朝上的，上下文坐标系朝下，上下颠倒显示
                
                CGInterpolationQuality old = CGContextGetInterpolationQuality(context);
                CGContextSetInterpolationQuality(context, kCGInterpolationNone);
                CGContextDrawImage(context, rect, image);
                CGContextSetInterpolationQuality(context, old);
                
                CGContextConcatCTM(context, CGAffineTransformInvert(af));   // 恢复成坐标系朝下
                CGImageRelease(image);
            }
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

CGContextRef GiCanvasIos::getCGContext()
{
    return m_draw->getContext();
}

CGImageRef GiCanvasIos::cachedBitmap(bool invert)
{
    CGImageRef image = m_draw->_caches[0];
    if (!image || !invert)
        return image;                       // 调用者不能释放图像
    
    size_t w = CGImageGetWidth(image);      // 图像宽度，像素单位，不是点单位
    size_t h = CGImageGetHeight(image);
    CGImageRef newimg = NULL;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(NULL, w, h, 8, w * 4,
                                                 colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    if (context) {
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, h);
        CGContextConcatCTM(context, af);    // 图像是朝上的，上下文坐标系朝下，上下颠倒显示
        CGContextDrawImage(context, CGRectMake(0, 0, w, h), image);
        CGContextConcatCTM(context, CGAffineTransformInvert(af));
    
        newimg = CGBitmapContextCreateImage(context);   // 得到上下颠倒的新图像
        CGContextRelease(context);
    }
    
    return newimg;                          // 由调用者释放图像, CGImageRelease
}

bool GiCanvasIosImpl::createBufferBitmap(float width, float height, float scale)
{
    width  *= scale;                       // 点数宽度转为像素宽度
    height *= scale;
    
    if (width < 4 || height < 4 || width > 2049 || height > 2049) {
        return false;
    }
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    _buffctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4,
                                     colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    CGContextClearRect(_buffctx, CGRectMake(0, 0, width, height));
    
    // 坐标系改为Y朝下，原点在左上角，这样除了放大倍数为1外，其余就与 _context 坐标系一致
    //if (_buffctx && _context) {
        CGContextTranslateCTM(_buffctx, 0, height);
        CGContextScaleCTM(_buffctx, scale, - scale);
    //}
    //else if (_buffctx) {
    //    CGContextScaleCTM(_buffctx, scale, scale);
    //}
    
    return !!_buffctx;
}

void GiCanvasIos::clearWindow()
{
    if (gs() && m_draw->getContext()) {
        CGContextClearRect(m_draw->getContext(), 
                           CGRectMake(0, 0, m_draw->width(), m_draw->height()));
    }
}

bool GiCanvasIos::drawCachedBitmap(float x, float y, bool secondBmp)
{
    int index = secondBmp ? 1 : 0;
    CGImageRef image = m_draw->_cacheserr[index] ? NULL : m_draw->_caches[index];
    CGContextRef context = m_draw->getContext();
    bool ret = false;
    
    if (context && image) {
        CGRect rect = CGRectMake(x, y, m_draw->width(), m_draw->height());
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
        
        CGContextConcatCTM(context, af);    // 图像是朝上的，上下文坐标系朝下，上下颠倒显示
        
        CGInterpolationQuality oldQuality = CGContextGetInterpolationQuality(context);
        CGContextSetInterpolationQuality(context, kCGInterpolationNone);
        CGContextDrawImage(context, rect, image);
        CGContextSetInterpolationQuality(context, oldQuality);
        
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
        int index = secondBmp ? 1 : 0;
        CGImageRef image = gs->m_draw->_cacheserr[index] ? NULL : gs->m_draw->_caches[index];
        CGContextRef context = m_draw->getContext();
        
        if (context && image) {
            CGRect rect = CGRectMake(x, y, m_draw->width(), m_draw->height());
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
            
            CGContextConcatCTM(context, af);
            
            CGInterpolationQuality oldQuality = CGContextGetInterpolationQuality(context);
            CGContextSetInterpolationQuality(context, kCGInterpolationNone);
            CGContextDrawImage(context, rect, image);
            CGContextSetInterpolationQuality(context, oldQuality);
            
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
    m_draw->_cacheserr[n] = 0;
    if (gs() && m_draw->getContext()
        && m_draw->xf().getWidth() < 1025
        && m_draw->xf().getHeight() < 1025) {
        m_draw->_caches[n] = CGBitmapContextCreateImage(m_draw->getContext());
    }
}

bool GiCanvasIos::hasCachedBitmap(bool secondBmp) const
{
    int n = secondBmp ? 1 : 0;
    return !m_draw->_cacheserr[n] && m_draw->_caches[n];
}

void GiCanvasIos::clearCachedBitmap(bool clearAll)
{
    giInterlockedIncrement(&m_draw->_cacheserr[0]);
    if (clearAll) {
        giInterlockedIncrement(&m_draw->_cacheserr[1]);
    }
}

bool GiCanvasIos::drawImage(CGImageRef image, float scale,
                            const Point2d& centerM, bool autoScale)
{
    CGContextRef context = m_draw->getContext();
    bool ret = false;
    
    if (context && image) {
        Point2d ptD = centerM * m_draw->xf().modelToDisplay();
        float w = CGImageGetWidth(image) / scale;
        float h = CGImageGetHeight(image) / scale;
        
        if (autoScale) {
            w *= m_draw->xf().getViewScale();
            h *= m_draw->xf().getViewScale();
        }
        
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
        af = CGAffineTransformTranslate(af, ptD.x - w * 0.5f, 
                                        m_draw->height() - (ptD.y + h * 0.5f));
        
        CGContextConcatCTM(context, af);
        CGContextDrawImage(context, CGRectMake(0, 0, w, h), image);
        CGContextConcatCTM(context, CGAffineTransformInvert(af));
        ret = true;
    }
    
    return ret;
}

bool GiCanvasIos::drawImage(CGImageRef image, const Box2d& rectM)
{
    CGContextRef context = m_draw->getContext();
    bool ret = false;
    
    if (context && image) {
        Point2d ptD = rectM.center() * m_draw->xf().modelToDisplay();
        Box2d rect = rectM * m_draw->xf().modelToDisplay();
        
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, 0, m_draw->height());
        af = CGAffineTransformTranslate(af, ptD.x - rect.width() * 0.5f, 
                                        m_draw->height() - (ptD.y + rect.height() * 0.5f));
        
        CGContextConcatCTM(context, af);
        CGContextDrawImage(context, CGRectMake(0, 0, rect.width(), rect.height()), image);
        CGContextConcatCTM(context, CGAffineTransformInvert(af));
        ret = true;
    }
    
    return ret;
}

bool GiCanvasIos::isBufferedDrawing() const
{
    return !!m_draw->_buffctx;
}

void GiCanvasIos::setScreenDpi(float dpi, float scale)
{
    GiCanvasIosImpl::_dpi = dpi;
    GiCanvasIosImpl::_scale = scale;
}

float GiCanvasIos::getScreenDpi() const
{
    return GiCanvasIosImpl::_dpi;
}

void GiCanvasIos::_clipBoxChanged(const RECT_2D& clipBox)
{
    if (m_draw->getContext())
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
    if (m_draw->getContext()) {
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

bool GiCanvasIos::rawBezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    CGContextAddCurveToPoint(m_draw->getContext(), c1x, c1y, c2x, c2y, x, y);
    return true;
}

bool GiCanvasIos::rawClosePath()
{
    CGContextClosePath(m_draw->getContext());
    return true;
}
