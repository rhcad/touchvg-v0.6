// gidrgdip.cpp: 实现用GDI+实现的图形显示接口类 GiGraphGdip
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "gidrgdip.h"
#include <_gigraph.h>

#define ULONG_PTR DWORD
#include <objbase.h>
#include <GdiPlus.h>
#ifdef _MSC_VER
#pragma comment(lib,"GdiPlus.lib")
#endif

#define G Gdiplus

//! DrawImpl类的基本数据
struct GdipDrawImplBase
{
    GiGraphGdip*        m_this;             //!< 拥有者
    G::Graphics*        m_gs;               //!< 绘图输出对象
    G::Graphics*        m_memGs;            //!< 缓冲绘图用的输出对象

    GiContext           m_context;          //!< 当前绘图参数
    G::Pen*             m_pen;              //!< 当前绘图参数对应的画笔
    G::SolidBrush*      m_brush;            //!< 当前绘图参数对应的画刷
    bool                m_penNull;          //!< 当前是否是空画笔
    bool                m_brushNull;        //!< 当前是否是空画刷

    GdipDrawImplBase(GiGraphGdip* owner)
        : m_this(owner), m_gs(NULL), m_memGs(NULL)
    {
    }

    const GiGraphics* owner() const
    {
        return m_this->owner();
    }

    G::Graphics* getDrawGs() const
    {
        return m_memGs != NULL ? m_memGs : m_gs;
    }

    G::Pen* createPen(const GiContext* ctx, bool* pNotSmoothing = NULL)
    {
        G::Pen* pPen = NULL;

        if (ctx == NULL)
            ctx = &m_context;

        m_penNull = ctx->isNullLine();
        if (!m_penNull)
        {
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
                {
                    delete m_pen;
                    m_pen = NULL;
                }

                float width = owner()->calcPenWidth(ctx->getLineWidth());
                GiColor color = owner()->calcPenColor(ctx->getLineColor());
                m_pen = new G::Pen(G::Color(ctx->getLineAlpha(), 
                    color.r, color.g, color.b), width);

                if (m_pen != NULL)
                {
                    m_pen->SetDashStyle((G::DashStyle)ctx->getLineStyle());
                    if (pNotSmoothing != NULL)
                    {
                        *pNotSmoothing = (width <= 1
                            && ctx->getLineStyle() != kLineSolid);
                    }
                }
            }

            pPen = m_pen;
        }

        return pPen;
    }

    G::SolidBrush* createBrush(const GiContext* ctx)
    {
        G::SolidBrush* pBrush = NULL;

        if (ctx == NULL)
            ctx = &m_context;

        m_brushNull = !ctx->hasFillColor();
        if (!m_brushNull)
        {
            if (m_brush == NULL
                || m_context.getFillColor() != ctx->getFillColor()
                || m_context.getFillAlpha() != ctx->getFillAlpha())
            {
                m_context.setFillColor(ctx->getFillColor());
                m_context.setFillAlpha(ctx->getFillAlpha());

                if (m_brush != NULL)
                {
                    delete m_brush;
                    m_brush = NULL;
                }

                GiColor color = owner()->calcPenColor(ctx->getFillColor());
                m_brush = new G::SolidBrush(
                    G::Color(ctx->getFillAlpha(), 
                    color.r, color.g, color.b));
            }
            pBrush = m_brush;
        }

        return pBrush;
    }
};

//! GiGraphGdip的内部实现类
class GiGraphGdipImpl : public GdipDrawImplBase
{
public:
    GiColor             m_bkColor;          //!< 当前背景色
    G::GraphicsPath*    m_path;             //!< 路径对象

