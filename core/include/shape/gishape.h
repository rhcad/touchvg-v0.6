//! \file gishape.h
//! \brief 定义矢量图形模板类 GiShapeT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_GISHAPE_TEMPL_H_
#define __GEOMETRY_GISHAPE_TEMPL_H_

#include <gigraph.h>
#include <mgshape.h>

//! 矢量图形模板类
/*! \ingroup _GEOM_SHAPE_
*/
template <class ShapeT, class ContextT = GiContext>
class GiShapeT : public GiShape
{
public:
    ShapeT      _shape;
    ContextT    _context;
    UInt32      _id;
    MgShapes*   _parent;

    GiShapeT() : _id(0), _parent(NULL)
    {
    }

    virtual ~GiShapeT()
    {
    }

    GiContext* context()
    {
        return &_context;
    }

    MgShape* shape()
    {
        return &_shape;
    }

    const MgShape* shape() const
    {
        return &_shape;
    }

    bool draw(GiGraphics& gs, const GiContext *ctx = NULL) const
    {
        ContextT tmpctx(getContext(gs, ctx));
        return shape()->draw(gs, tmpctx);
    }

    void release()
    {
        delete this;
    }

    GiShape* clone() const
    {
        GiShapeT<ShapeT, ContextT> *p = new GiShapeT<ShapeT, ContextT>;

        p->shape()->copy(_shape);
        p->_context = _context;

        return p;
    }

    UInt32 getID() const
    {
        return _id;
    }

    MgShapes* getParent() const
    {
        return _parent;
    }

    void setParent(MgShapes* p, UInt32 id)
    {
        _parent = p;
        _id = id;
    }

protected:
    ContextT getContext(GiGraphics& gs, const GiContext *ctx) const
    {
        ContextT tmpctx(_context);

        if (ctx && !ctx->isNullLine()) {
            Int16 width = tmpctx.getLineWidth();
            if (width > 0)
                width = - (Int16)gs.calcPenWidth(width);
            tmpctx.setLineWidth(width + ctx->getLineWidth());
        }

        if (ctx && !ctx->isNullLine())
            tmpctx.setLineColor(ctx->getLineColor());

        if (ctx && !ctx->isNullLine())
            tmpctx.setLineStyle(ctx->getLineStyle());

        if (ctx && ctx->hasFillColor())
            tmpctx.setFillColor(ctx->getFillColor());

        return tmpctx;
    }
};

#endif // __GEOMETRY_GISHAPE_TEMPL_H_
