// canvasgdi.cpp: 实现用GDI实现的图形显示接口类 GiCanvasGdi
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "canvasgdi.h"
#include <gigraph.h>
#include <gdiobj.h>
#include <vector>

// 判断是否是Windows NT4/2000及以后
static bool giIsNT()
{
    static int flag = 0;
    if (flag == 1)
        return true;
    else if (flag == -1)
        return false;

    OSVERSIONINFO ver;
    ZeroMemory(&ver, sizeof(ver));
    ver.dwOSVersionInfoSize = sizeof(ver);
    ::GetVersionEx(&ver);
    if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT
        && ver.dwMajorVersion >= 4)
    {
        flag = 1;
        return true;
    }
    else
    {
        flag = -1;
        return false;
    }
}

//! DrawImpl类的基本数据
struct GdiDrawImplBase
{
    GiCanvasGdi*    m_this;             //!< 拥有者
    HDC             m_hdc;              //!< 显示DC

    GdiDrawImplBase(GiCanvasGdi* gs)
        : m_this(gs), m_hdc(NULL)
    {
    }
};

//! 管理后备缓冲位图的辅助类
class GdiCachedBmp
{
public:
    GdiCachedBmp() : m_cachedBmp(NULL)
    {
    }

    ~GdiCachedBmp()
    {
        clear();
    }

    operator HBITMAP() const
    {
        return m_cachedBmp;
    }

    void clear()
    {
        if (m_cachedBmp != NULL)
        {
            ::DeleteObject(m_cachedBmp);
            m_cachedBmp = NULL;
        }
    }

    bool draw(const GdiDrawImplBase* pdraw, 
        HDC destDC, float x = 0, float y = 0) const;
    bool saveCachedBitmap(const GdiDrawImplBase* pdraw, HDC hSrcDC);

private:
    HBITMAP m_cachedBmp;
};

//! GiCanvasGdi的内部实现类
class GiCanvasGdiImpl : public GdiDrawImplBase
{
public:
    GiContext       m_context;          //!< 当前绘图参数
    HGDIOBJ         m_pen;              //!< 当前绘图参数对应的画笔
    HGDIOBJ         m_brush;            //!< 当前绘图参数对应的画刷

    HDC             m_buffDC;           //!< 缓冲DC
    HBITMAP         m_buffBmp;          //!< 缓冲位图
    HGDIOBJ         m_buffOldBmp;       //!< 缓冲DC中原来的位图
    GdiCachedBmp    m_cachedBmp[2];     //!< 后备缓冲位图

    GiCanvasGdiImpl(GiCanvasGdi* gs) : GdiDrawImplBase(gs)
    {
        m_pen = NULL;
        m_brush = NULL;
        m_buffDC = NULL;
        m_buffBmp = NULL;
        m_buffOldBmp = NULL;
    }

    ~GiCanvasGdiImpl()
    {
    }

    HDC getDrawDC() const
    {
        return m_buffDC != NULL ? m_buffDC : m_hdc;
    }

    HGDIOBJ createPen(const GiContext* ctx)
    {
        if (ctx == NULL)
            ctx = &m_context;

        if (m_pen == NULL
            || m_context.getLineStyle() != ctx->getLineStyle()
            || m_context.getLineWidth() != ctx->getLineWidth()
            || m_context.getLineColor() != ctx->getLineColor()
            || m_context.getLineAlpha() != ctx->getLineAlpha())
        {
            m_context.setLineStyle(ctx->getLineStyle());
            m_context.setLineWidth(ctx->getLineWidth());
            m_context.setLineColor(ctx->getLineColor());
            m_context.setLineAlpha(ctx->getLineAlpha());

            if (m_pen != NULL)
                ::DeleteObject(m_pen);

            if (ctx->isNullLine() || ctx->getLineAlpha() < 127)
                m_pen = ::GetStockObject(NULL_PEN);
            else
            {
                int width = mgRound(m_this->owner()->calcPenWidth(ctx->getLineWidth()));
                GiColor color = m_this->owner()->calcPenColor(ctx->getLineColor());
                COLORREF cr = RGB(color.r, color.g, color.b);
                int lineStyle = ctx->getLineStyle();

                if (width > 1)
                {
                    LOGBRUSH logBrush = { BS_SOLID, cr };
                    m_pen = ::ExtCreatePen(
                        PS_GEOMETRIC | PS_ENDCAP_FLAT | lineStyle, 
                        width, &logBrush, 0, NULL);
                }
                else
                {
                    m_pen = ::CreatePen(lineStyle, width, cr);
                }
            }
        }

        return m_pen;
    }