    G::Bitmap*          m_memBitmap;        //!< 缓冲位图
    G::CachedBitmap*    m_cachedBmp[2];     //!< 后备缓冲位图

private:
    static long         c_graphCount;       //!< GiGraphGdip count
    static ULONG_PTR    c_gdipToken;        //!< GDI+ token

public:
    GiGraphGdipImpl(GiGraphGdip* owner) : GdipDrawImplBase(owner)
    {
        m_path = NULL;
        m_pen = NULL;
        m_brush = NULL;
        m_penNull = false;
        m_brushNull = true;
        m_memGs = NULL;
        m_cachedBmp[0] = NULL;
        m_cachedBmp[1] = NULL;
        m_memBitmap = NULL;
        m_bkColor = GiColor::White();

        if (1 == InterlockedIncrement(&c_graphCount))
        {
            G::SolidBrush* p = new G::SolidBrush(G::Color());
            if (p != NULL)      // 外界已初始化
            {
                delete p;
            }
            else                // Initialize GDI+
            {
                G::GdiplusStartupInput gdiplusStartupInput;
                G::GdiplusStartup(&c_gdipToken, &gdiplusStartupInput, NULL);
            }
        }
    }

    ~GiGraphGdipImpl()
    {
        if (m_cachedBmp[0] != NULL)
        {
            delete m_cachedBmp[0];
            m_cachedBmp[0] = NULL;
        }
        if (m_cachedBmp[1] != NULL)
        {
            delete m_cachedBmp[1];
            m_cachedBmp[1] = NULL;
        }
        if (0 == InterlockedDecrement(&c_graphCount)
            && c_gdipToken != 0)
        {
            G::GdiplusShutdown(c_gdipToken);    // Shutdown GDI+
            c_gdipToken = 0;
        }
    }

    bool drawImage(GiGraphicsImpl* pImpl, G::Bitmap* pBmp, 
        long hmWidth, long hmHeight, const Box2d& rectW, bool fast);
    bool addPolyToPath(G::GraphicsPath* pPath, int count, 
        const Point2d* pxs, const UInt8* types);
};

long        GiGraphGdipImpl::c_graphCount = 0;
ULONG_PTR   GiGraphGdipImpl::c_gdipToken = 0;

GiGraphGdip::GiGraphGdip(GiGraphics* gs) : GiGraphWin(gs)
{
    m_draw = new GiGraphGdipImpl(this);
}

GiGraphGdip::~GiGraphGdip()
{
    delete m_draw;
}

bool GiGraphGdip::isBufferedDrawing() const
{
    return m_draw->m_gs != m_draw->getDrawGs();
}

HDC GiGraphGdip::acquireDC()
{
    HDC hdc = NULL;
    if (m_draw->getDrawGs() != NULL)
    {
        hdc = m_draw->getDrawGs()->GetHDC();
    }
    return hdc;
}

void GiGraphGdip::releaseDC(HDC hdc)
{
    if (hdc != NULL && m_draw->getDrawGs() != NULL)
    {
        m_draw->getDrawGs()->ReleaseHDC(hdc);
    }
}

void* GiGraphGdip::GetGraphics()
{
    return m_draw->getDrawGs();
}

void GiGraphGdip::_antiAliasModeChanged(bool antiAlias)
{
    if (m_draw->getDrawGs() != NULL)
    {
        if (antiAlias)
            m_draw->getDrawGs()->SetSmoothingMode(G::SmoothingModeAntiAlias);
        else
            m_draw->getDrawGs()->SetSmoothingMode(G::SmoothingModeNone);
    }
}

class GdipOverlayBmp
{
public:
    GdipOverlayBmp() : m_bmp(NULL)
    {
    }

    ~GdipOverlayBmp()
    {
        clear();
    }

    void clear()
    {
        if (m_bmp != NULL)
        {
            ::DeleteObject(m_bmp);
            m_bmp = NULL;
        }
    }

    bool save(const GiGraphics* draw, HDC srcDC)
    {
        bool ret = false;

        clear();

        if (srcDC != NULL)
        {
            m_bmp = ::CreateCompatibleBitmap(
                srcDC, draw->xf().getWidth(), draw->xf().getHeight());
            if (m_bmp != NULL)
            {
                GiCompatibleDC memDC (srcDC);
                if (memDC != NULL)
                {
                    HGDIOBJ oldBmp = ::SelectObject(memDC, m_bmp);
                    ret = ::BitBlt(memDC, 0, 0, 
                        draw->xf().getWidth(), draw->xf().getHeight(),
                        srcDC, 0, 0, SRCCOPY) != FALSE;
                    ::SelectObject(memDC, oldBmp);
                }
            }
            if (!ret)
                clear();
        }

        return ret;
    }

