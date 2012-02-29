#include "stdafx.h"
#include "shape.h"
#include <mgcurv.h>
#include <mgnear.h>

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
