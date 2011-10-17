#include "shape.h"
#include <mgcurv.h>
#include <mgnear.h>
#include <stdlib.h>

double RandomParam::RandDbl(double dMin, double dMax)
{
    return (rand() % mgRound((dMax - dMin) * 10)) * 0.1 + dMin;
}

long RandomParam::RandInt(long nMin, long nMax)
{
    return rand() % (nMax - nMin + 1) + nMin;
}

UInt8 RandomParam::RandUInt8(long nMin, long nMax)
{
    return (UInt8)RandInt(nMin, nMax);
}

void RandomParam::setShapeProp(ShapeItem* shape)
{
    shape->lineColor.set(RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(1, 255));
    shape->lineWidth = (Int16)RandInt(-10, 200);
    shape->lineStyle = (kLineStyle)(randomLineStyle ? RandInt(kLineSolid, kLineNull) : kLineSolid);
}

void RandomParam::initShapes(Shapes* shapes)
{
    for (int i = 0; i < shapes->getShapeCount(); i++)
    {
        int type = RandInt(0, 2);
        
        if (0 == type && 0 == lineCount)
            type = 1;
        if (1 == type && 0 == arcCount)
            type = 2;
        if (2 == type && 0 == curveCount)
            type = 0;
        if (0 == type && 0 == lineCount)
            type = 1;
        
        if (2 == type)
        {
            CurveType curveType = (CurveType)RandInt(0, kCubicSplines);
            CurveItem* shape = new CurveItem(RandInt(3, 20), curveType);
            shapes->setShape(i, shape);
            curveCount--;
            
            setShapeProp(shape);
            for (int i = 0; i < shape->count; i++)
            {
                if (0 == i)
                {
                    shape->points[i].set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
                }
                else
                {
                    shape->points[i] = shape->points[i-1] + Vector2d(RandDbl(-200.0, 200.0), RandDbl(-200.0, 200.0));
                }
            }
            shape->applyPoints();
        }
        else if (1 == type)
        {
            ArcItem* shape = new ArcItem();
            shapes->setShape(i, shape);
            arcCount--;
            
            setShapeProp(shape);
            shape->center.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
            shape->rx = RandDbl(1.0, 1000.0);
            shape->ry = RandDbl(1.0, 1000.0);
            shape->startAngle = RandDbl(0.0, _M_2PI);
            shape->sweepAngle = RandDbl(0.0, M_PI_2 * 6);
        }
        else
        {
            LineItem* shape = new LineItem();
            shapes->setShape(i, shape);
            lineCount--;
            
            setShapeProp(shape);
            shape->startpt.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
            shape->endpt.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
        }
    }

    shapes->recalcExtent();
}

// Shapes
//

Shapes::Shapes(int shapeCount)
    : m_count(0), m_shapes(NULL)
{
    if (shapeCount > 0)
    {
        m_count = shapeCount;
        m_shapes = new ShapeItem*[shapeCount];
        for (int i=0; i<shapeCount; i++)
            m_shapes[i] = NULL;
    }
}

Shapes::~Shapes()
{
    for (int i = 0; i < m_count; i++)
        delete m_shapes[i];
    delete[] m_shapes;
}

long Shapes::getShapeCount() const
{
    return m_count;
}

ShapeItem* Shapes::getShape(long index) const
{
    return index >= 0 && index < m_count ? m_shapes[index] : NULL;
}

Box2d Shapes::getExtent() const
{
    return m_extent;
}

void Shapes::recalcExtent()
{
    m_extent.empty();
    for (int i = 0; i < m_count; i++)
    {
        m_extent.unionWith(m_shapes[i]->getExtent());
    }
}

void Shapes::setShape(long index, ShapeItem* shape)
{
    if (index >= 0 && index < m_count
        && shape != NULL && shape != m_shapes[index])
    {
        delete m_shapes[index];
        m_shapes[index] = shape;
    }
}

void Shapes::draw(GiGraphics* gs, const GiContext *ctx) const
{
    Box2d clip(gs->getClipModel());
    
    for (int i = 0; i < m_count; i++)
    {
        if (m_shapes[i]->getExtent().isIntersect(clip)) {
            m_shapes[i]->draw(gs);
        }
    }
    
    GiContext context(0, GiColor(128, 128, 128, 150), kLineDot);
    gs->drawRect(&context, m_extent);
}

static inline Int16 useLineWidth(Int16 width, const GiContext *ctx, GiGraphics* gs)
{
    if (ctx && !ctx->isNullLine()) {
        if (width > 0)
            width = -gs->calcPenWidth(width);
        width += ctx->getLineWidth();
    }
    return width;
}

static inline GiColor useLineColor(const GiColor& color, const GiContext *ctx)
{
    if (ctx && !ctx->isNullLine())
        return ctx->getLineColor();
    return color;
}