    bool draw(G::Graphics* gs)
    {
        bool ret = false;

        if (m_bmp != NULL && gs != NULL)
        {
            G::Bitmap image (m_bmp, NULL);
            ret = (G::Ok == gs->DrawImage(&image, 0, 0));
        }

        return ret;
    }

private:
    HBITMAP m_bmp;
};

bool GiGraphGdip::beginPaint(HDC hdc, HDC attribDC, bool buffered, bool overlay)
{
    bool ret = (NULL == m_draw->m_gs)
        && GiGraphWin::beginPaint(hdc, attribDC, buffered, overlay);
    if (!ret)
        return false;

    buffered = buffered && !owner()->isPrint();
    COLORREF cr = ::GetBkColor(hdc);
    m_draw->m_bkColor.set(GetRValue(cr), GetGValue(cr), GetBValue(cr));

    GdipOverlayBmp oldDrawing;
    if (buffered && overlay)
        oldDrawing.save(m_owner, hdc);

    m_draw->m_gs = new G::Graphics(hdc);
    if (m_draw->m_gs == NULL)
    {
        GiGraphWin::endPaint(false);
        return false;
    }

    if (buffered)
    {
        m_draw->m_memBitmap = new G::Bitmap(xf().getWidth(), xf().getHeight());
        m_draw->m_memGs = G::Graphics::FromImage(m_draw->m_memBitmap);
        oldDrawing.draw(m_draw->m_memGs);
    }

    _antiAliasModeChanged(owner()->isAntiAliasMode());

    return ret;
}

void GiGraphGdip::clearWnd()
{
    if (!owner()->isPrint() && owner()->isDrawing())
    {
        G::Color color(m_draw->m_bkColor.r, m_draw->m_bkColor.g, m_draw->m_bkColor.b);
        m_draw->getDrawGs()->Clear(color);
    }
}

#define gdipCachedBmp(secondBmp)   \
    m_draw->m_cachedBmp[(secondBmp) ? 1 : 0]

void GiGraphGdip::clearCachedBitmap()
{
    if (gdipCachedBmp(false) != NULL)
    {
        delete gdipCachedBmp(false);
        gdipCachedBmp(false) = NULL;
    }
    if (gdipCachedBmp(true) != NULL)
    {
        delete gdipCachedBmp(true);
        gdipCachedBmp(true) = NULL;
    }
}

bool GiGraphGdip::drawCachedBitmap(float x, float y, bool secondBmp)
{
    bool ret = false;
    G::CachedBitmap* pBmp = gdipCachedBmp(secondBmp);
    if (m_draw->getDrawGs() != NULL && pBmp != NULL)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawCachedBitmap(
            pBmp, mgRound(x), mgRound(y)));
    }
    return ret;
}

bool GiGraphGdip::drawCachedBitmap2(const GiDrawAdapter* p, 
                                    float x, float y, bool secondBmp)
{
    bool ret = false;

    if (m_draw->getDrawGs() && p && p->getGraphType() == getGraphType())
    {
        const GiGraphGdip* gs = static_cast<const GiGraphGdip*>(p);

        if (gs->xf().getWidth() == xf().getWidth()
            && gs->xf().getHeight() == xf().getHeight())
        {
            G::CachedBitmap* pBmp = gdipCachedBmp(secondBmp);
            if (pBmp != NULL)
            {
                ret = (G::Ok == m_draw->getDrawGs()->DrawCachedBitmap(
                    pBmp, mgRound(x), mgRound(y)));
            }
        }
    }

    return ret;
}

