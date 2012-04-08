// gidrgdi.cpp: 实现用GDI实现的图形系统类GiGraphGdi
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw
#ifdef _WIN32
#include "gidrgdi.h"
#include <_gigraph.h>
#include <gdiobj.h>

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
    GiGraphGdi*     m_this;             //!< 拥有者
    HDC             m_hdc;              //!< 显示DC

    GdiDrawImplBase(GiGraphGdi* pThis)
        : m_this(pThis), m_hdc(NULL)
    {
    }
};

//! 管理后备缓冲位图的辅助类
class CachedBmp
{
public:
    CachedBmp() : m_cachedBmp(NULL)
    {
    }

    ~CachedBmp()
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
        HDC destDC, int x = 0, int y = 0) const;
    bool saveCachedBitmap(const GdiDrawImplBase* pdraw, HDC hSrcDC);

private:
    HBITMAP m_cachedBmp;
};

//! GiGraphGdi的内部实现类
class GiGraphGdi::DrawImpl : public GdiDrawImplBase
{
public:
    GiContext       m_context;          //!< 当前绘图参数
    HGDIOBJ         m_pen;              //!< 当前绘图参数对应的画笔
    HGDIOBJ         m_brush;            //!< 当前绘图参数对应的画刷

    HDC             m_buffDC;           //!< 缓冲DC
    HBITMAP         m_buffBmp;          //!< 缓冲位图
    HGDIOBJ         m_buffOldBmp;       //!< 缓冲DC中原来的位图
    CachedBmp       m_cachedBmp[2];     //!< 后备缓冲位图

    DrawImpl(GiGraphGdi* pThis) : GdiDrawImplBase(pThis)
    {
        m_pen = NULL;
        m_brush = NULL;
        m_buffDC = NULL;
        m_buffBmp = NULL;
        m_buffOldBmp = NULL;
    }

    ~DrawImpl()
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
                UInt16 width = m_this->calcPenWidth(ctx->getLineWidth());
                GiColor color = m_this->calcPenColor(ctx->getLineColor());
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
                GiColor color = m_this->calcPenColor(ctx->getFillColor());
                m_brush = ::CreateSolidBrush(RGB(color.r, color.g, color.b));
            }
        }

        return m_brush;
    }
};

GiGraphGdi::GiGraphGdi(GiTransform* xform) : GiGraphWin(xform)
{
    m_draw = new DrawImpl(this);
}

GiGraphGdi::GiGraphGdi(const GiGraphGdi& src) : GiGraphWin(src)
{
    m_draw = new DrawImpl(this);
    operator=(src);
}

GiGraphGdi::~GiGraphGdi()
{
    delete m_draw;
}

GiGraphGdi& GiGraphGdi::operator=(const GiGraphGdi& src)
{
    GiGraphWin::operator=(src);
    if (this != &src)
    {
    }

    return *this;
}

bool GiGraphGdi::isBufferedDrawing() const
{
    return m_draw->m_buffDC != m_draw->m_hdc;
}

HDC GiGraphGdi::acquireDC()
{
    return m_draw->getDrawDC();
}

void GiGraphGdi::releaseDC(HDC)
{
}

