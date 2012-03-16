#ifndef _TEST_SHAPE_H
#define _TEST_SHAPE_H

#include <gigraph.h>

class ShapeItem
{
public:
    GiColor     lineColor;
    Int16       lineWidth;
    kLineStyle  lineStyle;

    ShapeItem() {}
    virtual ~ShapeItem() {}
    
    virtual void draw(GiGraphics* gs, const GiContext *ctx = NULL) const = 0;
    virtual Box2d getExtent() const = 0;
};

class LineItem : public ShapeItem
{
public:
    Point2d     startpt;
    Point2d     endpt;

    virtual void draw(GiGraphics* gs, const GiContext *ctx = NULL) const;
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

    virtual void draw(GiGraphics* gs, const GiContext *ctx = NULL) const;
    virtual Box2d getExtent() const;
};

enum CurveType {
    kBSplines,
    kCubicSplines,
};

class CurveItem : public ShapeItem
{
public:
    CurveType   curveType;
    Int32       count;
    Point2d*    points;
    Vector2d*   knotVectors;
    
    CurveItem(CurveType curveType = kBSplines);
    CurveItem(int n, CurveType curveType);
    virtual ~CurveItem();
    
    void applyPoints();
    virtual void draw(GiGraphics* gs, const GiContext *ctx = NULL) const;
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

    void draw(GiGraphics* gs, const GiContext *ctx = NULL) const;

private:
    long            m_count;
    ShapeItem**     m_shapes;
    Box2d           m_extent;
};

struct RandomParam
{
    long lineCount;
    long arcCount;
    long curveCount;
    bool randomLineStyle;
    
    RandomParam() : lineCount(10), arcCount(10), curveCount(10), randomLineStyle(false) {}

    long getShapeCount() const { return lineCount + arcCount + curveCount; }
    void initShapes(Shapes* shapes);
    void setShapeProp(ShapeItem* shape);

    static double RandDbl(double dMin, double dMax);
    static long RandInt(long nMin, long nMax);
    static UInt8 RandUInt8(long nMin, long nMax);
};

#endif // _TEST_SHAPE_H