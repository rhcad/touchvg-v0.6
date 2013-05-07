// gigraph.cpp: 实现图形系统类 GiGraphics
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "gigraph.h"
#include "gigraph_.h"
#include <mglnrel.h>
#include <mgcurv.h>
#include "giplclip.h"

#ifndef SafeCall
#define SafeCall(p, f)      if (p) p->f
#endif

GiGraphics::GiGraphics(GiTransform* xform)
{
    m_impl = new GiGraphicsImpl(xform);
}

GiGraphics::~GiGraphics()
{
    delete m_impl;
}

void GiGraphics::_setCanvas(GiCanvas* canvas)
{
    m_impl->canvas = canvas;
    SafeCall(m_impl->canvas, _init(this, m_impl));
}

void GiGraphics::copy(const GiGraphics& src)
{
    if (this != &src)
    {
        m_impl->maxPenWidth = src.m_impl->maxPenWidth;
        m_impl->antiAlias = src.m_impl->antiAlias;
        m_impl->colorMode = src.m_impl->colorMode;
    }
}

const GiTransform& GiGraphics::xf() const
{
    return *m_impl->xform;
}

GiTransform& GiGraphics::_xf()
{
    return *m_impl->xform;
}

void GiGraphics::_beginPaint(const RECT_2D& clipBox)
{
    if (m_impl->lastZoomTimes != xf().getZoomTimes())
    {
        m_impl->zoomChanged();
        m_impl->lastZoomTimes = xf().getZoomTimes();
    }
    giInterlockedIncrement(&m_impl->drawRefcnt);

    if (!Box2d(clipBox).isEmpty())
    {
        m_impl->clipBox0 = clipBox;
        m_impl->clipBox  = clipBox;
        m_impl->rectDraw = Box2d(clipBox);
        m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
        m_impl->rectDrawM = Box2d(m_impl->rectDraw) * xf().displayToModel();
        m_impl->rectDrawMaxM = Box2d(0, 0, xf().getWidth(), xf().getHeight()) * xf().displayToModel();
        m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
        m_impl->rectDrawMaxW = m_impl->rectDrawMaxM * xf().modelToWorld();
    }
}

void GiGraphics::_endPaint()
{
    giInterlockedDecrement(&m_impl->drawRefcnt);
}

bool GiGraphics::isDrawing() const
{
    return m_impl->drawRefcnt > 0;
}

bool GiGraphics::isPrint() const
{
    return m_impl->isPrint;
}

GiCanvas* GiGraphics::getCanvas()
{
    return m_impl->canvas;
}

Box2d GiGraphics::getClipModel() const
{
    return m_impl->rectDrawM;
}

Box2d GiGraphics::getClipWorld() const
{
    return m_impl->rectDrawW;
}

RECT_2D& GiGraphics::getClipBox(RECT_2D& rc) const
{
    rc = m_impl->clipBox;
    return rc;
}

