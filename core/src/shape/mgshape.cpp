// mgshape.cpp: 实现矢量图形基类 MgBaseShape
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgshape.h"
#include <gigraph.h>

MgBaseShape::MgBaseShape() : _fixlen(false)
{
}

MgBaseShape::~MgBaseShape()
{
}

void MgBaseShape::_copy(const MgBaseShape& src)
{
    _extent = src._extent;
    _fixlen = src._fixlen;
}

bool MgBaseShape::_equals(const MgBaseShape& src) const
{
    return _fixlen == src._fixlen;
}

bool MgBaseShape::_isKindOf(UInt32 type) const
{
    return type == Type();
}

void MgBaseShape::_update()
{
    if (!_extent.isNull()) {
        if (_extent.width() < Tol::gTol().equalPoint()) {
            _extent.inflate(Tol::gTol().equalPoint(), 0);
        }
        if (_extent.height() < Tol::gTol().equalPoint()) {
            _extent.inflate(0, Tol::gTol().equalPoint());
        }
    }
}

void MgBaseShape::_transform(const Matrix2d& mat)
{
    _extent *= mat;
}

void MgBaseShape::_clear()
{
    _extent.empty();
}

bool MgBaseShape::_draw(GiGraphics&, const GiContext&) const
{
    return false;
}

UInt32 MgBaseShape::_getHandleCount() const
{
    return getPointCount();
}

Point2d MgBaseShape::_getHandlePoint(UInt32 index) const
{
    return getPoint(index);
}

bool MgBaseShape::_rotateHandlePoint(UInt32 index, const Point2d& pt)
{
    if (_fixlen) {
        Point2d basept(getHandlePoint(index > 0 ? index - 1 : getHandleCount() - 1));
        float a1 = (pt - basept).angle2();
        float a2 = (getHandlePoint(index) - basept).angle2();
        
        transform(Matrix2d::rotation(a1 - a2, basept));
    }
    return _fixlen;
}

bool MgBaseShape::_setHandlePoint(UInt32 index, const Point2d& pt, float)
{
    setPoint(index, pt);
    update();
    return true;
}

bool MgBaseShape::_offset(const Vector2d& vec, Int32)
{
    for (UInt32 i = 0; i < getPointCount(); i++) {
        setPoint(i, getPoint(i) + vec);
    }
    update();
    return true;
}

bool MgBaseShape::_hitTestBox(const Box2d& rect) const
{
    return getExtent().isIntersect(rect);
}
