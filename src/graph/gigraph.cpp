// gigraph.cpp: 实现图形系统类GiGraphics
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "gigraph.h"
#include "_gigraph.h"
#include <mglnrel.h>
#include <mgcurv.h>
#include "giplclip.h"

_GEOM_BEGIN

GiGraphics::GiGraphics(GiTransform& xform)
{
    m_impl = new GiGraphicsImpl(this, xform);
}

GiGraphics::GiGraphics(const GiGraphics& src)
{
    m_impl = new GiGraphicsImpl(this, src.m_impl->xform);
    operator=(src);
}

GiGraphics::~GiGraphics()
{
    delete m_impl;
}

GiGraphics& GiGraphics::operator=(const GiGraphics& src)
{
    if (this != &src)
    {
        m_impl->maxPenWidth = src.m_impl->maxPenWidth;
        m_impl->antiAlias = src.m_impl->antiAlias;
        m_impl->colorMode = src.m_impl->colorMode;
    }
    return *this;
}

const GiTransform& GiGraphics::xf() const
{
    return m_impl->xform;
}

void GiGraphics::beginPaint()
{
    if (m_impl->lastZoomTimes != xf().getZoomTimes())
    {
        m_impl->zoomChanged();
        m_impl->lastZoomTimes = xf().getZoomTimes();
    }
    giInterlockedIncrement(&m_impl->drawRefcnt);
}

void GiGraphics::endPaint()
{
    giInterlockedDecrement(&m_impl->drawRefcnt);
}

bool GiGraphics::isDrawing() const
{
    return m_impl->drawRefcnt > 0;
}

Box2d GiGraphics::getClipModel() const
{
    return m_impl->rectDrawM;
}

void GiGraphics::getClipBox(RECT* prc) const
{
    if (prc != NULL)
        memcpy(prc, &m_impl->clipBox, sizeof(RECT));
}

#ifndef _WIN32
bool IntersectRect(RECT *dst, const RECT *p1, const RECT *p2)
{
    Box2d r1(p1->left, p1->top, p1->right, p1->bottom, true);
    Box2d r2(p2->left, p2->top, p2->right, p2->bottom, true);

    r1.intersectWith(r2);
    r1.get(dst->left, dst->top, dst->right, dst->bottom);

    return !r1.isEmpty();
}
#endif

bool GiGraphics::setClipBox(const RECT* prc)
{
    if (!prc || m_impl->drawRefcnt < 1)
        return false;

    bool ret = false;
    RECT rc;

    if (IntersectRect(&rc, prc, &m_impl->clipBox0))
    {
        if (0 != memcmp(&m_impl->clipBox, &rc, sizeof(RECT)))
        {
            memcpy(&m_impl->clipBox, &rc, sizeof(RECT));
            m_impl->rectDraw.set(rc.left, rc.top, rc.right, rc.bottom);
            m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
            m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
            m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
        }
        ret = true;
    }

    return ret;
}

bool GiGraphics::setClipWorld(const Box2d& rectWorld)
{
    bool ret = false;

    if (isDrawing() && !rectWorld.isEmpty())
    {
        Box2d box (rectWorld * xf().worldToDisplay());
        box.intersectWith(Box2d(m_impl->clipBox0.left, m_impl->clipBox0.top, 
            m_impl->clipBox0.right, m_impl->clipBox0.bottom));
        if (!box.isEmpty(Tol(1, 0)))
        {
            RECT rc = { mgRound(box.xmin), mgRound(box.ymin),
                mgRound(box.xmax), mgRound(box.ymax) };

            if (0 != memcmp(&m_impl->clipBox, &rc, sizeof(RECT)))
            {
                memcpy(&m_impl->clipBox, &rc, sizeof(RECT));
                m_impl->rectDraw.set(rc.left, rc.top, rc.right, rc.bottom);
                m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
                m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
                m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
            }

            ret = true;
        }
    }

    return ret;
}

bool GiGraphics::isAntiAliasMode() const
{
    return m_impl->antiAlias;
}

void GiGraphics::setAntiAliasMode(bool antiAlias)
{
    m_impl->antiAlias = antiAlias;
}

int GiGraphics::getColorMode() const
{
    return m_impl->colorMode;
}

void GiGraphics::setColorMode(int mode)
{
    m_impl->colorMode = mode;
}