bool GiGraphics::setClipBox(const RECT_2D& rc)
{
    if (m_impl->drawRefcnt < 1)
        return false;

    bool ret = false;
    Box2d rect;

    if (!rect.intersectWith(Box2d(rc), Box2d(m_impl->clipBox0)).isEmpty())
    {
        if (rect != Box2d(m_impl->clipBox))
        {
            rect.get(m_impl->clipBox);
            m_impl->rectDraw.set(Box2d(rc));
            m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
            m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
            m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
            SafeCall(m_impl->canvas, _clipBoxChanged(m_impl->clipBox));
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

        box.intersectWith(Box2d(m_impl->clipBox0));
        if (!box.isEmpty(Tol(1, 0)))
        {
            if (box != Box2d(m_impl->clipBox))
            {
                box.get(m_impl->clipBox);
                m_impl->rectDraw = box;
                m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
                m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
                m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
                SafeCall(m_impl->canvas, _clipBoxChanged(m_impl->clipBox));
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

bool GiGraphics::setAntiAliasMode(bool antiAlias)
{
    bool old = m_impl->antiAlias;
    m_impl->antiAlias = antiAlias;
    SafeCall(m_impl->canvas, _antiAliasModeChanged(antiAlias));
    return old;
}

GiColorMode GiGraphics::getColorMode() const
{
    return m_impl->colorMode;
}

void GiGraphics::setColorMode(GiColorMode mode)
{
    m_impl->colorMode = mode;
}

GiColor GiGraphics::calcPenColor(const GiColor& color) const
{
    GiColor ret = color;

    if (kGiColorMono == m_impl->colorMode)
    {
        const GiColor& bk = getBkColor();
        if (color != bk)
            ret.set(~bk.r, ~bk.g, ~bk.b);
    }
    if (m_impl->drawColors == 2           // 黑白
        || kGiColorGray == m_impl->colorMode)
    {
        unsigned char by = (unsigned char)(
            (77 * ret.r + 151 * ret.g + 28 * ret.b + 128) / 256);
        ret.set(by, by, by);
    }
    if (m_impl->drawColors > 0 && m_impl->drawColors < 8
        && m_impl->canvas)              // 少于256色
    {
        ret = m_impl->canvas->getNearestColor(ret);
    }

    return ret;
}

float GiGraphics::calcPenWidth(float lineWidth, bool useViewScale) const
{
    float w = 1;

    if (m_impl->maxPenWidth <= 1)
        lineWidth = 0;

    if (lineWidth > 0) {        // 单位：0.01mm
        w = lineWidth / 2540.f * xf().getDpiY();
        w *= xf().getViewScale();
    }
    else if (lineWidth < 0) {   // 单位：像素
        w = -lineWidth;
        if (useViewScale)
            w *= xf().getViewScale();
    }
    w = mgMin(w, m_impl->maxPenWidth);
    w = mgMax(w, m_impl->minPenWidth);
    if (lineWidth <= 0 && xf().getDpiY() > getScreenDpi()) {
        w = w * xf().getDpiY() / getScreenDpi();
    }

    return w;
}

void GiGraphics::setMaxPenWidth(float pixels, float minw)
{
    if (minw < 0)
        minw = m_impl->minPenWidth;
    
    if (pixels < 0)
        pixels = m_impl->maxPenWidth;
    else if (pixels < minw)
        pixels = minw;
    else if (pixels > 200)
        pixels = 200;
    
    m_impl->maxPenWidth = pixels;
    m_impl->minPenWidth = minw;
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

    return rawLine(ctx, pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}

//! 折线绘制辅助类，用于将显示与环境设置分离
class PolylineAux
{
    GiGraphics* m_gs;
    const GiContext* m_pContext;
public:
    PolylineAux(GiGraphics* gs, const GiContext* ctx)
        : m_gs(gs), m_pContext(ctx)
    {
    }
    bool draw(const Point2d* pxs, int n) const
    {
        return pxs && n > 1 && m_gs->rawLines(m_pContext, pxs, n);
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
        vector<Point2d> pxpoints;
        pxpoints.resize(n);
        Point2d* pxs = &pxpoints.front();
        n = 0;
        for (int j = si; j <= ei; j++)
        {
            // 记下第一个点，其他点如果和上一点不重合则记下，否则跳过
            if (j == si || fabsf(pt1.x - pts[j].x) > 2
                || fabsf(pt1.y - pts[j].y) > 2)
            {
                pt1 = pts[j];
                pxs[n++] = pt1;
            }
        }

        return aux.draw(pxs, n);
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
    vector<Point2d> pxpoints;
    vector<Point2d> pointBuf;
    bool ret = false;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).contains(extent))  // 全部在显示区域内
    {
        pxpoints.resize(count);
        Point2d* pxs = &pxpoints.front();
        int n = 0;
        for (i = 0; i < count; i++)
        {
            pt2 = points[i] * matD;
            if (i == 0 || fabsf(pt1.x - pt2.x) > 2 || fabsf(pt1.y - pt2.y) > 2)
            {
                pt1 = pt2;
                pxs[n++] = pt2;
            }
        }
        ret = rawLines(ctx, pxs, n);
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
                             const Point2d* points, bool closed, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 4 || points == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    if (count > 0x2000)
        count = 0x2000;
    count = 1 + (count - 1) / 3 * 3;

    bool ret = false;
    vector<Point2d> pxpoints;
    vector<Point2d> pointBuf;
    int i, j, n, si, ei;
    Point2d * pxs;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;
    
    if (closed) {
        pxpoints.resize(count);
        pxs = &pxpoints.front();
        for (i = 0; i < count; i++)
            pxs[i] = points[i] * matD;
        ret = rawBeginPath();
        if (ret)
        {
            ret = rawMoveTo(pxs[0].x, pxs[0].y);
            for (i = 1; i + 2 < getSize(pxpoints); i += 3) {
                ret = rawBezierTo(pxs[i].x, pxs[i].y,
                    pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
            }
            ret = rawClosePath();
            ret = rawEndPath(ctx, true);
        }
    }
    else if (DRAW_MAXR(m_impl, modelUnit).contains(extent))  // 全部在显示区域内
    {
        pxpoints.resize(count);
        pxs = &pxpoints.front();
        for (i = 0; i < count; i++)
            pxs[i] = points[i] * matD;
        ret = rawBeziers(ctx, pxs, count);
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
                pxs = &pxpoints.front();
                for (j=0; j<n; j++)
                    pxs[j] = pts[si + j];
                ret = rawBeziers(ctx, pxs, n);
            }
            si = ei = i;
        }
    }
    return ret;
}

bool GiGraphics::drawArc(const GiContext* ctx, 
                         const Point2d& center, float rx, float ry, 
                         float startAngle, float sweepAngle, 
                         bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO || fabsf(sweepAngle) < 1e-5f)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (ry < _MGZERO)
        ry = rx;

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(Box2d(center, 2 * rx, 2 * ry)))
        return false;

    Point2d points[16];
    int count = mgAngleArcToBezier(points, center,
        rx, ry, startAngle, sweepAngle);
    S2D(xf(), modelUnit).TransformPoints(count, points);

    return count > 3 && rawBeziers(ctx, points, count);
}

bool GiGraphics::drawArc3P(const GiContext* ctx, const Point2d& startpt, 
                           const Point2d& midpt, const Point2d& endpt, 
                           bool modelUnit)
{
    Point2d center;
    float r, startAngle, sweepAngle;
    
    return (mgArc3P(startpt, midpt, endpt, center, r, &startAngle, &sweepAngle)
            && drawArc(ctx, center, r, r, startAngle, sweepAngle, modelUnit));
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
    vector<Point2d> pxpoints;
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
            Point2d *pxs = &pxpoints.front();
            n = 0;
            for (i = si; i <= ei; i++)
            {
                pt2 = clip.getPoint(i);
                if (i == si || fabsf(pt1.x - pt2.x) > 2
                    || fabsf(pt1.y - pt2.y) > 2)
                {
                    pt1 = pt2;
                    pxs[n++] = pt1;
                }
            }

            ret = aux.draw(pxs, n) || ret;
        }
    }

    return ret;
}

static bool _DrawPolygon(GiCanvas* cv, const GiContext* ctx, 
                         int count, const Point2d* points, 
                         bool bM2D, bool bFill, bool bEdge, bool modelUnit)
{
    if (!ctx && cv)
        ctx = cv->getCurrentContext();
    if (!ctx || !cv)
        return false;
    
    GiContext context (*ctx);
    if (!bEdge)
        context.setNullLine();
    if (!bFill)
        context.setNoFillColor();
    
    if (context.isNullLine() && !context.hasFillColor())
        return false;

    vector<Point2d> pxpoints;
    Point2d pt1, pt2;
    Matrix2d matD(S2D(cv->gs()->xf(), modelUnit));

    pxpoints.resize(count);
    Point2d *pxs = &pxpoints.front();
    int n = 0;
    for (int i = 0; i < count; i++)
    {
        pt2 = points[i];
        if (bM2D)
            pt2 *= matD;
        if (i == 0 || fabsf(pt1.x - pt2.x) > 2
            || fabsf(pt1.y - pt2.y) > 2)
        {
            pt1 = pt2;
            pxs[n++] = pt1;
        }
    }

    if (n == 4 && mgEquals(pxs[0].x, pxs[3].x) && mgEquals(pxs[1].x, pxs[2].x)
        && mgEquals(pxs[0].y, pxs[1].y) && mgEquals(pxs[2].y, pxs[3].y))
    {
        return cv->rawRect(&context, pxs[0].x, pxs[0].y, 
            pxs[2].x - pxs[0].x, pxs[2].y - pxs[0].y);
    }

    return cv->rawPolygon(&context, pxs, n);
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

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).contains(extent))  // 全部在显示区域内
    {
        ret = _DrawPolygon(m_impl->canvas, ctx, 
            count, points, true, true, true, modelUnit);
    }
    else                                                // 部分在显示区域内
    {
        PolygonClip clip (m_impl->rectDraw);
        if (!clip.clip(count, points, &S2D(xf(), modelUnit)))  // 多边形剪裁
            return false;
        count = clip.getCount();
        points = clip.getPoints();

        ret = _DrawPolygon(m_impl->canvas, ctx, 
            count, points, false, true, false, modelUnit);

        int ienter = findInvisibleEdge(clip);
        if (ienter == count)
        {
            ret = _DrawPolygon(m_impl->canvas, ctx, count, points, 
                false, false, true, modelUnit) || ret;
        }
        else
        {
            ret = drawPolygonEdge(PolylineAux(this, ctx), count, clip, ienter) || ret;
        }
    }

    return ret;
}

