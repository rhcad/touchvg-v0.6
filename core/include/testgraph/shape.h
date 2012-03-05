#ifndef _TEST_SHAPE_H
#define _TEST_SHAPE_H

#include <gigraph.h>

_GEOM_BEGIN

class ShapeItem
{
public:
    GiColor     lineColor;
    Int16       lineWidth;
    kLineStyle  lineStyle;

    virtual void draw(GiGraphics* gs) const = 0;
    virtual Box2d getExtent() const = 0;
};

class LineItem : public ShapeItem
{
public:
    Point2d     startpt;
    Point2d     endpt;

    virtual void draw(GiGraphics* gs) const;
    virtual Box2d getExtent() const;
};

class ArcItem : public ShapeItem
{
public:
    Point2d     center;
    double      rx;
    double      ry;
    double      startAngle;
    double      sweepAngle;

    virtual void draw(GiGraphics* gs) const;
    virtual Box2d getExtent() const;
};

class Shapes
{
public:
    Shapes(int shapeCount);
    ~Shapes();

    long getShapeCount() const;
    ShapeItem* getShape(long index) const;
    Box2d getExtent() const;

    void recalcExtent();
    void setShape(long index, ShapeItem* shape);

    void draw(GiGraphics* gs) const;

private:
    long            m_count;
    ShapeItem**     m_shapes;
    Box2d           m_extent;
};

struct RandomParam
{
    long lineCount;
    long arcCount;
    bool randomLineStyle;

    long getShapeCount() const { return lineCount + arcCount; }
    void initShapes(Shapes* shapes);
    void setShapeProp(ShapeItem* shape);

    static double RandDbl(double dMin, double dMax);
    static long RandInt(long nMin, long nMax);
    static UInt8 RandUInt8(long nMin, long nMax);
};

_GEOM_END
#endif // _TEST_SHAPE_H