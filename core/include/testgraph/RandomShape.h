#ifndef _TEST_RANDOMSHAPE_H
#define _TEST_RANDOMSHAPE_H

#include <mgshapes.h>
#include <mgshape.h>

struct RandomParam
{
    long lineCount;
    long rectCount;
    long arcCount;
    long curveCount;
    bool randomLineStyle;
    
    static void init();

    RandomParam() : lineCount(10), rectCount(0), arcCount(10), curveCount(10), randomLineStyle(true) {}

    long getShapeCount() const { return lineCount + rectCount + arcCount + curveCount; }
    void initShapes(MgShapes* shapes);
    void setShapeProp(GiContext* context);

    static float RandF(float dMin, float dMax);
    static long RandInt(long nMin, long nMax);
    static UInt8 RandUInt8(long nMin, long nMax);
};

#endif // _TEST_RANDOMSHAPE_H
