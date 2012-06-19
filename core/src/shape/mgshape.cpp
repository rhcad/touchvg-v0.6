// mgshape.cpp: 实现矢量图形基类 MgBaseShape
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgshape.h"

MgBaseShape::MgBaseShape()
{
}

MgBaseShape::~MgBaseShape()
{
}

void MgBaseShape::_copy(const MgBaseShape& src)
{
    _extent = src._extent;
}

bool MgBaseShape::_equals(const MgBaseShape&) const
{
    return true;
}

bool MgBaseShape::_isKindOf(UInt32 type) const
{
    return type == Type();
}

Box2d MgBaseShape::_getExtent() const
{
    return _extent;
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