void GiGraphGdip::saveCachedBitmap(bool secondBmp)
{
    G::CachedBitmap*& pBmp = gdipCachedBmp(secondBmp);
    if (pBmp != NULL)
    {
        delete pBmp;
        pBmp = NULL;
    }
    if (m_draw->getDrawGs() != NULL && m_draw->m_memBitmap != NULL)
    {
        pBmp = new G::CachedBitmap(
            m_draw->m_memBitmap, m_draw->getDrawGs());
    }
}

bool GiGraphGdip::hasCachedBitmap(bool secondBmp) const
{
    return gdipCachedBmp(secondBmp) != NULL;
}

void GiGraphGdip::endPaint(bool draw)
{
    if (owner()->isDrawing())
    {
        if (m_draw->m_memGs != NULL && draw)
        {
            m_draw->m_gs->SetInterpolationMode(G::InterpolationModeDefault);
            m_draw->m_gs->DrawImage(m_draw->m_memBitmap, 0, 0);
        }

        if (m_draw->m_pen != NULL)
        {
            delete m_draw->m_pen;
            m_draw->m_pen = NULL;
        }
        if (m_draw->m_brush != NULL)
        {
            delete m_draw->m_brush;
            m_draw->m_brush = NULL;
        }

        if (m_draw->m_memGs != NULL)
        {
            delete m_draw->m_memGs;
            m_draw->m_memGs = NULL;
        }
        if (m_draw->m_memBitmap != NULL)
        {
            delete m_draw->m_memBitmap;
            m_draw->m_memBitmap = NULL;
        }
        if (m_draw->m_path != NULL)
        {
            delete m_draw->m_path;
            m_draw->m_path = NULL;
        }
        if (m_draw->m_gs != NULL)
        {
            delete m_draw->m_gs;
            m_draw->m_gs = NULL;
        }

        GiGraphWin::endPaint(draw);
    }
}

void GiGraphGdip::_clipBoxChanged(const RECT2D& clipBox)
{
    m_draw->getDrawGs()->SetClip(G::RectF(clipBox.left, clipBox.top, 
        clipBox.right - clipBox.left, clipBox.bottom - clipBox.top));
}

GiColor GiGraphGdip::getBkColor() const
{
    return m_draw->m_bkColor;
}

GiColor GiGraphGdip::setBkColor(const GiColor& color)
{
    GiColor crOld = m_draw->m_bkColor;
    m_draw->m_bkColor = color;
    return crOld;
}

GiColor GiGraphGdip::getNearestColor(const GiColor& color) const
{
    GiColor ret = color;

    if (m_attribDC != NULL)
    {
        COLORREF cr = ::GetNearestColor(m_attribDC, RGB(ret.r, ret.g, ret.b));
        ret.set(GetRValue(cr), GetGValue(cr), GetBValue(cr));
    }
    else if (m_draw->getDrawGs() != NULL)
    {
        G::Color c(ret.r, ret.g, ret.b);
        m_draw->getDrawGs()->GetNearestColor(&c);
        ret.set(c.GetR(), c.GetG(), c.GetB());
    }

    return ret;
}

const GiContext* GiGraphGdip::getCurrentContext() const
{
    return &m_draw->m_context;
}

class TempGdipPen
{
public:
    TempGdipPen(GdipDrawImplBase* pdraw, const GiContext* ctx)
        : m_gs(pdraw->getDrawGs())
        , m_oldMode(G::SmoothingModeDefault)
    {
        bool bNotSmoothing = false;
        m_pen = pdraw->createPen(ctx, &bNotSmoothing);
        if (bNotSmoothing)
        {
            m_oldMode = m_gs->GetSmoothingMode();
            m_gs->SetSmoothingMode(G::SmoothingModeNone);
        }
    }

    ~TempGdipPen()
    {
        if (m_oldMode != G::SmoothingModeDefault)
            m_gs->SetSmoothingMode(m_oldMode);
    }

    operator G::Pen* ()
    {
        return m_pen;
    }

private:
    G::Graphics*    m_gs;
    G::Pen*         m_pen;
    G::SmoothingMode    m_oldMode;
};

