#include "RandomShape.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <stdlib.h>
#include <time.h>

void RandomParam::init()
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        srand((unsigned)time(NULL));
    }
}

float RandomParam::RandF(float dMin, float dMax)
{
    return (rand() % mgRound((dMax - dMin) * 10)) * 0.1f + dMin;
}

long RandomParam::RandInt(long nMin, long nMax)
{
    return rand() % (nMax - nMin + 1) + nMin;
}

unsigned char RandomParam::RandUInt8(long nMin, long nMax)
{
    return (unsigned char)RandInt(nMin, nMax);
}

void RandomParam::setShapeProp(GiContext* context)
{
    context->setLineColor(GiColor(RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(32, 255)));
    context->setLineWidth(RandF(-10, 100), true);
    context->setLineStyle((randomLineStyle ? (GiLineStyle)RandInt(kGiLineSolid, kGiLineDashDotdot) : kGiLineSolid));
    context->setFillColor(GiColor(RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(32, 240)));
}

void RandomParam::initShapes(MgShapes* shapes)
{
    for (long n = getShapeCount(); n > 0; n--)
    {
        int type = RandInt(0, 2);
        MgShape* sp = NULL;
        
        if (0 == type && 0 == lineCount)
            type = 1;
        if (1 == type && 0 == rectCount)
            type = 2;
        if (2 == type && 0 == arcCount)
            type = 3;
        if (3 == type && 0 == curveCount)
            type = 0;
        if (0 == type && 0 == lineCount)
            type = 1;
        
        if (3 == type)
        {
            MgShapeT<MgSplines> shape;

            shape._shape.resize(RandInt(3, 20));
            sp = shapes->addShape(shape);
            curveCount--;
            
            setShapeProp(sp->context());
            for (int i = 0; i < sp->shape()->getPointCount(); i++)
            {
                if (0 == i)
                {
                    sp->shape()->setPoint(i, 
                        Point2d(RandF(-1000, 1000), RandF(-1000, 1000)));
                }
                else
                {
                    sp->shape()->setPoint(i, sp->shape()->getPoint(i-1)
                        + Vector2d(RandF(-200, 200), RandF(-200, 200)));
                }
            }
        }
        else if (2 == type)
        {
            arcCount--;
            /*
            ArcItem* shape = new ArcItem();
            setShapeProp(sp->context());
            shape->center.set(RandDbl(-1000, 1000), RandDbl(-1000, 1000));
            shape->rx = RandDbl(1, 1000);
            shape->ry = RandDbl(1, 1000);
            shape->startAngle = RandDbl(0, _M_2PI);
            shape->sweepAngle = RandDbl(0, _M_PI_2 * 6);
            */
        }
        else if (1 == type)
        {
            MgShapeT<MgRect> shape;
            
            Box2d rect(Point2d(RandF(-1000, 1000), RandF(-1000, 1000)), RandF(1, 200), 0);
            shape._shape.setRect2P(rect.leftTop(), rect.rightBottom());
            sp = shapes->addShape(shape);
            rectCount--;
            
            setShapeProp(sp->context());
        }
        else
        {
            lineCount--;
        }

        if (NULL == sp)
        {
            MgShapeT<MgLine> shape;

            sp = shapes->addShape(shape);
            setShapeProp(sp->context());
            sp->shape()->setPoint(0, Point2d(RandF(-1000, 1000), RandF(-1000, 1000)));
            sp->shape()->setPoint(1, Point2d(RandF(-1000, 1000), RandF(-1000, 1000)));
        }

        sp->shape()->update();
    }
}
