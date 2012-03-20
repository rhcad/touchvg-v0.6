#ifndef _TEST_SHAPE_H
#define _TEST_SHAPE_H

#include <gishape.h>

class Shapes
{
public:
    Shapes(int shapeCount);
    ~Shapes();

    long getShapeCount() const;
    GiShape* getShape(long index) const;
    Box2d getExtent() const;

    void update();
    void setShape(long index, GiShape* shape);

    void draw(GiGraphics* gs, const GiContext *ctx = NULL) const;

private:
    long            m_count;
    GiShape**       m_shapes;
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
    void setShapeProp(GiShape* shape);

    static double RandDbl(double dMin, double dMax);
    static long RandInt(long nMin, long nMax);
    static UInt8 RandUInt8(long nMin, long nMax);
};

#endif // _TEST_SHAPE_H