bool GiGraphGdip::rawLine(const GiContext* ctx, 
                          float x1, float y1, float x2, float y2)
{
    bool ret = false;
    TempGdipPen pPen(m_draw, ctx);

    if (pPen != NULL)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawLine(pPen, x1, y1, x2, y2));
    }

    return ret;
}

bool GiGraphGdip::rawPolyline(const GiContext* ctx, 
                              const Point2d* pxs, int count)
{
    bool ret = false;
    TempGdipPen pPen(m_draw, ctx);

    if (pPen != NULL)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawLines(
            pPen, (G::PointF*)pxs, count));
    }

    return ret;
}

bool GiGraphGdip::rawPolyBezier(const GiContext* ctx, 
                                const Point2d* pxs, int count)
{
    bool ret = false;
    TempGdipPen pPen(m_draw, ctx);

    if (pPen != NULL)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawBeziers(
            pPen, (G::PointF*)pxs, count));
    }

    return ret;
}

bool GiGraphGdip::rawPolygon(const GiContext* ctx, 
                             const Point2d* pxs, int count)
{
    bool ret = false;
    TempGdipPen pPen(m_draw, ctx);
    G::Brush* pBrush = m_draw->createBrush(ctx);

    if (pBrush != NULL)
    {
        ret = (G::Ok == m_draw->getDrawGs()->FillPolygon(pBrush, 
            (G::PointF*)pxs, count));
    }
    if (pPen != NULL)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawPolygon(
            pPen, (G::PointF*)pxs, count));
    }

    return ret;
}

bool GiGraphGdip::rawRect(const GiContext* ctx, 
                          float x, float y, float w, float h)
{
    bool ret = false;
    TempGdipPen pPen(m_draw, ctx);
    G::Brush* pBrush = m_draw->createBrush(ctx);

    if (w < 0)
    {
        x += w;
        w = -w;
    }
    if (h < 0)
    {
        y += h;
        h = -h;
    }

    if (pBrush != NULL && w > 0 && h > 0)
    {
        ret = (G::Ok == m_draw->getDrawGs()->FillRectangle(pBrush, x, y, w, h));
    }
    if (pPen != NULL && w > 0 && h > 0)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawRectangle(pPen, x, y, w, h));
    }

    return ret;
}

bool GiGraphGdip::rawEllipse(const GiContext* ctx, 
                             float x, float y, float w, float h)
{
    bool ret = false;
    TempGdipPen pPen(m_draw, ctx);
    G::Brush* pBrush = m_draw->createBrush(ctx);

    if (w < 0)
    {
        x += w;
        w = -w;
    }
    if (h < 0)
    {
        y += h;
        h = -h;
    }

    if (pBrush != NULL && w > 0 && h > 0)
    {
        ret = (G::Ok == m_draw->getDrawGs()->FillEllipse(pBrush, x, y, w, h));
    }
    if (pPen != NULL && w > 0 && h > 0)
    {
        ret = (G::Ok == m_draw->getDrawGs()->DrawEllipse(pPen, x, y, w, h));
    }

    return ret;
}

bool GiGraphGdip::rawBeginPath()
{
    if (m_draw->m_path != NULL)
    {
        delete m_draw->m_path;
        m_draw->m_path = NULL;
    }
    m_draw->m_path = new G::GraphicsPath;
    return m_draw->m_path != NULL;
}

bool GiGraphGdip::rawEndPath(const GiContext* ctx, bool fill)
{
    bool ret = false;

    if (m_draw->m_path != NULL)
    {
        if (fill)
        {
            G::Brush* pBrush = m_draw->createBrush(ctx);
            if (pBrush != NULL)
            {
                ret = (G::Ok == m_draw->getDrawGs()->FillPath(
                    pBrush, m_draw->m_path));
            }
        }
        TempGdipPen pPen(m_draw, ctx);
        if (pPen != NULL)
        {
            ret = (G::Ok == m_draw->getDrawGs()->DrawPath(pPen, m_draw->m_path));
        }

        delete m_draw->m_path;
        m_draw->m_path = NULL;
    }

    return ret;
}