GiColor GiGraphics::calcPenColor(const GiColor& color) const
{
    const GiColor& bk = getBkColor();
    GiColor ret = color;

    if (kColorMono == m_impl->colorMode && color != bk)
        ret.set(~bk.r, ~bk.g, ~bk.b);

    if (m_impl->drawColors == 2           // 黑白
        || kColorGray == m_impl->colorMode)
    {
        unsigned char by = (unsigned char)(
            (77 * ret.r + 151 * ret.g + 28 * ret.b + 128) / 256);
        ret.set(by, by, by);
    }
    if (m_impl->drawColors > 0 && m_impl->drawColors < 8)  // 少于256色
    {
        ret = getNearestColor(ret);
    }

    return ret;
}

UInt16 GiGraphics::calcPenWidth(Int16 lineWidth, bool useViewScale) const
{
    long w = 1;
    double px;

    if (m_impl->maxPenWidth <= 1)
        lineWidth = 0;

    if (lineWidth > 0)      // 单位：0.01mm
    {
        px = lineWidth / 2540.0 * xf().getDpiY();
        if (useViewScale)
            px *= xf().getViewScale();
        w = mgRound(px);
        w = mgMin(w, (long)m_impl->maxPenWidth);
    }
    else if (lineWidth < 0) // 单位：像素
    {
        w = mgMin(-lineWidth, (int)m_impl->maxPenWidth);
    }

    if (w < 1)
        w = 1;
    if (lineWidth <= 0 && xf().getDpiY() > getScreenDpi())
        w = w * xf().getDpiY() / getScreenDpi();

    return static_cast<UInt16>(w);
}

void GiGraphics::setMaxPenWidth(UInt8 pixels)
{
    if (pixels < 1)
        pixels = 1;
    else if (pixels > 50)
        pixels = 50;
    m_impl->maxPenWidth = pixels;
}

static inline const Matrix2d& S2D(const GiTransform& xf, bool modelUnit)
{
    return modelUnit ? xf.modelToDisplay() : xf.worldToDisplay();
}

static inline const Box2d& DRAW_RECT(const GiGraphicsImpl* p, bool modelUnit)
{
    return modelUnit ? p->rectDrawM : p->rectDrawW;
}

static inline const Box2d& DRAW_MAXR(const GiGraphicsImpl* p, bool modelUnit)
{
    return modelUnit ? p->rectDrawMaxM : p->rectDrawMaxW;
}

bool GiGraphics::drawLine(const GiContext* ctx, 
                          const Point2d& startPt, const Point2d& endPt, 
                          bool modelUnit)
{
    if (m_impl->drawRefcnt == 0)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(Box2d(startPt, endPt)))
        return false;

    Point2d pts[2] = { startPt * S2D(xf(), modelUnit), endPt * S2D(xf(), modelUnit) };

    if (!mgClipLine(pts[0], pts[1], m_impl->rectDraw))
        return false;

    return rawLine(ctx, mgRound(pts[0].x), mgRound(pts[0].y), 
        mgRound(pts[1].x), mgRound(pts[1].y));
}

class PolylineAux
{
    GiGraphics* m_gs;
    const GiContext* m_pContext;
public:
    PolylineAux(GiGraphics* gs, const GiContext* ctx)
        : m_gs(gs), m_pContext(ctx)
    {
    }
    bool draw(const POINT* lppt, int n) const
    {
        return lppt && n > 1 && m_gs->rawPolyline(m_pContext, lppt, n);
    }
};

