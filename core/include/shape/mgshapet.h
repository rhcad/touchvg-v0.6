//! \file mgshapet.h
//! \brief 定义矢量图形模板类 MgShapeT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSHAPE_TEMPL_H_
#define __GEOMETRY_MGSHAPE_TEMPL_H_

#include <gigraph.h>
#include <mgshape.h>
#include <mgstorage.h>

//! 矢量图形模板类
/*! \ingroup GEOM_SHAPE
 */
template <class ShapeT, class ContextT = GiContext>
class MgShapeT : public MgShape
{
    typedef MgShapeT<ShapeT, ContextT> ThisClass;
public:
    ShapeT      _shape;
    ContextT    _context;
    int         _id;
    MgShapes*   _parent;
    int         _tag;
    
    MgShapeT() : _id(0), _parent(NULL), _tag(0)
    {
    }
    
    MgShapeT(const ContextT& ctx) : _id(0), _parent(NULL), _tag(0)
    {
        _context = ctx;
    }
    
    virtual ~MgShapeT()
    {
    }
    
    GiContext* context()
    {
        return &_context;
    }
    
    const GiContext* contextc() const
    {
        return &_context;
    }
    
    MgBaseShape* shape()
    {
        return &_shape;
    }
    
    const MgBaseShape* shapec() const
    {
        return &_shape;
    }
    
    bool draw(int mode, GiGraphics& gs, const GiContext *ctx = NULL, int segment = -1) const
    {
        if (shapec()->isKindOf(6)) { // MgComposite
            GiContext ctxnull(0, GiColor(), kGiLineNull);
            return shapec()->draw(mode, gs, ctx ? *ctx : ctxnull, segment);
        }
        ContextT tmpctx(getContext(gs, ctx));
        return shapec()->draw(mode, gs, tmpctx, segment);
    }
    
    static MgShape* create()
    {
        return new ThisClass;
    }
    
    static int Type() { return 0x10000 | ShapeT::Type(); }
    int getType() const { return Type(); }
    
    bool isKindOf(int type) const
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
        p->copy(*this);
        return p;
    }
    
    void copy(const MgObject& src)
    {
        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            shape()->copy(_src._shape);
            _context = _src._context;
            _tag = _src._tag;
            if (!_parent && 0 == _id) {
                _parent = _src._parent;
                _id = _src._id;
            }
        }
        else if (src.isKindOf(ShapeT::Type())) {
            shape()->copy((const ShapeT&)src);
        }
        shape()->update();
    }
    
    bool equals(const MgObject& src) const
    {
        bool ret = false;
        
        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            ret = shapec()->equals(_src._shape)
            && _context == _src._context
            && _tag == _src._tag;
        }
        
        return ret;
    }
    
    int getID() const
    {
        return _id;
    }
    
    MgShapes* getParent() const
    {
        return _parent;
    }
    
    void setParent(MgShapes* p, int sid)
    {
        _parent = p;
        _id = sid;
        shape()->setOwner(this);
    }

    int getTag() const
    {
        return _tag;
    }

    void setTag(int tag)
    {
        _tag = tag;
    }
    
    bool save(MgStorage* s) const
    {
        GiColor c;
        
        s->writeUInt32("tag", _tag);
        s->writeUInt8("lineStyle", (unsigned char)_context.getLineStyle());
        s->writeFloat("lineWidth", _context.getLineWidth());
        
        c = _context.getLineColor();
        s->writeUInt32("lineColor", c.r | (c.g << 8) | (c.b << 16) | (c.a << 24));
        c = _context.getFillColor();
        s->writeUInt32("fillColor", c.r | (c.g << 8) | (c.b << 16) | (c.a << 24));
        s->writeBool("autoFillColor", _context.isAutoFillColor());
        
        return shapec()->save(s);
    }
    
    bool load(MgStorage* s)
    {
        _tag = s->readUInt32("tag", _tag);
        _context.setLineStyle((GiLineStyle)s->readUInt8("lineStyle", 0));
        _context.setLineWidth(s->readFloat("lineWidth", 0), true);
        
        _context.setLineColor(GiColor(s->readUInt32("lineColor", 0xFF000000), true));
        _context.setFillColor(GiColor(s->readUInt32("fillColor", 0), true));
        _context.setAutoFillColor(s->readBool("autoFillColor", _context.isAutoFillColor()));
        
        bool ret = shape()->load(s);
        if (ret) {
            shape()->update();
        }
        
        return ret;
    }
    
protected:
    ContextT getContext(GiGraphics& gs, const GiContext *ctx) const
    {
        ContextT tmpctx(_context);
        
        if (ctx) {
            float addw  = ctx->getLineWidth();
            float width = tmpctx.getLineWidth();
            
            width = -gs.calcPenWidth(width, tmpctx.isAutoScale());  // 像素宽度，负数
            if (addw <= 0)
                tmpctx.setLineWidth(width + addw, false);           // 像素宽度加宽
            else                                                    // 传入正数表示像素宽度
                tmpctx.setLineWidth(-addw, ctx->isAutoScale());     // 换成新的像素宽度
        }
        
        if (ctx && ctx->getLineColor().a > 0) {
            tmpctx.setLineColor(ctx->getLineColor());
        }
        if (ctx && !ctx->isNullLine()) {
            tmpctx.setLineStyle(ctx->getLineStyle());
        }
        if (ctx && ctx->hasFillColor()) {
            tmpctx.setFillColor(ctx->getFillColor());
        }
        
        return tmpctx;
    }
};

#endif // __GEOMETRY_MGSHAPE_TEMPL_H_
