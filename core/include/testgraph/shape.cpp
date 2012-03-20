#include "shape.h"
#include <gishape.h>
#include <mgbasicsp.h>
#include <stdlib.h>
#include <mgshapest.h>
#include <vector>

MgShapes* createShapes()
{
    return new MgShapesT<std::vector<GiShape*> >;
}

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

void RandomParam::setShapeProp(GiShape* shape)
{
    shape->context()->setLineColor(GiColor(RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(1, 255)));
    shape->context()->setLineWidth((Int16)RandInt(-10, 200));
    shape->context()->setLineStyle((kLineStyle)(randomLineStyle ? RandInt(kLineSolid, kLineNull) : kLineSolid));
}

void RandomParam::initShapes(MgShapes* shapes)
{
    shapes->clear();

    for (long n = getShapeCount(); n > 0; n--)
    {
        int type = RandInt(0, 2);
        GiShape* sp = NULL;
        
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
            GiShapeT<MgSplines> shape;

            shape._shape.resize(RandInt(3, 20));
            sp = shapes->addShape(shape);
            curveCount--;
            
            setShapeProp(sp);
            for (UInt32 i = 0; i < sp->shape()->getPointCount(); i++)
            {
                if (0 == i)
                {
                    sp->shape()->setPoint(i, 
                        Point2d(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0)));
                }
                else
                {
                    sp->shape()->setPoint(i, sp->shape()->getPoint(i-1)
                        + Vector2d(RandDbl(-200.0, 200.0), RandDbl(-200.0, 200.0)));
                }
            }
        }
        else if (1 == type)
        {
            arcCount--;
            /*
            ArcItem* shape = new ArcItem();
            setShapeProp(sp);
            shape->center.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
            shape->rx = RandDbl(1.0, 1000.0);
            shape->ry = RandDbl(1.0, 1000.0);
            shape->startAngle = RandDbl(0.0, _M_2PI);
            shape->sweepAngle = RandDbl(0.0, M_PI_2 * 6);
            */
        }
        else
        {
            lineCount--;
        }

        if (NULL == sp)
        {
            GiShapeT<MgLine> shape;

            sp = shapes->addShape(shape);
            setShapeProp(sp);
            sp->shape()->setPoint(0, Point2d(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0)));
            sp->shape()->setPoint(1, Point2d(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0)));
        }

        sp->shape()->update();
    }
}