    HGDIOBJ createBrush(const GiContext* ctx)
    {
        if (ctx == NULL)
            ctx = &m_context;

        if (m_brush == NULL
            || m_context.getFillColor() != ctx->getFillColor()
            || m_context.getFillAlpha() != ctx->getFillAlpha())
        {
            m_context.setFillColor(ctx->getFillColor());
            m_context.setFillAlpha(ctx->getFillAlpha());

            if (m_brush != NULL)
                ::DeleteObject(m_brush);

            if (!ctx->hasFillColor() || ctx->getFillAlpha() < 127)
                m_brush = ::GetStockObject(NULL_BRUSH);
            else
            {
                GiColor color = m_this->owner()->calcPenColor(ctx->getFillColor());
                m_brush = ::CreateSolidBrush(RGB(color.r, color.g, color.b));
            }
        }

        return m_brush;
    }
};

GiCanvasGdi::GiCanvasGdi(GiGraphics* gs) : GiCanvasWin(gs)
{
    m_draw = new GiCanvasGdiImpl(this);
}

GiCanvasGdi::~GiCanvasGdi()
{
    delete m_draw;
}

bool GiCanvasGdi::isBufferedDrawing() const
{
    return m_draw->m_buffDC != m_draw->m_hdc;
}

HDC GiCanvasGdi::acquireDC()
{
    return m_draw->getDrawDC();
}

void GiCanvasGdi::releaseDC(HDC)
{
}

bool GiCanvasGdi::beginPaint(HDC hdc, HDC attribDC, bool buffered, bool overlay)
{
    bool ret = (NULL == m_draw->m_hdc)
        && GiCanvasWin::beginPaint(hdc, attribDC, buffered, overlay);
    if (!ret)
        return false;

    buffered = buffered && !m_owner->isPrint();
    m_draw->m_hdc = hdc;

    GdiCachedBmp oldDrawing;
    if (buffered && overlay)
        oldDrawing.saveCachedBitmap(m_draw, hdc);

    if (buffered)
    {
        if ((m_draw->m_buffDC = ::CreateCompatibleDC(hdc)) != NULL)
        {
            RECT rc;
            RECT2D clipBox;

            Box2d(owner()->getClipBox(clipBox)).get(rc);

            m_draw->m_buffBmp = ::CreateCompatibleBitmap(hdc, 
                rc.right - rc.left, rc.bottom - rc.top);
            if (m_draw->m_buffBmp == NULL)
            {
                ::DeleteDC(m_draw->m_buffDC);
                m_draw->m_buffDC = NULL;
            }
            else
            {
                ::OffsetViewportOrgEx(m_draw->m_buffDC, -rc.left, -rc.top, NULL);
                m_draw->m_buffOldBmp = ::SelectObject(
                    m_draw->m_buffDC, m_draw->m_buffBmp);
                ::SetBrushOrgEx(m_draw->m_buffDC, rc.left % 8, rc.top % 8, NULL);
                ::IntersectClipRect(m_draw->m_buffDC,
                    rc.left, rc.top, rc.right, rc.bottom);

                ::SetBkColor(m_draw->m_buffDC, ::GetBkColor(hdc));
            }
        }
        oldDrawing.draw(m_draw, m_draw->m_buffDC);
    }

    ::SetBkMode(m_draw->getDrawDC(), TRANSPARENT);

    // 设置折线斜接连接的最小夹角为60度
    ::SetMiterLimit(m_draw->getDrawDC(), 
        static_cast<float>(1.0 / sin(_M_PI_6)), NULL);

    return ret;
}