static bool DrawEdge(int count, int &i, Point2d* pts, Point2d &ptLast, 
                     const PolylineAux& aux, const Box2d& rectDraw)
{
    int n, si, ei;
    Point2d pt1, pt2;

    pt1 = ptLast;
    ptLast = pts[i+1];
    pt2 = ptLast;
    if (!mgClipLine(pt1, pt2, rectDraw))    // 该边不可见
        return false;

    si = i;                                 // 收集第一条可见边
    ei = i+1;
    pts[i] = pt1;                           // 收集起点
    if (pt2 != ptLast)                      // 如果该边终点不可见
    {
        pts[i+1] = pt2;                     // 收集交点
    }
    else                                    // 否则继续收集后续点
    {
        i++;                                // 下一边的起点可见，跳过
        for (; i < count - 1; i++)          // 依次处理后续可见边
        {
            pt1 = ptLast;
            ptLast = pts[i+1];
            pt2 = ptLast;
            if (!mgClipLine(pt1, pt2, rectDraw)) // 该边不可见
                break;
            ei++;
            if (pt2 != ptLast)              // 该边起点可见，终点不可见
            {
                pts[i+1] = pt2;             // 收集交点
                break;
            }
        }
    }

    // 显示找到的多条线段
    n = ei - si + 1;
    if (n > 1)
    {
        vector<POINT> pxpoints;
        pxpoints.resize(n);
        POINT* lppt = &pxpoints.front();
        n = 0;
        for (int j = si; j <= ei; j++)
        {
            // 记下第一个点，其他点如果和上一点不重合则记下，否则跳过
            if (j == si || fabs(pt1.x - pts[j].x) > 2
                || fabs(pt1.y - pts[j].y) > 2)
            {
                pt1 = pts[j];
                pt1.get(lppt[n].x, lppt[n].y);
                n++;
            }
        }

        return aux.draw(lppt, n);
    }

    return false;
}

bool GiGraphics::drawLines(const GiContext* ctx, int count, 
                           const Point2d* points, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || points == NULL)
        return false;
    if (count > 0x2000)
        count = 0x2000;
    GiLock lock (&m_impl->drawRefcnt);

    int i;
    Point2d pt1, pt2, ptLast;
    vector<POINT> pxpoints;
    vector<Point2d> pointBuf;
    bool ret = false;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).isInside(extent))    // 全部在显示区域内
    {
        pxpoints.resize(count);
        POINT* lppt = &pxpoints.front();
        int n = 0;
        for (i = 0; i < count; i++)
        {
            pt2 = points[i] * matD;
            if (i == 0 || fabs(pt1.x - pt2.x) > 2 || fabs(pt1.y - pt2.y) > 2)
            {
                pt1 = pt2;
                pt2.get(lppt[n].x, lppt[n].y);
                n++;
            }
        }
        ret = rawPolyline(ctx, lppt, n);
    }
    else                                            // 部分在显示区域内
    {
        pointBuf.resize(count);
        for (i = 0; i < count; i++)                 // 转换到像素坐标
            pointBuf[i] = points[i] * matD;
        Point2d* pts = &pointBuf.front();

        ptLast = pts[0];
        PolylineAux aux(this, ctx);
        for (i = 0; i < count - 1; i++)
        {
            ret = DrawEdge(count, i, pts, ptLast, aux, m_impl->rectDraw) || ret;
        }
    }

    return ret;
}

bool GiGraphics::drawBeziers(const GiContext* ctx, int count, 
                             const Point2d* points, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 4 || points == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    if (count > 0x2000)
        count = 0x2000;
    count = 1 + (count - 1) / 3 * 3;

    bool ret = false;
    vector<POINT> pxpoints;
    vector<Point2d> pointBuf;
    int i, j, n, si, ei;
    POINT * lppt;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).isInside(extent))    // 全部在显示区域内
    {
        pxpoints.resize(count);
        lppt = &pxpoints.front();
        for (i = 0; i < count; i++)
            (points[i] * matD).get(lppt[i].x, lppt[i].y);
        ret = rawPolyBezier(ctx, lppt, count);
    }
    else
    {        
        pointBuf.resize(count);
        for (i = 0; i < count; i++)                 // 转换到像素坐标
            pointBuf[i] = points[i] * matD;
        Point2d* pts = &pointBuf.front();

        si = ei = 0;
        for (i = 3; i < count; i += 3)
        {
            for ( ; i < count
                && m_impl->rectDraw.isIntersect(Box2d(4, &pts[ei])); i += 3)
                ei = i;
            n = ei - si + 1;
            if (n > 1)
            {
                pxpoints.resize(n);
                lppt = &pxpoints.front();
                for (j=0; j<n; j++)
                    pts[si + j].get(lppt[j].x, lppt[j].y);
                ret = rawPolyBezier(ctx, lppt, n);
            }
            si = ei = i;
        }
    }
    return ret;
}