static inline kLineStyle useLineStyle(kLineStyle lineStyle, const GiContext *ctx)
{
    if (ctx && !ctx->isNullLine())
        return ctx->getLineStyle();
    return lineStyle;
}

static inline GiColor useFillColor(const GiColor& color, const GiContext *ctx)
{
    if (ctx && ctx->hasFillColor())
        return ctx->getFillColor();
    return color;
}

// LineItem
//

void LineItem::draw(GiGraphics* gs, const GiContext *ctx) const
{
    GiContext context(useLineWidth(lineWidth,ctx,gs), useLineColor(lineColor,ctx), useLineStyle(lineStyle, ctx));
    gs->drawLine(&context, startpt, endpt);
}

Box2d LineItem::getExtent() const
{
    return Box2d(startpt, endpt);
}

void LineItem::hitTest(const Box2d& limits, double& dist)
{
    Point2d pts[] = { startpt, endpt };
    Point2d ptNear;
    Int32 segment;
    
    dist = mgLinesHit(2, pts, false, limits.center(), limits.width(), ptNear, segment);
}

// ArcItem
//

void ArcItem::draw(GiGraphics* gs, const GiContext *ctx) const
{
    GiContext context(useLineWidth(lineWidth,ctx,gs), useLineColor(lineColor,ctx), useLineStyle(lineStyle, ctx));
    gs->drawArc(&context, center, 
        rx, ry, startAngle, sweepAngle);
}

Box2d ArcItem::getExtent() const
{
    Point2d points[16];
    int count = mgAngleArcToBezier(points, center, rx, ry, startAngle, sweepAngle);

    Box2d rect;
    mgBeziersBox(rect, count, points);

    return rect;
}

void ArcItem::hitTest(const Box2d& limits, double& dist)
{
    Point2d points[16];
    int count = mgAngleArcToBezier(points, center, rx, ry, startAngle, sweepAngle);
    
    Point2d ptNear;
    
    for (int i = 0; i + 3 < count; i += 3) {
        mgNearestOnBezier(limits.center(), points + i, ptNear);
        double d = ptNear.distanceTo(limits.center());
        if (dist > d) {
            dist = d;
        }
    }
}

// CurveItem
//

CurveItem::CurveItem(CurveType type)
    : curveType(type), count(0), points(NULL), knotVectors(NULL), bzpts(NULL)
{
}

CurveItem::CurveItem(int n, CurveType type)
    : curveType(type), count(n), points(new Point2d[n]), knotVectors(NULL), bzpts(NULL)
{
}

CurveItem::~CurveItem()
{
    if (points)
        delete[] points;
    if (knotVectors)
        delete[] knotVectors;
    if (bzpts)
        delete[] bzpts;
}

void CurveItem::applyPoints()
{
    if (kCubicSplines == curveType) {
        if (!knotVectors)
            knotVectors = new Vector2d[count];
        mgCubicSplines(count, points, knotVectors);
    }
    else if (kBSplines == curveType) {
        if (!bzpts)
            bzpts = new Point2d[1 + count * 3];
        bzcount = mgBSplinesToBeziers(bzpts, count, points, false);
    }
}

void CurveItem::draw(GiGraphics* gs, const GiContext *ctx) const
{
    GiContext context(useLineWidth(lineWidth,ctx,gs), useLineColor(lineColor,ctx), useLineStyle(lineStyle, ctx));
    //GiContext ctxaux(0, GiColor(128,128,128,64), kLineDash);
    
    //gs->drawLines(&ctxaux, count, points);
    
    if (knotVectors) {
        gs->drawSplines(&context, count, points, knotVectors);
        /*ctxaux.setLineColor(GiColor(0,0,128,32));
        ctxaux.setFillColor(GiColor(128,128,128,32));
        for (int i = 0; i < count; i++) {
            gs->drawLine(&ctxaux, points[i], points[i] + knotVectors[i]);
            gs->drawEllipse(&ctxaux, points[i] + knotVectors[i], 1);
        }*/
    }
    else {
        gs->drawBSplines(&context, count, points);
    }
}

Box2d CurveItem::getExtent() const
{
    Box2d rect;
    
    if (knotVectors) {
        mgCubicSplinesBox(rect, count, points, knotVectors);
    }
    else if (bzpts) {
        mgBeziersBox(rect, bzcount, bzpts);
    }
    
    return rect;
}

void CurveItem::hitTest(const Box2d& limits, double& dist)
{
    Point2d ptNear;
    Int32 segment;
    
    if (bzpts) {
        for (int i = 0; i + 3 < bzcount; i += 3) {
            mgNearestOnBezier(limits.center(), bzpts + i, ptNear);
            double d = ptNear.distanceTo(limits.center());
            if (dist > d) {
                dist = d;
            }
        }
    }
    else if (knotVectors) {
        dist = mgCubicSplinesHit(count, points, knotVectors, false,
                                 limits.center(), limits.width(), ptNear, segment);
    }
}