void GiCanvasGdi::clearWindow()
{
    if (!m_owner->isPrint() && m_owner->isDrawing())
    {
        RECT rc;
        RECT2D clipBox;

        Box2d(owner()->getClipBox(clipBox)).get(rc);
        ExtTextOut(m_draw->getDrawDC(), 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }
}

#define GdiCachedBmp(secondBmp)   \
    m_draw->m_cachedBmp[secondBmp ? 1 : 0]

void GiCanvasGdi::clearCachedBitmap()
{
    m_draw->m_cachedBmp[0].clear();
    m_draw->m_cachedBmp[1].clear();
}

bool GiCanvasGdi::drawCachedBitmap(float x, float y, bool secondBmp)
{
    return GdiCachedBmp(secondBmp).draw(m_draw, m_draw->getDrawDC(), x, y);
}

bool GiCanvasGdi::drawCachedBitmap2(const GiCanvas* p, float x, float y, bool secondBmp)
{
    if (p && p->getCanvasType() == getCanvasType())
    {
        const GiCanvasGdi* gs = static_cast<const GiCanvasGdi*>(p);
        return gs->xf().getWidth() == xf().getWidth()
            && gs->xf().getHeight() == xf().getHeight()
            && GdiCachedBmp(secondBmp).draw(m_draw, m_draw->getDrawDC(), x, y);
    }
    return false;
}

bool GdiCachedBmp::draw(const GdiDrawImplBase* pdraw, HDC destDC, float x, float y) const
{
    bool ret = false;

    if (m_cachedBmp != NULL && destDC != NULL)
    {
        GiCompatibleDC memDC (pdraw->m_hdc);
        if (memDC != NULL)
        {
            HGDIOBJ oldBmp = ::SelectObject(memDC, m_cachedBmp);
            ret = ::BitBlt(destDC, mgRound(x), mgRound(y), 
                pdraw->m_this->xf().getWidth(), 
                pdraw->m_this->xf().getHeight(),
                memDC, 0, 0, SRCCOPY) != FALSE;
            ::SelectObject(memDC, oldBmp);
        }
    }

    return ret;
}

bool GdiCachedBmp::saveCachedBitmap(const GdiDrawImplBase* pdraw, HDC hSrcDC)
{
    bool ret = false;
    int width = pdraw->m_this->xf().getWidth();
    int height = pdraw->m_this->xf().getHeight();

    clear();

    if (hSrcDC != NULL)
    {
        m_cachedBmp = ::CreateCompatibleBitmap(
            pdraw->m_hdc, width, height);
        if (m_cachedBmp != NULL)
        {
            GiCompatibleDC memDC (pdraw->m_hdc);
            if (memDC != NULL)
            {
                HGDIOBJ oldBmp = ::SelectObject(memDC, m_cachedBmp);
                ret = ::BitBlt(memDC, 0, 0, width, height,
                    hSrcDC, 0, 0, SRCCOPY) != FALSE;
                ::SelectObject(memDC, oldBmp);
            }
        }
        if (!ret)
            clear();
    }

    return ret;
}

void GiCanvasGdi::saveCachedBitmap(bool secondBmp)
{
    GdiCachedBmp(secondBmp).saveCachedBitmap(m_draw, m_draw->getDrawDC());
}

bool GiCanvasGdi::hasCachedBitmap(bool secondBmp) const
{
    return GdiCachedBmp(secondBmp) != NULL;
}

void GiCanvasGdi::endPaint(bool draw)
{
    if (m_owner->isDrawing())
    {
        if (m_draw->m_buffBmp != NULL && draw)
        {
            RECT rc;
            RECT2D clipBox;

            Box2d(owner()->getClipBox(clipBox)).get(rc);
            BitBlt(m_draw->m_hdc, rc.left, rc.top, 
                rc.right - rc.left, rc.bottom - rc.top,
                m_draw->m_buffDC, rc.left, rc.top, SRCCOPY);
        }

        if (m_draw->m_pen != NULL)
        {
            ::DeleteObject(m_draw->m_pen);
            m_draw->m_pen = NULL;
        }
        if (m_draw->m_brush != NULL)
        {
            ::DeleteObject(m_draw->m_brush);
            m_draw->m_brush = NULL;
        }
        if (m_draw->m_buffBmp != NULL)
        {
            ::SelectObject(m_draw->m_buffDC, m_draw->m_buffOldBmp);
            ::DeleteObject(m_draw->m_buffBmp);
            m_draw->m_buffBmp = NULL;
            m_draw->m_buffOldBmp = NULL;
        }
        if (m_draw->m_buffDC != NULL)
        {
            ::DeleteDC(m_draw->m_buffDC);
            m_draw->m_buffDC = NULL;
        }

        m_draw->m_hdc = NULL;

        GiCanvasWin::endPaint(draw);
    }
}

void GiCanvasGdi::_clipBoxChanged(const RECT2D& clipBox)
{
    RECT rc = { mgRound(clipBox.left), mgRound(clipBox.top),
        mgRound(clipBox.right), mgRound(clipBox.bottom)
    };
    HRGN hRgn = ::CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
    if (hRgn != NULL)
    {
        ::SelectClipRgn(m_draw->getDrawDC(), hRgn);
        ::DeleteObject(hRgn);
    }
}

GiColor GiCanvasGdi::getBkColor() const
{
    GiColor bkColor(GiColor::White());
    if (m_draw->getDrawDC() != NULL)
    {
        COLORREF cr = ::GetBkColor(m_draw->getDrawDC());
        bkColor.set(GetRValue(cr), GetGValue(cr), GetBValue(cr));
    }
    return bkColor;
}

GiColor GiCanvasGdi::setBkColor(const GiColor& color)
{
    if (m_draw->getDrawDC() != NULL)
    {
        COLORREF cr = RGB(color.r, color.g, color.b);
        cr = ::SetBkColor(m_draw->getDrawDC(), cr);
        return GiColor(GetRValue(cr), GetGValue(cr), GetBValue(cr));
    }
    return color;
}

GiColor GiCanvasGdi::getNearestColor(const GiColor& color) const
{
    COLORREF cr = RGB(color.r, color.g, color.b);
    if (m_attribDC != NULL)
    {
        cr = ::GetNearestColor(m_attribDC, cr);
        return GiColor(GetRValue(cr), GetGValue(cr), GetBValue(cr), color.a);
    }
    if (m_draw->m_hdc != NULL)
    {
        cr = ::GetNearestColor(m_draw->m_hdc, cr);
        return GiColor(GetRValue(cr), GetGValue(cr), GetBValue(cr), color.a);
    }
    return color;
}

const GiContext* GiCanvasGdi::getCurrentContext() const
{
    return &m_draw->m_context;
}

bool GiCanvasGdi::rawLine(const GiContext* ctx, 
                          float x1, float y1, float x2, float y2)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);

    BOOL ret = ::MoveToEx(hdc, mgRound(x1), mgRound(y1), NULL);
    ret = ret && ::LineTo(hdc, mgRound(x2), mgRound(y2));

    return ret ? true : false;
}

