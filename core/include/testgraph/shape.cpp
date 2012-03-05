#include "shape.h"
#include <mgcurv.h>
#include <mgnear.h>
#include <stdlib.h>

_GEOM_BEGIN

double RandomParam::RandDbl(double dMin, double dMax)
{
	return (double)rand() / (RAND_MAX + 1) * (dMax - dMin) + dMin;
}

long RandomParam::RandInt(long nMin, long nMax)
{
	return rand() * (nMax - nMin) / (RAND_MAX + 1) + nMin;
}

UInt8 RandomParam::RandUInt8(UInt8 nMin, UInt8 nMax)
{
	return (UInt8)RandInt(nMin, nMax);
}

void RandomParam::setShapeProp(ShapeItem* shape)
{
	shape->lineColor.set(RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(0, 255));
	shape->lineWidth = (Int16)RandInt(-10, 500);
	shape->lineAlpha = (UInt8)RandInt(0, 255);
	shape->lineStyle = (kLineStyle)(randomLineStyle ? RandInt(kLineSolid, kLineNull) : kLineSolid);
}

void RandomParam::initShapes(Shapes* shapes)
{
    for (int i = 0; i < shapes->getShapeCount(); i++)
	{
		bool bLine = (RandInt(0, 100) % 2 == 0);
		if (bLine && lineCount > 0 || 0 == arcCount)
		{
			LineItem* shape = new LineItem();
			shapes->setShape(i, shape);
			lineCount--;
			
			setShapeProp(shape);
			shape->startpt.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
			shape->endpt.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
		}
		else
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
    for (int i = 0; i < m_count; i++)
	{
		m_shapes[i]->draw(gs);
	}
	GiContext context(0, GiColor(128, 128, 128), kLineDot);
	context.setLineAlpha(150);
	gs->drawRect(&context, m_extent);
}

// LineItem
//

void LineItem::draw(GiGraphics* gs) const
{
	GiContext context(lineWidth, lineColor, lineStyle);
	context.setLineAlpha(lineAlpha);
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
	context.setLineAlpha(lineAlpha);
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

_GEOM_END