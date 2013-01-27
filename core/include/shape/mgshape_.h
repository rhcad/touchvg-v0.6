//! \file mgshape_.h
//! \brief 定义图形类实现用的辅助宏
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSHAPEIMPL_H_
#define __GEOMETRY_MGSHAPEIMPL_H_

#include <gigraph.h>

#define MG_IMPLEMENT_CREATE(Cls)                                \
    Cls* Cls::create() { return new Cls(); }                    \
    MgObject* Cls::clone() const                                \
        { Cls* p = create(); p->_copy(*this); return p; }       \
    void Cls::copy(const MgObject& src) {                       \
        if (src.isKindOf(Type())) _copy((const Cls&)src);       \
        else if (src.isKindOf(__super::Type()))                 \
            __super::_copy((const Cls&)src);                    \
    }                                                           \
    void Cls::release() { delete this; }                        \
    bool Cls::equals(const MgObject& src) const                 \
        { return src.isKindOf(Type()) && _equals((const Cls&)src); } \
    bool Cls::_isKindOf(int type) const                      \
        { return type == Type() || __super::_isKindOf(type); }  \
    Box2d Cls::getExtent() const { return _getExtent(); }       \
    void Cls::update() { _update(); }                           \
    void Cls::transform(const Matrix2d& mat) { _transform(mat); } \
    void Cls::clear() { _clear(); }                             \
    int Cls::getPointCount() const { return _getPointCount(); } \
    Point2d Cls::getPoint(int index) const { return _getPoint(index); }  \
    void Cls::setPoint(int index, const Point2d& pt) { _setPoint(index, pt); }  \
    bool Cls::isClosed() const { return _isClosed(); }   \
    float Cls::hitTest(const Point2d& pt, float tol, Point2d& nearpt, int& segment) const \
        { return _hitTest(pt, tol, nearpt, segment); }          \
    bool Cls::hitTestBox(const Box2d& rect) const               \
        { return _hitTestBox(rect); }                           \
    bool Cls::draw(int mode, GiGraphics& gs, const GiContext& ctx) const  \
        { return _draw(mode, gs, ctx); }                              \
    bool Cls::save(MgStorage* s) const { return _save(s); }     \
    bool Cls::load(MgStorage* s)       { return _load(s); }     \
    int Cls::getHandleCount() const { return _getHandleCount(); }    \
    Point2d Cls::getHandlePoint(int index) const             \
        { return _getHandlePoint(index); }                      \
    bool Cls::setHandlePoint(int index, const Point2d& pt, float tol)   \
        { return _rotateHandlePoint(index, pt) || _setHandlePoint(index, pt, tol); } \
    bool Cls::isHandleFixed(int index) const { return _isHandleFixed(index); } \
    bool Cls::offset(const Vector2d& vec, int segment)        \
        { return _offset(vec, segment); }                       \
    int Cls::getDimensions(const Matrix2d& m2w, float* vars, char* types, int count) const \
        { return _getDimensions(m2w, vars, types, count); }

#endif // __GEOMETRY_MGSHAPEIMPL_H_