bool GiCanvasGdi::rawLines(const GiContext* ctx, 
                           const Point2d* pxs, int count)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    bool ret = false;

    if (count > 0)
    {
        std::vector<POINT> pts;
        pts.resize(count);
        for (int i = 0; i < count; i++)
            pxs[i].get(pts[i].x, pts[i].y);
        ret = !!Polyline(hdc, &pts.front(), count);
    }

    return ret;
}

bool GiCanvasGdi::rawBeziers(const GiContext* ctx, 
                             const Point2d* pxs, int count)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    bool ret = false;

    if (count > 0)
    {
        std::vector<POINT> pts;
        pts.resize(count);
        for (int i = 0; i < count; i++)
            pxs[i].get(pts[i].x, pts[i].y);
        ret = !!PolyBezier(hdc, &pts.front(), count);
    }

    return ret;
}

bool GiCanvasGdi::rawPolygon(const GiContext* ctx, 
                             const Point2d* pxs, int count)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    bool ret = false;

    if (count > 0)
    {
        std::vector<POINT> pts;
        pts.resize(count);
        for (int i = 0; i < count; i++)
            pxs[i].get(pts[i].x, pts[i].y);
        ret = !!Polygon(hdc, &pts.front(), count);
    }

    return ret;
}

bool GiCanvasGdi::rawRect(const GiContext* ctx, 
                          float x, float y, float w, float h)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    return ::Rectangle(hdc, mgRound(x), mgRound(y), 
        mgRound(x + w), mgRound(y + h)) ? true : false;
}

