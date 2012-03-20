#include "shape.h"
#include <mgbasicsp.h>
#include <stdlib.h>

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
    shape->context.setLineColor(GiColor(RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(0, 255), RandUInt8(1, 255)));
    shape->context.setLineWidth((Int16)RandInt(-10, 200));
    shape->context.setLineStyle((kLineStyle)(randomLineStyle ? RandInt(kLineSolid, kLineNull) : kLineSolid));
}

void RandomParam::initShapes(Shapes* shapes)
{
    for (int i = 0; i < shapes->getShapeCount(); i++)
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
            GiShapeT<MgSplines> *shape = new GiShapeT<MgSplines>;

            shape->shape.resize(RandInt(3, 20));
            shapes->setShape(i, shape);
            sp = shape;
            curveCount--;
            
            setShapeProp(shape);
            for (UInt32 i = 0; i < shape->getPointCount(); i++)
            {
                if (0 == i)
                {
                    shape->setPoint(i, 
                        Point2d(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0)));
                }
                else
                {
                    shape->setPoint(i, shape->getPoint(i-1)
                        + Vector2d(RandDbl(-200.0, 200.0), RandDbl(-200.0, 200.0)));
                }
            }
        }
        else if (1 == type)
        {
            arcCount--;
            /*
            ArcItem* shape = new ArcItem();
            shapes->setShape(i, shape);
            sp = shape;
            
            setShapeProp(shape);
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
            GiShapeT<MgLine> *shape = new GiShapeT<MgLine>;

            shapes->setShape(i, shape);
            sp = shape;
            
            setShapeProp(shape);
            shape->setPoint(0, Point2d(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0)));
            shape->setPoint(1, Point2d(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0)));
        }

        sp->update();
    }

    shapes->update();
}

// Shapes
//

Shapes::Shapes(int shapeCount)
    : m_count(0), m_shapes(NULL)
{
    if (shapeCount > 0)
    {
        m_count = shapeCount;
        m_shapes = new GiShape*[shapeCount];
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

GiShape* Shapes::getShape(long index) const
{
    return index >= 0 && index < m_count ? m_shapes[index] : NULL;
}

Box2d Shapes::getExtent() const
{
    return m_extent;
}

void Shapes::update()
{
    m_extent.empty();
    for (int i = 0; i < m_count; i++)
    {
        m_extent.unionWith(m_shapes[i]->getExtent());
    }
}

void Shapes::setShape(long index, GiShape* shape)
{
    if (index >= 0 && index < m_count
        && shape != NULL && shape != m_shapes[index])
    {
        delete m_shapes[index];
        m_shapes[index] = shape;
    }
}

void Shapes::draw(GiGraphics* gs, const GiContext *ctx) const
{
    Box2d clip(gs->getClipModel());
    
    for (int i = 0; i < m_count; i++)
    {
        if (m_shapes[i]->getExtent().isIntersect(clip)) {
            m_shapes[i]->draw(*gs, ctx);
        }
    }
    
    GiContext context(0, GiColor(128, 128, 128, 150), kLineDot);
    gs->drawRect(&context, m_extent);
}
