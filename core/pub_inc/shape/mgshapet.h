//! \file mgshapet.h
//! \brief 定义矢量图形模板类 MgShapeT
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSHAPE_TEMPL_H_
#define TOUCHVG_MGSHAPE_TEMPL_H_

#include "mgshape.h"

//! 矢量图形模板类
/*! \ingroup CORE_SHAPE
    使用 MgShapeT<ShapeClass>::registerCreator() 登记图形种类;
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
    
    MgShapeT() : _id(0), _parent(NULL), _tag(0) {
    }
    
    MgShapeT(const ContextT& ctx) : _id(0), _parent(NULL), _tag(0) {
        _context = ctx;
    }
    
    virtual ~MgShapeT() {
    }

    GiContext* context() {
        return &_context;
    }
    
    const GiContext* contextc() const {
        return &_context;
    }
    
    MgBaseShape* shape() {
        return &_shape;
    }
    
    const MgBaseShape* shapec() const {
        return &_shape;
    }

    bool hasFillColor() const
    {
        return _context.hasFillColor() && _shape.isClosed();
    }
    
    bool draw(int mode, GiGraphics& gs, const GiContext *ctx = NULL, int segment = -1) const {
        if (shapec()->isKindOf(6)) { // MgComposite
            GiContext ctxnull(0, GiColor(), kGiLineNull);
            return shapec()->draw(mode, gs, ctx ? *ctx : ctxnull, segment);
        }
        ContextT tmpctx(getContext(gs, ctx));
        return shapec()->draw(mode, gs, tmpctx, segment);
    }

#ifdef TOUCHVG_SHAPE_FACTORY_H_
	//! 登记类型号对应的图形创建函数
	static void registerCreator(MgShapeFactory* factory) {
		factory->registerShape(Type(), create);
    }
#endif
    
    static MgShape* create() {
        return new ThisClass;
    }
    
    static int Type() { return 0x10000 | ShapeT::Type(); }
    int getType() const { return Type(); }
    
    bool isKindOf(int type) const {
        return type == Type() || type == MgShape::Type();
    }
    
    void release() {
        delete this;
    }
    
    MgObject* clone() const {
        ThisClass *p = new ThisClass;
        p->copy(*this);
        return p;
    }
    
    void copy(const MgObject& src) {
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
    
    bool equals(const MgObject& src) const {
        bool ret = false;
        
        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            ret = shapec()->equals(_src._shape)
            && _context == _src._context
            && _tag == _src._tag;
        }
        
        return ret;
    }
    
    int getID() const {
        return _id;
    }
    
    MgShapes* getParent() const {
        return _parent;
    }
    
    void setParent(MgShapes* p, int sid) {
        _parent = p;
        _id = sid;
        shape()->setOwner(this);
    }

    int getTag() const {
        return _tag;
    }

    void setTag(int tag) {
        _tag = tag;
    }
    
protected:
    ContextT getContext(GiGraphics& gs, const GiContext *ctx) const {
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

#endif // TOUCHVG_MGSHAPE_TEMPL_H_