bool GiGraphics::drawArc(const GiContext* ctx, 
                         const Point2d& center, double rx, double ry, 
                         double startAngle, double sweepAngle, 
                         bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO || fabs(sweepAngle) < 1e-5)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (ry < _MGZERO)
        ry = rx;

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(Box2d(center, 2 * rx, 2 * ry)))
        return false;

    Point2d points[16];
    int count = mgAngleArcToBezier(points, center,
        rx, ry, startAngle, sweepAngle);
    if (count < 4)
        return false;
    S2D(xf(), modelUnit).TransformPoints(count, points);
    POINT cen;
    (center * S2D(xf(), modelUnit)).get(cen.x, cen.y);

    vector<POINT> pxpoints;
    pxpoints.resize(count);
    POINT *lppt = &pxpoints.front();
    for (int i = 0; i < count; i++)
        points[i].get(lppt[i].x, lppt[i].y);

    return rawPolyBezier(ctx, lppt, count);
}

static inline int findInvisibleEdge(const PolygonClip& clip)
{
    int i = 0;
    for (; i < clip.getCount() && clip.isLinked(i); i++) ;
    return i;
}

static inline int findVisibleEdge(const PolygonClip& clip, int i, int end)
{
    for (; (i - end) % clip.getCount() != 0 && !clip.isLinked(i); i++) ;
    return i;
}

static inline int findInvisibleEdge(const PolygonClip& clip, int i, int end)
{
    for (; (i - end) % clip.getCount() != 0 && clip.isLinked(i); i++) ;
    return i;
}

static bool drawPolygonEdge(const PolylineAux& aux, 
                            int count, const PolygonClip& clip, 
                            int ienter)
{
    bool ret = false;
    vector<POINT> pxpoints;
    Point2d pt1, pt2;
    int si, ei, n, i;

    for (si = ei = ienter + 1; (ei - ienter) % count != 0; )
    {
        si = findVisibleEdge(clip, ei, ienter);
        ei = findInvisibleEdge(clip, si, ienter);
        n = ei - si + 1;
        if (n > 1)
        {
            pxpoints.resize(n);
            POINT *lppt = &pxpoints.front();
            n = 0;
            for (i = si; i <= ei; i++)
            {
                pt2 = clip.getPoint(i);
                if (i == si || fabs(pt1.x - pt2.x) > 2
                    || fabs(pt1.y - pt2.y) > 2)
                {
                    pt1 = pt2;
                    pt1.get(lppt[n].x, lppt[n].y);
                    n++;
                }
            }

            ret = aux.draw(lppt, n) || ret;
        }
    }

    return ret;
}

static bool _DrawPolygon(GiGraphics* gs, const GiContext* ctx, 
                         int count, const Point2d* points, 
                         bool bM2D, bool bFill, bool bEdge, bool modelUnit)
{
    if (NULL == ctx)
        ctx = gs->getCurrentContext();
    if (NULL == ctx)
        return false;

    vector<POINT> pxpoints;
    Point2d pt1, pt2;
    Matrix2d matD(S2D(gs->xf(), modelUnit));

    pxpoints.resize(count);
    POINT *lppt = &pxpoints.front();
    int n = 0;
    for (int i = 0; i < count; i++)
    {
        pt2 = points[i];
        if (bM2D)
            pt2 *= matD;
        if (i == 0 || fabs(pt1.x - pt2.x) > 2
            || fabs(pt1.y - pt2.y) > 2)
        {
            pt1 = pt2;
            pt1.get(lppt[n].x, lppt[n].y);
            n++;
        }
    }

    GiContext context (*ctx);
    if (!bEdge)
        context.setNullLine();
    if (!bFill)
        context.setNoFillColor();

    return gs->rawPolygon(&context, lppt, n);
}

bool GiGraphics::drawPolygon(const GiContext* ctx, int count, 
                             const Point2d* points, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || points == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    if (count > 0x2000)
        count = 0x2000;

    bool ret = false;

    const Box2d extent (count, points);             // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).isInside(extent))    // 全部在显示区域内
    {
        ret = _DrawPolygon(this, ctx, 
            count, points, true, true, true, modelUnit);
    }
    else                                                // 部分在显示区域内
    {
        PolygonClip clip (m_impl->rectDraw);
        if (!clip.clip(count, points, &S2D(xf(), modelUnit)))  // 多边形剪裁
            return false;
        count = clip.getCount();
        points = clip.getPoints();

        ret = _DrawPolygon(this, ctx, 
            count, points, false, true, false, modelUnit);

        int ienter = findInvisibleEdge(clip);
        if (ienter == count)
        {
            ret = _DrawPolygon(this, ctx, count, points, 
                false, false, true, modelUnit) || ret;
        }
        else
        {
            ret = drawPolygonEdge(PolylineAux(this, ctx), count, clip, ienter) || ret;
        }
    }

    return ret;
}

