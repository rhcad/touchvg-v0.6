// mgrect.cpp: 实现矩形图形类 MgRect
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>
#include <mgnear.h>
#include <mgstorage.h>

// MgBaseRect
//

MgBaseRect::MgBaseRect()
{
}

UInt32 MgBaseRect::_getPointCount() const
{
    return 4;
}

Point2d MgBaseRect::_getPoint(UInt32 index) const
{
    return _points[index];
}

void MgBaseRect::_setPoint(UInt32 index, const Point2d& pt)
{
    _points[index] = pt;
}

bool MgBaseRect::_isClosed() const
{
    return true;
}

void MgBaseRect::_copy(const MgBaseRect& src)
{
    for (int i = 0; i < 4; i++)
        _points[i] = src._points[i];
    __super::_copy(src);
}

bool MgBaseRect::_equals(const MgBaseRect& src) const
{
    for (int i = 0; i < 4; i++)
    {
        if (_points[i] != src._points[i])
            return false;
    }
    return __super::_equals(src);
}

bool MgBaseRect::_isKindOf(UInt32 type) const
{
    return type == Type() || __super::_isKindOf(type);
}

void MgBaseRect::_update()
{
    float yoff = _points[2].distanceTo(_points[1]);
    _points[2] = _points[1].rulerPoint(_points[0], yoff);
    _points[3] = _points[0].rulerPoint(_points[1], -yoff);
    _extent.set(4, _points);
    __super::_update();
}

void MgBaseRect::_transform(const Matrix2d& mat)
{
    for (int i = 0; i < 4; i++)
        _points[i] *= mat;
    __super::_transform(mat);
}

void MgBaseRect::_clear()
{
    Point2d center = getCenter();
    for (int i = 0; i < 4; i++)
        _points[1] = center;
    __super::_clear();
}

Point2d MgBaseRect::getCenter() const
{
    return (_points[0] + _points[2]) * 0.5;
}

Box2d MgBaseRect::getRect() const
{
    return Box2d(getCenter(), getWidth(), getHeight());
}

float MgBaseRect::getWidth() const
{
    return _points[0].distanceTo(_points[1]);
}

float MgBaseRect::getHeight() const
{
    return _points[1].distanceTo(_points[2]);
}

float MgBaseRect::getAngle() const
{
    return (_points[1] - _points[0]).angle2();
}

bool MgBaseRect::isEmpty(float minDist) const
{
    return _points[2].distanceTo(_points[0]) < mgMax(minDist, _MGZERO);
}

bool MgBaseRect::isOrtho() const
{
    return mgIsZero(_points[1].y - _points[0].y);
}

void MgBaseRect::setRect(const Box2d& rect, float angle)
{
    _points[0] = rect.leftTop();
    _points[1] = rect.rightTop();
    _points[2] = rect.rightBottom();
    _points[3] = rect.leftBottom();

    if (!mgIsZero(angle))
    {
        Matrix2d mat(Matrix2d::rotation(angle, rect.center()));
        for (int i = 0; i < 4; i++)
            _points[i] *= mat;
    }
}

void MgBaseRect::setRect(const Point2d points[4])
{
    for (int i = 0; i < 4; i++)
        _points[i] = points[i];
}

void MgBaseRect::setCenter(const Point2d& pt)
{
    Point2d old = getCenter();
    for (int i = 0; i < 4; i++)
        _points[i].offset(pt.x - old.x, pt.y - old.y);
}

float MgBaseRect::_hitTest(const Point2d& pt, float tol, 
                           Point2d& nearpt, Int32& segment) const
{
    return mgLinesHit(4, _points, true, pt, tol, nearpt, segment);
}

bool MgBaseRect::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i < 3; i++) {
        if (Box2d(_points[i], _points[i + 1]).isIntersect(rect))
            return true;
    }
    
    return false;
}

UInt32 MgBaseRect::_getHandleCount() const
{
    return 8;
}

Point2d MgBaseRect::_getHandlePoint(UInt32 index) const
{
    Point2d pt;
    mgGetRectHandle(getRect(), index, pt);
    pt *= Matrix2d::rotation(getAngle(), getCenter());
    return pt;
}

bool MgBaseRect::_setHandlePoint(UInt32 index, const Point2d& pt, float)
{
    Point2d pt2(pt * Matrix2d::rotation(-getAngle(), getCenter()));
    Box2d rect(getRect());
    mgMoveRectHandle(rect, index, pt2);
    setRect(rect, getAngle());
    update();
    return true;
}

bool MgBaseRect::_save(MgStorage* s) const
{
    s->writeFloatArray("points", &(_points[0].x), 8);
    return true;
}

bool MgBaseRect::_load(MgStorage* s)
{
    return s->readFloatArray("points", &(_points[0].x), 8) == 8;
}

// MgRect
//

MG_IMPLEMENT_CREATE(MgRect)

MgRect::MgRect()
{
}

MgRect::~MgRect()
{
}

bool MgRect::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = gs.drawPolygon(&ctx, 4, _points);
    return __super::_draw(gs, ctx) || ret;
}