bool GiCanvasGdi::rawEllipse(const GiContext* ctx, 
                             float x, float y, float w, float h)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    return ::Ellipse(hdc, mgRound(x), mgRound(y), 
        mgRound(x + w), mgRound(y + h)) ? true : false;
}

bool GiCanvasGdi::rawBeginPath()
{
    return ::BeginPath(m_draw->getDrawDC()) ? true : false;
}

bool GiCanvasGdi::rawEndPath(const GiContext* ctx, bool fill)
{
    HDC hdc = m_draw->getDrawDC();
    BOOL ret = ::EndPath(hdc);
    if (ret)
    {
        KGDIObject pen (hdc, m_draw->createPen(ctx), false);
        if (fill)
        {
            KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
            ret = ::StrokeAndFillPath(hdc);
        }
        else
        {
            ret = ::StrokePath(hdc);
        }
    }

    return ret ? true : false;
}

bool GiCanvasGdi::rawMoveTo(float x, float y)
{
    return ::MoveToEx(m_draw->getDrawDC(), mgRound(x), mgRound(y), NULL) ? true : false;
}

bool GiCanvasGdi::rawLineTo(float x, float y)
{
    return ::LineTo(m_draw->getDrawDC(), mgRound(x), mgRound(y)) ? true : false;
}

bool GiCanvasGdi::rawBezierTo(const Point2d* pxs, int count)
{
    bool ret = false;

    if (count > 0)
    {
        std::vector<POINT> pts;
        pts.resize(count);
        for (int i = 0; i < count; i++)
            pxs[i].get(pts[i].x, pts[i].y);
        ret = !!PolyBezierTo(m_draw->getDrawDC(), &pts.front(), count);
    }

    return ret;
}

bool GiCanvasGdi::rawClosePath()
{
    return ::CloseFigure(m_draw->getDrawDC()) ? true : false;
}

static BOOL PolyDraw98(HDC hdc, const Point2d *pxs, const BYTE *types, int n)
{
    POINT pts[3];

    for (int i = 0; i < n; i++)
    {
        switch (types[i] & ~kGiCloseFigure)
        {
        case kGiMoveTo:
            ::MoveToEx(hdc, mgRound(pxs[i].x), mgRound(pxs[i].y), NULL);
            break;

        case kGiLineTo:
            ::LineTo(hdc, mgRound(pxs[i].x), mgRound(pxs[i].y));
            break;

        case kGiBeziersTo:
            if (i + 2 >= n)
                return FALSE;
            for (int j = 0; j < 3; j++)
                pxs[i+j].get(pts[j].x, pts[j].y);
            ::PolyBezierTo(hdc, pts, 3);
            i += 2;
            break;

        default:
            return FALSE;
        }
        if (types[i] & kGiCloseFigure)
            ::CloseFigure(hdc);
    }

    return TRUE;
}