bool GiGraphics::drawEllipse(const GiContext* ctx, 
                             const Point2d& center, double rx, double ry, 
                             bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    bool ret = false;
    Matrix2d matD(S2D(xf(), modelUnit));

    if (ry < _MGZERO)
        ry = rx;

    const Box2d extent (center, rx, ry);                    // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    if (mgIsZero(matD.m12) && mgIsZero(matD.m21))
    {
        Point2d cen (center * matD);
        rx *= matD.m11;
        ry *= matD.m22;

        ret = rawEllipse(ctx, mgRound(cen.x - rx), mgRound(cen.y - ry),
            mgRound(2 * rx), mgRound(2 * ry));
    }
    else
    {
        Point2d points[13];
        mgEllipseToBezier(points, center, rx, ry);
        matD.TransformPoints(13, points);

        POINT pts[13];
        for (int i = 0; i < 13; i++)
            points[i].get(pts[i].x, pts[i].y);

        ret = rawBeginPath();
        if (ret)
        {
            ret = rawMoveTo(pts[0].x, pts[0].y);
            ret = rawPolyBezierTo(pts + 1, 12);
            ret = rawEndPath(ctx, true);
        }
    }

    return ret;
}

bool GiGraphics::drawPie(const GiContext* ctx, 
                         const Point2d& center, double rx, double ry, 
                         double startAngle, double sweepAngle, 
                         bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO || fabs(sweepAngle) < 1e-5)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (ry < _MGZERO)
        ry = rx;

    const Box2d extent (center, rx, ry);                    // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    Point2d points[16];
    int count = mgAngleArcToBezier(points, center,
        rx, ry, startAngle, sweepAngle);
    if (count < 4)
        return false;
    S2D(xf(), modelUnit).TransformPoints(count, points);
    POINT cen;
    (center * S2D(xf(), modelUnit)).get(cen.x, cen.y);

    vector<POINT> pxpoints;
    pxpoints.resize(count);
    POINT *lppt = &pxpoints.front();
    for (int i = 0; i < count; i++)
        points[i].get(lppt[i].x, lppt[i].y);

    bool ret = rawBeginPath();
    if (ret)
    {
        ret = rawMoveTo(cen.x, cen.y);
        ret = rawLineTo(lppt[0].x, lppt[0].y);
        ret = rawPolyBezierTo(lppt + 1, count - 1);
        ret = rawCloseFigure();
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawRect(const GiContext* ctx, const Box2d& rect, 
                          bool modelUnit)
{
    Point2d points[4] = {
        rect.leftBottom(), rect.rightBottom(), 
        rect.rightTop(), rect.leftTop()
    };
    return drawPolygon(ctx, 4, points, modelUnit);
}

bool GiGraphics::drawRoundRect(const GiContext* ctx, 
                               const Box2d& rect, double rx, double ry, 
                               bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rect.isEmpty())
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    bool ret = false;

    if (ry < _MGZERO)
        ry = rx;

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(rect))  // 全部在显示区域外
        return false;

    if (rx < _MGZERO)
    {
        Point2d points[4] = {
            rect.leftBottom(), rect.rightBottom(), 
            rect.rightTop(), rect.leftTop()
        };
        return drawPolygon(ctx, 4, points);
    }
    else
    {
        Point2d points[16];
        mgRoundRectToBeziers(points, rect, rx, ry);

        S2D(xf(), modelUnit).TransformPoints(16, points);

        POINT pts[16];
        for (int i = 0; i < 16; i++)
            points[i].get(pts[i].x, pts[i].y);

        ret = rawBeginPath();
        if (ret)
        {
            ret = rawMoveTo(pts[0].x, pts[0].y);
            ret = rawPolyBezierTo(&pts[1], 3);

            ret = rawLineTo(pts[4].x, pts[4].y);
            ret = rawPolyBezierTo(&pts[5], 3);

            ret = rawLineTo(pts[8].x, pts[8].y);
            ret = rawPolyBezierTo(&pts[9], 3);

            ret = rawLineTo(pts[12].x, pts[12].y);
            ret = rawPolyBezierTo(&pts[13], 3);

            ret = rawCloseFigure();
            ret = rawEndPath(ctx, true);
        }
    }

    return ret;
}

