// mgshape.cpp: 实现矢量图形基类 MgBaseShape
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

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

Box2d MgBaseShape::getExtent() const
{
    return _extent;
}

void MgBaseShape::_update()
{
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

UInt32 MgBaseShape::getHandleCount() const
{
    return getPointCount();
}

Point2d MgBaseShape::getHandlePoint(UInt32 index) const
{
    return getPoint(index);
}

bool MgBaseShape::setHandlePoint(UInt32 index, const Point2d& pt, double)
{
    setPoint(index, pt);
    update();
    return true;
}

bool MgBaseShape::offset(const Vector2d& vec, Int32)
{
    for (UInt32 i = 0; i < getPointCount(); i++) {
        setPoint(i, getPoint(i) + vec);
    }
    update();
    return true;
}