bool GiGraphics::drawEllipse(const GiContext* ctx, const Box2d& rect, bool modelUnit)
{
    return drawEllipse(ctx, rect.center(), rect.width() / 2, rect.height() / 2, modelUnit);
}

bool GiGraphics::drawEllipse(const GiContext* ctx, const Point2d& center, 
                             float rx, float ry, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    bool ret = false;
    Matrix2d matD(S2D(xf(), modelUnit));

    if (ry < _MGZERO) {
        ry = (Vector2d(rx, rx) * matD).x;
        ry = fabsf((Vector2d(ry, ry) * matD.inverse()).y);
    }

    const Box2d extent (center, rx*2.f, ry*2.f);            // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    if (mgIsZero(matD.m12) && mgIsZero(matD.m21))
    {
        Point2d cen (center * matD);
        rx *= fabsf(matD.m11);
        ry *= fabsf(matD.m22);

        ret = rawEllipse(ctx, cen.x - rx, cen.y - ry, 2 * rx, 2 * ry);
    }
    else
    {
        Point2d pxs[13];
        mgEllipseToBezier(pxs, center, rx, ry);
        matD.TransformPoints(13, pxs);

        ret = rawBeginPath();
        if (ret)
        {
            ret = rawMoveTo(pxs[0].x, pxs[0].y);
            for (int i = 1; i + 2 < 13; i += 3) {
                ret = rawBezierTo(pxs[i].x, pxs[i].y,
                    pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
            }
            ret = rawClosePath();
            ret = rawEndPath(ctx, true);
        }
    }

    return ret;
}

bool GiGraphics::drawPie(const GiContext* ctx, 
                         const Point2d& center, float rx, float ry, 
                         float startAngle, float sweepAngle, 
                         bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO || fabsf(sweepAngle) < 1e-5f)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (ry < _MGZERO)
        ry = rx;

    const Box2d extent (center, rx*2.f, ry*2.f);            // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    Point2d pxs[16];
    int count = mgAngleArcToBezier(pxs, center,
        rx, ry, startAngle, sweepAngle);
    if (count < 4)
        return false;
    S2D(xf(), modelUnit).TransformPoints(count, pxs);
    Point2d cen(center * S2D(xf(), modelUnit));

    bool ret = rawBeginPath();
    if (ret)
    {
        ret = rawMoveTo(cen.x, cen.y);
        ret = rawLineTo(pxs[0].x, pxs[0].y);
        for (int i = 1; i + 2 < count; i += 3) {
            ret = rawBezierTo(pxs[i].x, pxs[i].y,
                pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
        }
        ret = rawClosePath();
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
    return !rect.isEmpty() && drawPolygon(ctx, 4, points, modelUnit);
}

bool GiGraphics::drawRoundRect(const GiContext* ctx, 
                               const Box2d& rect, float rx, float ry, 
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
        Point2d pxs[16];

        mgRoundRectToBeziers(pxs, rect, rx, ry);
        S2D(xf(), modelUnit).TransformPoints(16, pxs);

        ret = rawBeginPath();
        if (ret)
        {
            ret = rawMoveTo(pxs[0].x, pxs[0].y);
            ret = rawBezierTo(pxs[1].x, pxs[1].y, pxs[2].x, pxs[2].y, pxs[3].x, pxs[3].y);

            ret = rawLineTo(pxs[4].x, pxs[4].y);
            ret = rawBezierTo(pxs[5].x, pxs[5].y, pxs[6].x, pxs[6].y, pxs[7].x, pxs[7].y);

            ret = rawLineTo(pxs[8].x, pxs[8].y);
            ret = rawBezierTo(pxs[9].x, pxs[9].y, pxs[10].x, pxs[10].y, pxs[11].x, pxs[11].y);

            ret = rawLineTo(pxs[12].x, pxs[12].y);
            ret = rawBezierTo(pxs[13].x, pxs[13].y, pxs[14].x, pxs[14].y, pxs[15].x, pxs[15].y);

            ret = rawClosePath();
            ret = rawEndPath(ctx, true);
        }
    }

    return ret;
}

bool GiGraphics::drawSplines(const GiContext* ctx, int count, 
                             const Point2d* knots, 
                             const Vector2d* knotvs, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 
        || knots == NULL || knotvs == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>(1 + (0x2000 - 1) / 3));

    int i;
    Point2d pt;
    Vector2d vec;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + (count - 1) * 3);
    Point2d *pxs = &pxpoints.front();

    pt = knots[0] * matD;                       // 第一个Bezier段的起点
    vec = knotvs[0] * matD / 3.f;               // 第一个Bezier段的起始矢量
    *pxs++ = pt;                                // 产生Bezier段的起点
    for (i = 1; i < count; i++)                 // 计算每一个Bezier段
    {
        *pxs++ = (pt += vec);                   // 产生Bezier段的第二点
        pt = knots[i] * matD;                   // Bezier段的终点
        vec = knotvs[i] * matD / 3.f;           // Bezier段的终止矢量
        *pxs++ = pt - vec;                      // 产生Bezier段的第三点
        *pxs++ = pt;                            // 产生Bezier段的终点
    }

    // 绘图
    return rawBeziers(ctx, &pxpoints.front(), getSize(pxpoints));
}

