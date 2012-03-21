//! \file mgshapet.h
//! \brief 定义矢量图形模板类 MgShapeT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGSHAPE_TEMPL_H_
#define __GEOMETRY_MGSHAPE_TEMPL_H_

#include <gigraph.h>
#include <mgshape.h>

//! 矢量图形模板类
/*! \ingroup _GEOM_SHAPE_
*/
template <class ShapeT, class ContextT = GiContext>
class MgShapeT : public MgShape
{
    typedef MgShapeT<ShapeT, ContextT> ThisClass;
public:
    ShapeT      _shape;
    ContextT    _context;
    UInt32      _id;
    MgShapes*   _parent;

    MgShapeT() : _id(0), _parent(NULL)
    {
    }

    virtual ~MgShapeT()
    {
    }

    GiContext* context()
    {
        return &_context;
    }

    MgBaseShape* shape()
    {
        return &_shape;
    }

    const MgBaseShape* shape() const
    {
        return &_shape;
    }

    bool draw(GiGraphics& gs, const GiContext *ctx = NULL) const
    {
        ContextT tmpctx(getContext(gs, ctx));
        return shape()->draw(gs, tmpctx);
    }

    static UInt32 Type() { return 9; }
    UInt32 getType() const { return Type(); }

    bool isKindOf(UInt32 type) const
    {
        return type == Type() || type == MgShape::Type();
    }

    void release()
    {
        delete this;
    }

    MgObject* clone() const
    {
        ThisClass *p = new ThisClass;

        p->shape()->copy(_shape);
        p->_context = _context;

        return p;
    }

    void copy(const MgObject& src)
    {
        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            shape()->copy(_src._shape);
            _context = _src._context;
        }
        else if (src.isKindOf(ShapeT::Type())) {
            shape()->copy((const ShapeT&)src);
        }
    }
    
    bool equals(const MgObject& src) const
    {
        bool ret = false;

        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            ret = shape()->equals(_src._shape)
                && _context == _src._context;
        }

        return ret;
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

#endif // __GEOMETRY_MGSHAPE_TEMPL_H_
