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
            CurveItem* shape = new CurveItem(RandInt(3, 20));
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

void Shapes::draw(GiGraphics* gs) const
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

// LineItem
//

void LineItem::draw(GiGraphics* gs) const
{
    GiContext context(lineWidth, lineColor, lineStyle);
    gs->drawLine(&context, startpt, endpt);
}

Box2d LineItem::getExtent() const
{
    return Box2d(startpt, endpt);
}

// ArcItem
//

void ArcItem::draw(GiGraphics* gs) const
{
    GiContext context(lineWidth, lineColor, lineStyle);
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

// CurveItem
//

CurveItem::CurveItem() : count(0), points(NULL)
{
}

CurveItem::CurveItem(int n) : count(n), points(new Point2d[n])
{
}

CurveItem::~CurveItem()
{
    if (points)
        delete[] points;
}

void CurveItem::draw(GiGraphics* gs) const
{
    GiContext context(lineWidth, lineColor, lineStyle);
    gs->drawBSplines(&context, count, points);
}

Box2d CurveItem::getExtent() const
{
    return Box2d(count, points);
}