bool GiGraphGdi::beginPaint(HDC hdc, HDC attribDC, bool buffered, bool overlay)
{
    bool ret = (NULL == m_draw->m_hdc)
        && GiGraphWin::beginPaint(hdc, attribDC, buffered, overlay);
    if (!ret)
        return false;

    buffered = buffered && !m_impl->isPrint;
    m_draw->m_hdc = hdc;

    CachedBmp oldDrawing;
    if (buffered && overlay)
        oldDrawing.saveCachedBitmap(m_draw, hdc);

    if (buffered)
    {
        if ((m_draw->m_buffDC = ::CreateCompatibleDC(hdc)) != NULL)
        {
            m_draw->m_buffBmp = ::CreateCompatibleBitmap(hdc, 
                m_impl->clipBox0.right - m_impl->clipBox0.left, 
                m_impl->clipBox0.bottom - m_impl->clipBox0.top);
            if (m_draw->m_buffBmp == NULL)
            {
                ::DeleteDC(m_draw->m_buffDC);
                m_draw->m_buffDC = NULL;
            }
            else
            {
                ::OffsetViewportOrgEx(m_draw->m_buffDC,
                    -m_impl->clipBox0.left, -m_impl->clipBox0.top, NULL);
                m_draw->m_buffOldBmp = ::SelectObject(
                    m_draw->m_buffDC, m_draw->m_buffBmp);
                ::SetBrushOrgEx(m_draw->m_buffDC, 
                    m_impl->clipBox0.left % 8,
                    m_impl->clipBox0.top % 8, NULL);
                ::IntersectClipRect(m_draw->m_buffDC,
                    m_impl->clipBox0.left, m_impl->clipBox0.top,
                    m_impl->clipBox0.right, m_impl->clipBox0.bottom);

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

void GiGraphGdi::clearWnd()
{
    if (!m_impl->isPrint && isDrawing())
    {
        ::ExtTextOut(m_draw->getDrawDC(), 0, 0, ETO_OPAQUE, 
            &m_impl->clipBox0, NULL, 0, NULL);
    }
}

#define CachedBmp(secondBmp)   \
    m_draw->m_cachedBmp[secondBmp ? 1 : 0]

void GiGraphGdi::clearCachedBitmap()
{
    m_draw->m_cachedBmp[0].clear();
    m_draw->m_cachedBmp[1].clear();
}

bool GiGraphGdi::drawCachedBitmap(int x, int y, bool secondBmp)
{
    return CachedBmp(secondBmp).draw(m_draw, m_draw->getDrawDC(), x, y);
}

bool GiGraphGdi::drawCachedBitmap2(const GiGraphics* p, bool secondBmp)
{
    const GiGraphGdi* gs = static_cast<const GiGraphGdi*>(p);
    return p != NULL
        && p->xf().getWidth() == xf().getWidth()
        && p->xf().getHeight() == xf().getHeight()
        && gs->CachedBmp(secondBmp).draw(m_draw, m_draw->getDrawDC());
}

bool CachedBmp::draw(const GdiDrawImplBase* pdraw, HDC destDC, int x, int y) const
{
    bool ret = false;

    if (m_cachedBmp != NULL && destDC != NULL)
    {
        GiCompatibleDC memDC (pdraw->m_hdc);
        if (memDC != NULL)
        {
            HGDIOBJ oldBmp = ::SelectObject(memDC, m_cachedBmp);
            ret = ::BitBlt(destDC, x, y, 
                pdraw->m_this->xf().getWidth(), 
                pdraw->m_this->xf().getHeight(),
                memDC, 0, 0, SRCCOPY) != FALSE;
            ::SelectObject(memDC, oldBmp);
        }
    }

    return ret;
}

bool CachedBmp::saveCachedBitmap(const GdiDrawImplBase* pdraw, HDC hSrcDC)
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

void GiGraphGdi::saveCachedBitmap(bool secondBmp)
{
    CachedBmp(secondBmp).saveCachedBitmap(m_draw, m_draw->getDrawDC());
}

bool GiGraphGdi::hasCachedBitmap(bool secondBmp) const
{
    return CachedBmp(secondBmp) != NULL;
}

void GiGraphGdi::endPaint(bool draw)
{
    if (isDrawing())
    {
        if (m_draw->m_buffBmp != NULL && draw)
        {
            ::BitBlt(m_draw->m_hdc, 
                m_impl->clipBox0.left, m_impl->clipBox0.top,
                m_impl->clipBox0.right - m_impl->clipBox0.left,
                m_impl->clipBox0.bottom - m_impl->clipBox0.top,
                m_draw->m_buffDC, 
                m_impl->clipBox0.left, m_impl->clipBox0.top, SRCCOPY);
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

        GiGraphWin::endPaint(draw);
    }
}

bool GiGraphGdi::setClipBox(const RECT* prc)
{
    bool ret = GiGraphWin::setClipBox(prc);
    if (ret)
    {
        HRGN hRgn = ::CreateRectRgn(
            m_impl->clipBox.left, m_impl->clipBox.top, 
            m_impl->clipBox.right, m_impl->clipBox.bottom);
        if (hRgn != NULL)
        {
            ::SelectClipRgn(m_draw->getDrawDC(), hRgn);
            ::DeleteObject(hRgn);
        }
    }

    return ret;
}

bool GiGraphGdi::setClipWorld(const Box2d& rectWorld)
{
    bool ret = GiGraphWin::setClipWorld(rectWorld);
    if (ret)
    {
        HRGN hRgn = ::CreateRectRgn(
            m_impl->clipBox.left, m_impl->clipBox.top, 
            m_impl->clipBox.right, m_impl->clipBox.bottom);
        if (hRgn != NULL)
        {
            ::SelectClipRgn(m_draw->getDrawDC(), hRgn);
            ::DeleteObject(hRgn);
        }
    }

    return ret;
}

GiColor GiGraphGdi::getBkColor() const
{
    GiColor bkColor(GiColor::White());
    if (m_draw->getDrawDC() != NULL)
    {
        COLORREF cr = ::GetBkColor(m_draw->getDrawDC());
        bkColor.set(GetRValue(cr), GetGValue(cr), GetBValue(cr));
    }
    return bkColor;
}

GiColor GiGraphGdi::setBkColor(const GiColor& color)
{
    if (m_draw->getDrawDC() != NULL)
    {
        COLORREF cr = RGB(color.r, color.g, color.b);
        cr = ::SetBkColor(m_draw->getDrawDC(), cr);
        return GiColor(GetRValue(cr), GetGValue(cr), GetBValue(cr));
    }
    return color;
}

GiColor GiGraphGdi::getNearestColor(const GiColor& color) const
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

const GiContext* GiGraphGdi::getCurrentContext() const
{
    return &m_draw->m_context;
}

bool GiGraphGdi::rawLine(const GiContext* ctx, 
                         int x1, int y1, int x2, int y2)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);

    BOOL ret = ::MoveToEx(hdc, x1, y1, NULL);
    ret = ret && ::LineTo(hdc, x2, y2);

    return ret ? true : false;
}

bool GiGraphGdi::rawPolyline(const GiContext* ctx, 
                             const POINT* lppt, int count)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    return ::Polyline(hdc, lppt, count) ? true : false;
}

bool GiGraphGdi::rawPolyBezier(const GiContext* ctx, 
                               const POINT* lppt, int count)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    return ::PolyBezier(hdc, lppt, count) ? true : false;
}

bool GiGraphGdi::rawPolygon(const GiContext* ctx, 
                            const POINT* lppt, int count)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    return ::Polygon(hdc, lppt, count) ? true : false;
}