bool GiGraphics::drawClosedSplines(const GiContext* ctx, int count, 
                                   const Point2d* knots, 
                                   const Vector2d* knotvs, 
                                   bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || 
        knots == NULL || knotvs == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>((0x2000 - 1) / 3));

    int i, j = 0;
    Point2d pt;
    Vector2d vec;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + count * 3);
    Point2d *pxs = &pxpoints.front();

    pt = knots[0] * matD;                       // 第一个Bezier段的起点
    vec = knotvs[0] * matD / 3.f;               // 第一个Bezier段的起始矢量
    pxs[j++] = pt;                              // 产生Bezier段的起点
    for (i = 1; i < count; i++)                 // 计算每一个Bezier段
    {
        pxs[j++] = (pt += vec);                 // 产生Bezier段的第二点
        pt = knots[i] * matD;                   // Bezier段的终点
        vec = knotvs[i] * matD / 3.f;           // Bezier段的终止矢量
        pxs[j++] = pt - vec;                    // 产生Bezier段的第三点
        pxs[j++] = pt;                          // 产生Bezier段的终点
    }
    pxs[j++] = (pt += vec);                     // 产生Bezier段的第二点
    pxs[j]   = 2 * pxs[0] - pxs[1].asVector();  // 产生Bezier段的第三点
    pxs[j+1] = pxs[0];                          // 产生Bezier段的终点

    // 绘图
    bool ret = rawBeginPath();
    if (ret)
    {
        ret = rawMoveTo(pxs[0].x, pxs[0].y);
        for (i = 1; i + 2 < getSize(pxpoints); i += 3) {
            ret = rawBezierTo(pxs[i].x, pxs[i].y,
                pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
        }
        ret = rawClosePath();
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawBSplines(const GiContext* ctx, int count, 
                              const Point2d* ctlpts, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 4 || ctlpts == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>(3 + (0x2000 - 1) / 3));

    const Box2d extent (count, ctlpts);              // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    int i;
    Point2d pt1, pt2, pt3, pt4;
    float d6 = 1.f / 6.f;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + (count - 3) * 3);
    Point2d *pxs = &pxpoints.front();

    // 计算第一个曲线段
    pt1 = ctlpts[0] * matD;
    pt2 = ctlpts[1] * matD;
    pt3 = ctlpts[2] * matD;
    pt4 = ctlpts[3 % count] * matD;
    (*pxs++).set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6,  (4 * pt2.y + 2 * pt3.y)   *d6);
    (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6,  (2 * pt2.y + 4 * pt3.y)   *d6);
    (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);

    // 计算其余曲线段
    for (i = 4; i < count; i++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = ctlpts[i % count] * matD;
        (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)   *d6);
        (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)   *d6);
        (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6,(pt2.y + 4 * pt3.y + pt4.y)*d6);
    }

    // 绘图
    return rawBeziers(ctx, &pxpoints.front(), getSize(pxpoints));
}