bool GiGraphGdip::rawMoveTo(float x, float y)
{
    bool ret = false;

    if (m_draw->m_path != NULL)
    {
        if (m_draw->m_path->GetPointCount() > 0)
            ret = (G::Ok == m_draw->m_path->StartFigure());
        ret = (G::Ok == m_draw->m_path->AddLine(x, y, x, y));
    }

    return ret;
}

bool GiGraphGdip::rawLineTo(float x, float y)
{
    bool ret = false;

    if (m_draw->m_path != NULL)
    {
        G::PointF pt;
        ret = (G::Ok == m_draw->m_path->GetLastPoint(&pt));
        ret = (G::Ok == m_draw->m_path->AddLine(pt.X, pt.Y, x, y));
    }

    return ret;
}

bool GiGraphGdip::rawPolyBezierTo(const Point2d* pxs, int count)
{
    bool ret = false;
    G::PointF pts[4];

    if (m_draw->m_path != NULL)
    {
        ret = (G::Ok == m_draw->m_path->GetLastPoint(&pts[0]));
        for (int i = 0; i + 2 < count; i += 3)
        {
            pts[1].X = pxs[i].x;
            pts[1].Y = pxs[i].y;
            pts[2].X = pxs[i+1].x;
            pts[2].Y = pxs[i+1].y;
            pts[3].X = pxs[i+2].x;
            pts[3].Y = pxs[i+2].y;
            ret = (G::Ok == m_draw->m_path->AddBezier(
                pts[0], pts[1], pts[2], pts[3]));
            pts[0] = pts[3];
        }
    }

    return ret;
}

bool GiGraphGdip::rawCloseFigure()
{
    bool ret = false;
    if (m_draw->m_path != NULL)
    {
        ret = (G::Ok == m_draw->m_path->CloseFigure());
    }

    return ret;
}

bool GiGraphGdip::rawPolyDraw(const GiContext* ctx, int count, 
                              const Point2d* pxs, const UInt8* types)
{
    bool ret = false;
    G::GraphicsPath* pPath = NULL;

    if (pxs != NULL && types != NULL && count > 1
        && (pPath = new G::GraphicsPath) != NULL)
    {
        if (NULL == ctx)
            ctx = getCurrentContext();

        ret = m_draw->addPolyToPath(pPath, count, pxs, types);
        if (ret)
        {
            if (ctx->hasFillColor())
            {
                G::Brush* pBrush = m_draw->createBrush(ctx);
                if (pBrush != NULL)
                {
                    ret = (G::Ok == m_draw->getDrawGs()->FillPath(pBrush, pPath));
                }
            }
            TempGdipPen pPen(m_draw, ctx);
            if (pPen != NULL)
            {
                ret = (G::Ok == m_draw->getDrawGs()->DrawPath(pPen, pPath));
            }
        }
    }

    if (pPath != NULL)
        delete pPath;

    return ret;
}

bool GiGraphGdipImpl::addPolyToPath(G::GraphicsPath* pPath, int count, 
                                    const Point2d* pxs, const UInt8* types)
{
    Point2d pt;
    bool ret = true;

    for (int i = 0; i < count && ret; i++)
    {
        if (PT_MOVETO == types[i])
        {
            if (pPath->GetPointCount() > 0)
                ret = (G::Ok == pPath->StartFigure());
            pt = pxs[i];
            ret = (G::Ok == pPath->AddLine(pt.x, pt.y, pt.x, pt.y));
        }
        else if (PT_LINETO == (types[i] & (PT_LINETO | PT_BEZIERTO)))
        {
            ret = (G::Ok == pPath->AddLine(pt.x, pt.y, pxs[i].x, pxs[i].y));
            pt = pxs[i];
        }
        else if (PT_BEZIERTO == (types[i] & (PT_LINETO | PT_BEZIERTO)))
        {
            if (i + 2 >= count
                || PT_BEZIERTO != (types[i+1] & (PT_LINETO | PT_BEZIERTO))
                || PT_BEZIERTO != (types[i+2] & (PT_LINETO | PT_BEZIERTO)))
            {
                ret = false;
            }
            else
            {
                ret = (G::Ok == pPath->AddBezier(
                    pt.x, pt.y, 
                    pxs[i].x, pxs[i].y, 
                    pxs[i+1].x, pxs[i+1].y, 
                    pxs[i+2].x, pxs[i+2].y));
                i += 2;
                pt = pxs[i];
            }
        }

        if (PT_CLOSEFIGURE == (types[i] & PT_CLOSEFIGURE))
        {
            ret = (G::Ok == pPath->CloseFigure());
        }
    }

    return ret;
}

