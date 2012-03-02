#ifndef _TEST_SHAPE_H
#define _TEST_SHAPE_H

struct ShapeItem
{
	GiColor       lineColor;
	Int16		lineWidth;
	UInt8		lineAlpha;
	kLineStyle  lineStyle;

	virtual void draw(GiGraphics* gs) const = 0;
	virtual Box2d getExtent() const = 0;
};

struct LineItem : public ShapeItem
{
	Point2d		startpt;
	Point2d		endpt;

	virtual void draw(GiGraphics* gs) const;
	virtual Box2d getExtent() const;
};

struct ArcItem : public ShapeItem
{
	Point2d		center;
	double		rx;
	double		ry;
	double		startAngle;
	double		sweepAngle;

	virtual void draw(GiGraphics* gs) const;
	virtual Box2d getExtent() const;
};

#endif // _TEST_SHAPE_H