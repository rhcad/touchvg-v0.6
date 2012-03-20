//! \file gishape.h
//! \brief 定义矢量图形模板类 GiShapeT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_GISHAPE_H_
#define __GEOMETRY_GISHAPE_H_

#include <gigraph.h>
#include <mgshape.h>

struct GiShape : MgShape
{
    GiContext   context;

    virtual bool draw(GiGraphics& gs, const GiContext *ctxsel = NULL) const = 0;
};

template <class Shape>
class GiShapeT : public GiShape
{
public:
    Shape       shape;

    GiShapeT()
    {
    }

    MgShape* p()
    {
        return &shape;
    }

    const MgShape* p() const
    {
        return &shape;
    }

    bool draw(GiGraphics& gs, const GiContext *ctxsel = NULL) const
    {
        GiContext ctx(getContext(gs, ctxsel));
        return draw(gs, ctx);
    }

    GiContext getContext(GiGraphics& gs, const GiContext *ctxsel) const
    {
        GiContext ctx(context);

        if (ctxsel && !ctxsel->isNullLine()) {
            Int16 width = ctx.getLineWidth();
            if (width > 0)
                width = - (Int16)gs.calcPenWidth(width);
            ctx.setLineWidth(width + ctxsel->getLineWidth());
        }

        if (ctxsel && !ctxsel->isNullLine())
            ctx.setLineColor(ctxsel->getLineColor());

        if (ctxsel && !ctxsel->isNullLine())
            ctx.setLineStyle(ctxsel->getLineStyle());

        if (ctxsel && ctxsel->hasFillColor())
            ctx.setFillColor(ctxsel->getFillColor());

        return ctx;
    }

public:
    MgObject* clone() const
    {
        GiShapeT<Shape> *p = new GiShapeT<Shape>;
        p->copy(*this);
        return p;
    }

    void copy(const MgObject& src)
    {
        if (src.isKindOf(Type())) {
            const GiShapeT<Shape> *p = (const GiShapeT<Shape> *)&src;
            shape = p->shape;
            context = p->context;
        }
    }

    bool equals(const MgObject& src) const
    {
        bool ret = false;

        if (src.isKindOf(Type())) {
            const GiShapeT<Shape> *_src = (const GiShapeT<Shape> *)&src;
            ret = (p()->equals(_src->shape) && context == _src->context);
        }

        return ret;
    }

    void release()
    {
        delete this;
    }

    UInt32 getType() const
    {
        return Shape::Type();
    }

    bool isKindOf(UInt32 type) const
    {
        return p()->isKindOf(type);
    }

    static UInt32 Type()
    {
        return Shape::Type();
    }

    Box2d getExtent() const
    {
        return p()->getExtent();
    }

    void update()
    {
        p()->update();
    }

    void transform(const Matrix2d& mat)
    {
        p()->transform(mat);
    }

    void clear()
    {
        p()->clear();
    }

    UInt32 getPointCount() const
    {
        return p()->getPointCount();
    }

    Point2d getPoint(UInt32 index) const
    {
        return p()->getPoint(index);
    }

    void setPoint(UInt32 index, const Point2d& pt)
    {
        p()->setPoint(index, pt);
    }

    bool isClosed() const
    {
        return p()->isClosed();
    }

    double hitTest(const Point2d& pt, double tol, 
        Point2d& ptNear, Int32& segment) const
    {
        return p()->hitTest(pt, tol, ptNear, segment);
    }

    bool draw(GiGraphics& gs, const GiContext& ctx) const
    {
        return p()->draw(gs, ctx);
    }
};

#endif // __GEOMETRY_GISHAPE_H_