bool GiGraphGdipImpl::drawImage(GiGraphicsImpl* pImpl, G::Bitmap* pBmp, 
                                long hmWidth, long hmHeight, 
                                const Box2d& rectW, bool fast)
{
    RECT2D rc, rcDraw, rcFrom;
    Box2d rect;

    // rc: 整个图像对应的显示坐标区域
    (rectW * owner()->xf().worldToDisplay()).get(rc);

    // rcDraw: 图像经剪裁后的可显示部分
    if (rect.intersectWith(Box2d(rc), Box2d(pImpl->clipBox)).isEmpty())
        return false;
    rect.get(rcDraw);

    float width = (float)hmWidth * owner()->xf().getDpiX() / 2540.f;
    float height = (float)hmHeight * owner()->xf().getDpiY() / 2540.f;

    // rcFrom: rcDraw在原始图像上对应的图像范围
    rcFrom.left = (rcDraw.left - rc.left) * width / (rc.right - rc.left);
    rcFrom.top  = (rcDraw.top - rc.top) * height / (rc.bottom - rc.top);
    rcFrom.right = (rcDraw.right - rc.left) * width / (rc.right - rc.left);
    rcFrom.bottom = (rcDraw.bottom - rc.top) * height / (rc.bottom - rc.top);

    // 根据rectW正负决定是否颠倒显示图像
    if (rectW.xmin > rectW.xmax)
        mgSwap(rcDraw.left, rcDraw.right);
    if (rectW.ymin > rectW.ymax)
        mgSwap(rcDraw.top, rcDraw.bottom);

    G::InterpolationMode nOldMode = getDrawGs()->GetInterpolationMode();
    getDrawGs()->SetInterpolationMode( (!fast || pImpl->isPrint)
        ? G::InterpolationModeBilinear : G::InterpolationModeLowQuality);

    G::Status ret = getDrawGs()->DrawImage(pBmp, 
        G::RectF(rcDraw.left, rcDraw.top, 
        rcDraw.right - rcDraw.left, 
        rcDraw.bottom - rcDraw.top), 
        rcFrom.left, rcFrom.top, 
        rcFrom.right - rcFrom.left, 
        rcFrom.bottom - rcFrom.top, 
        G::UnitPixel);
    getDrawGs()->SetInterpolationMode(nOldMode);

    return G::Ok == ret;
}

bool GiGraphGdip::drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
                            const Box2d& rectW, bool fast)
{
    bool ret = false;

    if (m_draw->getDrawGs() != NULL
        && hmWidth > 0 && hmHeight > 0 && hbitmap != NULL
        && owner()->getClipWorld().isIntersect(Box2d(rectW, true)))
    {
        G::Bitmap bmp (hbitmap, NULL);
        ret = m_draw->drawImage(m_impl, &bmp, 
            hmWidth, hmHeight, rectW, fast);
    }

    return ret;
}

bool GiGraphGdip::drawGdipImage(long hmWidth, long hmHeight, LPVOID pBmp, 
                                const Box2d& rectW, bool fast)
{
    bool ret = false;

    if (m_draw->getDrawGs() != NULL
        && hmWidth > 0 && hmHeight > 0 && pBmp != NULL
        && owner()->getClipWorld().isIntersect(Box2d(rectW, true)))
    {
        ret = m_draw->drawImage(m_impl, (G::Bitmap*)pBmp, 
            hmWidth, hmHeight, rectW, fast);
    }

    return ret;
}
