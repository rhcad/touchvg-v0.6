// mgshape.cpp: 实现矢量图形基类 MgBaseShape
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgshape.h"
#include <mgstorage.h>

MgBaseShape::MgBaseShape() : _flags(0)
{
}

MgBaseShape::~MgBaseShape()
{
}

void MgBaseShape::_copy(const MgBaseShape& src)
{
    _extent = src._extent;
    _flags = src._flags;
}

bool MgBaseShape::_equals(const MgBaseShape& src) const
{
    return _flags == src._flags;
}

void MgBaseShape::_update()
{
    if (_extent.width() < minTol().equalPoint()) {
        _extent.inflate(minTol().equalPoint(), 0);
    }
    if (_extent.height() < minTol().equalPoint()) {
        _extent.inflate(0, minTol().equalPoint());
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

bool MgBaseShape::_draw(int, GiGraphics&, const GiContext&, int) const
{
    return false;
}

int MgBaseShape::_getHandleCount() const
{
    return getPointCount();
}

Point2d MgBaseShape::_getHandlePoint(int index) const
{
    return getPoint(index);
}

bool MgBaseShape::_rotateHandlePoint(int index, const Point2d& pt)
{
    if (getFlag(kMgFixedLength)) {
        if (getFlag(kMgRotateDisnable)) {
            offset(pt - getHandlePoint(index), -1);
        }
        else {
            Point2d basept(_extent.center());
            if (!getFlag(kMgSquare)) {
                int baseindex = index > 0 ? index - 1 : getHandleCount() - 1;
                while (baseindex > 0 && isHandleFixed(baseindex))
                    baseindex--;
                basept = (getHandlePoint(baseindex));
            }
            float a1 = (pt - basept).angle2();
            float a2 = (getHandlePoint(index) - basept).angle2();
            
            transform(Matrix2d::rotation(a1 - a2, basept));
        }
        return true;
    }

    return false;
}

bool MgBaseShape::_setHandlePoint(int index, const Point2d& pt, float)
{
    setPoint(index, pt);
    update();
    return true;
}

bool MgBaseShape::_setHandlePoint2(int index, const Point2d& pt, float tol, int&)
{
    return setHandlePoint(index, pt, tol);
}

bool MgBaseShape::_offset(const Vector2d& vec, int)
{
    for (int i = 0; i < getPointCount(); i++) {
        setPoint(i, getPoint(i) + vec);
    }
    update();
    return true;
}

bool MgBaseShape::_hitTestBox(const Box2d& rect) const
{
    return getExtent().isIntersect(rect);
}

bool MgBaseShape::_save(MgStorage* s) const
{
    s->writeUInt32("flags", _flags);
    return true;
}

bool MgBaseShape::_load(MgShapeFactory*, MgStorage* s)
{
    _flags = s->readUInt32("flags", _flags);
    return true;
}

bool MgBaseShape::getFlag(MgShapeBit bit) const
{
    return (_flags & (1 << bit)) != 0;
}

void MgBaseShape::setFlag(MgShapeBit bit, bool on)
{
    _flags = on ? _flags | (1 << bit) : _flags & ~(1 << bit);
}

// MgShape
//

bool MgShape::save(MgStorage* s) const {
    GiColor c;

    s->writeUInt32("tag", getTag());
    s->writeUInt8("lineStyle", (unsigned char)contextc()->getLineStyle());
    s->writeFloat("lineWidth", contextc()->getLineWidth());

    c = contextc()->getLineColor();
    s->writeUInt32("lineColor", c.b | (c.g << 8) | (c.r << 16) | (c.a << 24));
    c = contextc()->getFillColor();
    s->writeUInt32("fillColor", c.b | (c.g << 8) | (c.r << 16) | (c.a << 24));
    s->writeBool("autoFillColor", contextc()->isAutoFillColor());

    return shapec()->save(s);
}

bool MgShape::load(MgShapeFactory* factory, MgStorage* s) {
    setParent(getParent(), s->readUInt32("tag", getTag()));

    context()->setLineStyle((GiLineStyle)s->readUInt8("lineStyle", 0));
    context()->setLineWidth(s->readFloat("lineWidth", 0), true);

    context()->setLineColor(GiColor(s->readUInt32("lineColor", 0xFF000000), true));
    context()->setFillColor(GiColor(s->readUInt32("fillColor", 0), true));
    context()->setAutoFillColor(s->readBool("autoFillColor", context()->isAutoFillColor()));

    bool ret = shape()->load(factory, s);
    if (ret) {
        shape()->update();
    }

    return ret;
}