bool GiCanvasGdi::rawPath(const GiContext* ctx, int count, 
                          const Point2d* pxs, const UInt8* types)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    BOOL ret = FALSE;

    if (NULL == ctx)
        ctx = getCurrentContext();

    if (giIsNT())
    {
        std::vector<POINT> pts;

        if (ctx->hasFillColor())
        {
            ret = ::BeginPath(hdc);
            if (ret)
            {
                pts.resize(count);
                for (int i = 0; i < count; i++)
                    pxs[i].get(pts[i].x, pts[i].y);
                ret = ::PolyDraw(hdc, &pts.front(), types, count);
                ret = ::EndPath(hdc);
                ret = ::StrokeAndFillPath(hdc);
            }
        }
        else
        {
            pts.resize(count);
            for (int i = 0; i < count; i++)
                pxs[i].get(pts[i].x, pts[i].y);
            ret = ::PolyDraw(hdc, &pts.front(), types, count);
        }
    }
    else
    {
        ret = ::BeginPath(hdc);
        if (ret)
        {
            ret = PolyDraw98(hdc, pxs, types, count);
            ret = ::EndPath(hdc);
            if (ctx->hasFillColor())
                ret = ::StrokeAndFillPath(hdc);
            else
                ret = ::StrokePath(hdc);
        }
    }

    return ret ? true : false;
}

bool GiCanvasGdi::drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
                            const Box2d& rectW, bool fast)
{
    BOOL ret = FALSE;
    HDC hdc = m_draw->getDrawDC();

    if (hdc != NULL && hmWidth > 0 && hmHeight > 0 && hbitmap != NULL
        && m_owner->getClipWorld().isIntersect(Box2d(rectW, true)))
    {
        RECT2D clipBox;
        RECT rc, rcDraw, rcFrom;

        // rc: 整个图像对应的显示坐标区域
        (rectW * xf().worldToDisplay()).get(rc);

        // rcDraw: 图像经剪裁后的可显示部分
        Box2d(owner()->getClipBox(clipBox)).get(rcFrom);
        if (!IntersectRect(&rcDraw, &rc, &rcFrom))
            return false;

        long width, height;       // pixel units
        width = MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), 2540);
        height = MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), 2540);

        // rcFrom: rcDraw在原始图像上对应的图像范围
        rcFrom.left = MulDiv(rcDraw.left - rc.left, width, rc.right - rc.left);
        rcFrom.top = MulDiv(rcDraw.top - rc.top, height, rc.bottom - rc.top);
        rcFrom.right = MulDiv(rcDraw.right - rc.left, width, rc.right - rc.left);
        rcFrom.bottom = MulDiv(rcDraw.bottom - rc.top, height, rc.bottom - rc.top);

        // 根据rectW正负决定是否颠倒显示图像
        if (rectW.xmin > rectW.xmax)
            mgSwap(rcDraw.left, rcDraw.right);
        if (rectW.ymin > rectW.ymax)
            mgSwap(rcDraw.top, rcDraw.bottom);

        GiCompatibleDC memDC (hdc);
        if (memDC == NULL)
            return false;
        KGDIObject bmp (memDC, hbitmap, false);
        KGDIObject brush (hdc, ::GetStockObject(NULL_BRUSH), false);

        int nBltMode = (!fast || m_owner->isPrint()) ? HALFTONE : COLORONCOLOR;
        int nOldMode = ::SetStretchBltMode(hdc, nBltMode);
        ::SetBrushOrgEx(hdc, rcDraw.left % 8, rcDraw.top % 8, NULL);
        ret = ::StretchBlt(hdc, 
            rcDraw.left, rcDraw.top, 
            rcDraw.right - rcDraw.left, 
            rcDraw.bottom - rcDraw.top, 
            memDC, 
            rcFrom.left, rcFrom.top, 
            rcFrom.right - rcFrom.left, 
            rcFrom.bottom - rcFrom.top, 
            SRCCOPY);
        ::SetStretchBltMode(hdc, nOldMode);
    }

    return ret ? true : false;
}