bool GiGraphics::drawClosedBSplines(const GiContext* ctx, 
                                    int count, 
                                    const Point2d* ctlpts, 
                                    bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 3 || ctlpts == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>((0x2000 - 1) / 3));

    const Box2d extent (count, ctlpts);              // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    int i;
    Point2d pt1, pt2, pt3, pt4;
    float d6 = 1.f / 6.f;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + count * 3);
    Point2d *pxs = &pxpoints.front();

    // 计算第一个曲线段
    pt1 = ctlpts[0] * matD;
    pt2 = ctlpts[1] * matD;
    pt3 = ctlpts[2] * matD;
    pt4 = ctlpts[3 % count] * matD;
    (*pxs++).set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
    (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
    (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);

    // 计算其余曲线段
    for (i = 4; i < count + 3; i++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = ctlpts[i % count] * matD;
        (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
        (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
        (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    }

    // 绘图
    bool ret = rawBeginPath();
    if (ret)
    {
        pxs = &pxpoints.front();
        ret = rawMoveTo(pxs[0].x, pxs[0].y);
        for (i = 1; i + 2 < getSize(pxpoints); i += 3) {
            ret = rawBezierTo(pxs[i].x, pxs[i].y,
                pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
        }
        ret = rawClosePath();
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

void GiGraphics::clearCachedBitmap(bool clearAll)
{
    SafeCall(m_impl->canvas, clearCachedBitmap(clearAll));
}

GiColor GiGraphics::getBkColor() const
{
    return m_impl->canvas ? m_impl->canvas->getBkColor() : GiColor::Invalid();
}

GiColor GiGraphics::setBkColor(const GiColor& color)
{
    GiColor colorNoAlhpa(color.r, color.g, color.b);
    return m_impl->canvas ? m_impl->canvas->setBkColor(colorNoAlhpa) : color;
}

float GiGraphics::getScreenDpi() const
{
    return m_impl->canvas ? m_impl->canvas->getScreenDpi() : 96;
}

bool GiGraphics::rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2)
{
    return m_impl->canvas && m_impl->canvas->rawLine(ctx, x1, y1, x2, y2);
}

bool GiGraphics::rawLines(const GiContext* ctx, const Point2d* pxs, int count)
{
    return m_impl->canvas && m_impl->canvas->rawLines(ctx, pxs, count);
}

bool GiGraphics::rawBeziers(const GiContext* ctx, const Point2d* pxs, int count)
{
    return m_impl->canvas && m_impl->canvas->rawBeziers(ctx, pxs, count);
}

bool GiGraphics::rawPolygon(const GiContext* ctx, const Point2d* pxs, int count)
{
    return m_impl->canvas && m_impl->canvas->rawPolygon(ctx, pxs, count);
}

bool GiGraphics::rawRect(const GiContext* ctx, float x, float y, float w, float h)
{
    return m_impl->canvas && m_impl->canvas->rawRect(ctx, x, y, w, h);
}

bool GiGraphics::rawEllipse(const GiContext* ctx, float x, float y, float w, float h)
{
    return m_impl->canvas && m_impl->canvas->rawEllipse(ctx, x, y, w, h);
}

bool GiGraphics::rawBeginPath()
{
    return m_impl->canvas && m_impl->canvas->rawBeginPath();
}

bool GiGraphics::rawEndPath(const GiContext* ctx, bool fill)
{
    return m_impl->canvas && m_impl->canvas->rawEndPath(ctx, fill);
}

bool GiGraphics::rawMoveTo(float x, float y)
{
    return m_impl->canvas && m_impl->canvas->rawMoveTo(x, y);
}

bool GiGraphics::rawLineTo(float x, float y)
{
    return m_impl->canvas && m_impl->canvas->rawLineTo(x, y);
}

bool GiGraphics::rawBezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    return m_impl->canvas && m_impl->canvas->rawBezierTo(c1x, c1y, c2x, c2y, x, y);
}

bool GiGraphics::rawClosePath()
{
    return m_impl->canvas && m_impl->canvas->rawClosePath();
}

void GiGraphics::rawTextCenter(const char* text, float x, float y, float h)
{
    m_impl->canvas->rawTextCenter(text, x, y, h);
}

bool GiGraphics::drawImage(const char* name, float xc, float yc, float w, float h, float angle)
{
    return m_impl->canvas->drawImage(name, xc, yc, w, h, angle);
}
