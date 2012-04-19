//! \file _mgshape.h
//! \brief 定义图形类实现用的辅助宏
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGSHAPEIMPL_H_
#define __GEOMETRY_MGSHAPEIMPL_H_

#include <gigraph.h>

#define MG_IMPLEMENT_CREATE(Cls)                                \
    Cls* Cls::create() { return new Cls(); }                    \
    MgObject* Cls::clone() const                                \
        { Cls* p = create(); p->_copy(*this); return p; }       \
    void Cls::copy(const MgObject& src)                         \
        { if (src.isKindOf(Type())) _copy((const Cls&)src); }   \
    void Cls::release() { delete this; }                        \
    bool Cls::equals(const MgObject& src) const                 \
        { return src.isKindOf(Type()) && _equals((const Cls&)src); } \
    bool Cls::_isKindOf(UInt32 type) const                      \
        { return type == Type() || __super::_isKindOf(type); }  \
    Box2d Cls::getExtent() const { return _getExtent(); }       \
    void Cls::update() { _update(); }                           \
    void Cls::transform(const Matrix2d& mat) { _transform(mat); } \
    void Cls::clear() { _clear(); }                             \
    UInt32 Cls::getPointCount() const { return _getPointCount(); } \
    Point2d Cls::getPoint(UInt32 index) const { return _getPoint(index); }  \
    void Cls::setPoint(UInt32 index, const Point2d& pt) { _setPoint(index, pt); }  \
    bool Cls::isClosed() const { return _isClosed(); }          \
    float Cls::hitTest(const Point2d& pt, float tol, Point2d& nearpt, Int32& segment) const \
        { return _hitTest(pt, tol, nearpt, segment); }          \
    bool Cls::hitTestBox(const Box2d& rect) const               \
        { return _hitTestBox(rect); }                           \
    bool Cls::draw(GiGraphics& gs, const GiContext& ctx) const  \
        { return _draw(gs, ctx); }                              \
    bool Cls::save(MgStorage* s) const { return _save(s); }     \
    bool Cls::load(MgStorage* s)       { return _load(s); }     \
    UInt32 Cls::getHandleCount() const { return _getHandleCount(); }    \
    Point2d Cls::getHandlePoint(UInt32 index) const             \
        { return _getHandlePoint(index); }                      \
    bool Cls::setHandlePoint(UInt32 index, const Point2d& pt, float tol)   \
        { return _setHandlePoint(index, pt, tol); }             \
    bool Cls::offset(const Vector2d& vec, Int32 segment)   \
        { return _offset(vec, segment); }

#endif // __GEOMETRY_MGSHAPEIMPL_H_
