#ifndef _TEST_RANDOMSHAPE_H
#define _TEST_RANDOMSHAPE_H

#include <mgshapes.h>

struct RandomParam
{
    long lineCount;
    long arcCount;
    long curveCount;
    bool randomLineStyle;
    
    RandomParam() : lineCount(10), arcCount(10), curveCount(10), randomLineStyle(false) {}

    long getShapeCount() const { return lineCount + arcCount + curveCount; }
    void initShapes(MgShapes* shapes);
    void setShapeProp(MgShape* shape);

    static double RandDbl(double dMin, double dMax);
    static long RandInt(long nMin, long nMax);
    static UInt8 RandUInt8(long nMin, long nMax);
};

#endif // _TEST_RANDOMSHAPE_H