bool GiGraphGdi::rawRect(const GiContext* ctx, 
                         int x, int y, int w, int h)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    return ::Rectangle(hdc, x, y, x + w, y + h) ? true : false;
}

bool GiGraphGdi::rawEllipse(const GiContext* ctx, 
                            int x, int y, int w, int h)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    return ::Ellipse(hdc, x, y, x + w, y + h) ? true : false;
}

bool GiGraphGdi::rawBeginPath()
{
    return ::BeginPath(m_draw->getDrawDC()) ? true : false;
}

bool GiGraphGdi::rawEndPath(const GiContext* ctx, bool fill)
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

bool GiGraphGdi::rawMoveTo(int x, int y)
{
    return ::MoveToEx(m_draw->getDrawDC(), x, y, NULL) ? true : false;
}

bool GiGraphGdi::rawLineTo(int x, int y)
{
    return ::LineTo(m_draw->getDrawDC(), x, y) ? true : false;
}

bool GiGraphGdi::rawPolyBezierTo(const POINT* lppt, int count)
{
    return ::PolyBezierTo(m_draw->getDrawDC(), lppt, count) ? true : false;
}

bool GiGraphGdi::rawCloseFigure()
{
    return ::CloseFigure(m_draw->getDrawDC()) ? true : false;
}

static BOOL PolyDraw98(HDC hdc, const POINT *lppt, const BYTE *types, int n)
{
    for (int i = 0; i < n; i++)
    {
        switch (types[i] & ~PT_CLOSEFIGURE)
        {
        case PT_MOVETO:
            ::MoveToEx(hdc, lppt[i].x, lppt[i].y, NULL);
            break;

        case PT_LINETO:
            ::LineTo(hdc, lppt[i].x, lppt[i].y);
            break;

        case PT_BEZIERTO:
            if (i + 2 >= n)
                return FALSE;
            ::PolyBezierTo(hdc, &lppt[i], 3);
            i += 2;
            break;

        default:
            return FALSE;
        }
        if (types[i] & PT_CLOSEFIGURE)
            ::CloseFigure(hdc);
    }

    return TRUE;
}

bool GiGraphGdi::rawPolyDraw(const GiContext* ctx, int count, 
                             const POINT* lppt, const UInt8* types)
{
    HDC hdc = m_draw->getDrawDC();
    KGDIObject pen (hdc, m_draw->createPen(ctx), false);
    KGDIObject brush (hdc, m_draw->createBrush(ctx), false);
    BOOL ret = FALSE;

    if (NULL == ctx)
        ctx = getCurrentContext();

    if (giIsNT())
    {
        if (ctx->hasFillColor())
        {
            ret = ::BeginPath(hdc);
            if (ret)
            {
                ret = ::PolyDraw(hdc, lppt, types, count);
                ret = ::EndPath(hdc);
                ret = ::StrokeAndFillPath(hdc);
            }
        }
        else
        {
            ret = ::PolyDraw(hdc, lppt, types, count);
        }
    }
    else
    {
        ret = ::BeginPath(hdc);
        if (ret)
        {
            ret = PolyDraw98(hdc, lppt, types, count);
            ret = ::EndPath(hdc);
            if (ctx->hasFillColor())
                ret = ::StrokeAndFillPath(hdc);
            else
                ret = ::StrokePath(hdc);
        }
    }

    return ret ? true : false;
}

bool GiGraphGdi::drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
                           const Box2d& rectW, bool fast)
{
    BOOL ret = FALSE;
    HDC hdc = m_draw->getDrawDC();

    if (hdc != NULL && hmWidth > 0 && hmHeight > 0 && hbitmap != NULL
        && m_impl->rectDrawW.isIntersect(Box2d(rectW, true)))
    {
        RECT rc, rcDraw, rcFrom;

        // rc: 整个图像对应的显示坐标区域
        (rectW * xf().worldToDisplay()).get(rc.left, rc.top, rc.right, rc.bottom);

        // rcDraw: 图像经剪裁后的可显示部分
        if (!IntersectRect(&rcDraw, &rc, &m_impl->clipBox))
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

        int nBltMode = (!fast || m_impl->isPrint) ? HALFTONE : COLORONCOLOR;
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

#endif //_WIN32