bool GiGraphics::drawSplines(const GiContext* ctx, int count, 
                             const Point2d* knots, 
                             const Vector2d* knotVectors, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 
        || knots == NULL || knotVectors == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>(1 + (0x2000 - 1) / 3));

    int i;
    Point2d pt;
    Vector2d vec;
    vector<POINT> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟整形像素坐标数组
    pxpoints.resize(1 + (count - 1) * 3);
    POINT *lppt = &pxpoints.front();

    pt = knots[0] * matD;                               // 第一个Bezier段的起点
    vec = knotVectors[0] * matD / 3.0;                  // 第一个Bezier段的起始矢量
    pt.get(lppt->x, lppt->y); lppt++;                   // 产生Bezier段的起点
    for (i = 1; i < count; i++)                         // 计算每一个Bezier段
    {
        (pt += vec).get(lppt->x, lppt->y); lppt++;      // 产生Bezier段的第二点
        pt = knots[i] * matD;                           // Bezier段的终点
        vec = knotVectors[i] * matD / 3.0;              // Bezier段的终止矢量
        (pt - vec).get(lppt->x, lppt->y); lppt++;       // 产生Bezier段的第三点
        pt.get(lppt->x, lppt->y); lppt++;               // 产生Bezier段的终点
    }

    // 绘图
    return rawPolyBezier(ctx, &pxpoints.front(), getSize(pxpoints));
}

bool GiGraphics::drawClosedSplines(const GiContext* ctx, int count, 
                                   const Point2d* knots, 
                                   const Vector2d* knotVectors, 
                                   bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || 
        knots == NULL || knotVectors == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>((0x2000 - 1) / 3));

    int i, j = 0;
    Point2d pt;
    Vector2d vec;
    vector<POINT> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟整形像素坐标数组
    pxpoints.resize(1 + count * 3);
    POINT *lppt = &pxpoints.front();

    pt = knots[0] * matD;                               // 第一个Bezier段的起点
    vec = knotVectors[0] * matD / 3.0;                  // 第一个Bezier段的起始矢量
    pt.get(lppt[j].x, lppt[j].y); j++;                  // 产生Bezier段的起点
    for (i = 1; i < count; i++)                         // 计算每一个Bezier段
    {
        (pt += vec).get(lppt[j].x, lppt[j].y); j++;     // 产生Bezier段的第二点
        pt = knots[i] * matD;                           // Bezier段的终点
        vec = knotVectors[i] * matD / 3.0;              // Bezier段的终止矢量
        (pt - vec).get(lppt[j].x, lppt[j].y); j++;      // 产生Bezier段的第三点
        pt.get(lppt[j].x, lppt[j].y); j++;              // 产生Bezier段的终点
    }
    (pt += vec).get(lppt[j].x, lppt[j].y); j++;         // 产生Bezier段的第二点
    lppt[j].x = 2 * lppt[0].x - lppt[1].x;              // 产生Bezier段的第三点
    lppt[j].y = 2 * lppt[0].y - lppt[1].y;              // K0 - V0 / 3
    lppt[j+1] = lppt[0];                                // 产生Bezier段的终点

    // 绘图
    bool ret = rawBeginPath();
    if (ret)
    {
        POINT *lppt = &pxpoints.front();
        ret = rawMoveTo(lppt[0].x, lppt[0].y);
        ret = rawPolyBezierTo(lppt + 1, getSize(pxpoints) - 1);
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawBSplines(const GiContext* ctx, int count, 
                              const Point2d* controlPoints, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 4 || controlPoints == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>(3 + (0x2000 - 1) / 3));

    const Box2d extent (count, controlPoints);              // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    int i;
    Point2d pt1, pt2, pt3, pt4, pt;
    double d6 = 1.0 / 6.0;
    vector<POINT> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟整形像素坐标数组
    pxpoints.resize(1 + (count - 3) * 3);
    POINT *lppt = &pxpoints.front();

    // 计算第一个曲线段
    pt1 = controlPoints[0] * matD;
    pt2 = controlPoints[1] * matD;
    pt3 = controlPoints[2] * matD;
    pt4 = controlPoints[3 % count] * matD;
    pt.set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    pt.get(lppt->x, lppt->y); lppt++;
    pt.set((4 * pt2.x + 2 * pt3.x)   *d6, (4 * pt2.y + 2 * pt3.y)   *d6);
    pt.get(lppt->x, lppt->y); lppt++;
    pt.set((2 * pt2.x + 4 * pt3.x)   *d6, (2 * pt2.y + 4 * pt3.y)   *d6);
    pt.get(lppt->x, lppt->y); lppt++;
    pt.set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    pt.get(lppt->x, lppt->y); lppt++;

    // 计算其余曲线段
    for (i = 4; i < count; i++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = controlPoints[i % count] * matD;
        pt.set((4 * pt2.x + 2 * pt3.x)   *d6, (4 * pt2.y + 2 * pt3.y)   *d6);
        pt.get(lppt->x, lppt->y); lppt++;
        pt.set((2 * pt2.x + 4 * pt3.x)   *d6, (2 * pt2.y + 4 * pt3.y)   *d6);
        pt.get(lppt->x, lppt->y); lppt++;
        pt.set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
        pt.get(lppt->x, lppt->y); lppt++;
    }

    // 绘图
    return rawPolyBezier(ctx, &pxpoints.front(), getSize(pxpoints));
}

