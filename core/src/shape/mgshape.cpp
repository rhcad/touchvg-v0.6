// mgshape.cpp: 实现矢量图形基类 MgShape
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgshape.h"

MgShape::MgShape()
{
}

MgShape::~MgShape()
{
}

void MgShape::_copy(const MgShape& src)
{
    _extent = src._extent;
}

bool MgShape::_equals(const MgShape&) const
{
    return true;
}

bool MgShape::_isKindOf(UInt32 type) const
{
    return type == Type();
}

Box2d MgShape::getExtent() const
{
    return _extent;
}

void MgShape::_update()
{
}

void MgShape::_transform(const Matrix2d& mat)
{
    _extent *= mat;
}

void MgShape::_clear()
{
    _extent.empty();
}

bool MgShape::_draw(GiGraphics&, const GiContext&) const
{
    return false;
}