bool GiGraphics::drawClosedBSplines(const GiContext* ctx, 
                                    int count, 
                                    const Point2d* controlPoints, 
                                    bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 3 || controlPoints == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>((0x2000 - 1) / 3));

    const Box2d extent (count, controlPoints);              // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    int i;
    Point2d pt1, pt2, pt3, pt4, pt;
    double d6 = 1.0 / 6.0;
    vector<POINT> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟整形像素坐标数组
    pxpoints.resize(1 + count * 3);
    POINT *lppt = &pxpoints.front();

    // 计算第一个曲线段
    pt1 = controlPoints[0] * matD;
    pt2 = controlPoints[1] * matD;
    pt3 = controlPoints[2] * matD;
    pt4 = controlPoints[3 % count] * matD;
    pt.set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    pt.get(lppt->x, lppt->y); lppt++;
    pt.set((4 * pt2.x + 2 * pt3.x)   *d6, (4 * pt2.y + 2 * pt3.y)   *d6);
    pt.get(lppt->x, lppt->y); lppt++;
    pt.set((2 * pt2.x + 4 * pt3.x)   *d6, (2 * pt2.y + 4 * pt3.y)   *d6);
    pt.get(lppt->x, lppt->y); lppt++;
    pt.set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    pt.get(lppt->x, lppt->y); lppt++;

    // 计算其余曲线段
    for (i = 4; i < count + 3; i++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = controlPoints[i % count] * matD;
        pt.set((4 * pt2.x + 2 * pt3.x)   *d6, (4 * pt2.y + 2 * pt3.y)   *d6);
        pt.get(lppt->x, lppt->y); lppt++;
        pt.set((2 * pt2.x + 4 * pt3.x)   *d6, (2 * pt2.y + 4 * pt3.y)   *d6);
        pt.get(lppt->x, lppt->y); lppt++;
        pt.set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
        pt.get(lppt->x, lppt->y); lppt++;
    }

    // 绘图
    bool ret = rawBeginPath();
    if (ret)
    {
        POINT *lppt = &pxpoints.front();
        ret = rawMoveTo(lppt[0].x, lppt[0].y);
        ret = rawPolyBezierTo(lppt + 1, getSize(pxpoints) - 1);
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawPath(const GiContext* ctx, int count, 
                          const Point2d* points, const UInt8* types, 
                          bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 
        || points == NULL || types == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    if (count > 0x2000)
        count = 0x2000;

    int i;
    Point2d pt;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                     // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))    // 全部在显示区域外
        return false;

    vector<POINT> pxpoints;
    pxpoints.resize(count);
    POINT *lppt = &pxpoints.front();

    for (i = 0; i < count; i++)
    {
        pt = points[i] * matD;
        pt.get(lppt[i].x, lppt[i].y);
    }

    return rawPolyDraw(ctx, count, lppt, types);
}

_GEOM_END