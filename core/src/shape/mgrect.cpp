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

void MgBaseRect::_copy(const MgBaseRect& src)
{
    for (int i = 0; i < 4; i++)
        _points[i] = src._points[i];
    __super::_copy(src);
}

bool MgBaseRect::_equals(const MgBaseRect& src) const
{
    for (int i = 0; i < 4; i++) {
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
    Box2d rect(getRect());
    setRect(rect.leftTop(), rect.rightBottom(), getAngle(), rect.center());
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

void MgBaseRect::setRect(const Point2d& pt1, const Point2d& pt2)
{
    setRect(pt1, pt2, 0, (pt1 + pt2) / 2);
}

void MgBaseRect::setRect(const Point2d& pt1, const Point2d& pt2,
                         float angle, const Point2d& basept)
{
    Box2d rect(pt1, pt2);
    
    if (getFlag(kMgSquare)) {
        float len = (float)sqrt(fabs((pt2.x - pt1.x) * (pt2.y - pt1.y)));
        rect.set(basept, len, 0);
    }
    
    _points[0] = rect.leftTop();
    _points[1] = rect.rightTop();
    _points[2] = rect.rightBottom();
    _points[3] = rect.leftBottom();

    if (!mgIsZero(angle))
    {
        Matrix2d mat(Matrix2d::rotation(angle, basept));
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
    if (index < 4 && getFlag(kMgSquare)) {
        Point2d basept(getCenter());
        Point2d pt2(pt * Matrix2d::rotation(-getAngle(), basept));
        setRect(basept * 2.f - pt2.asVector(), pt2, getAngle(), basept);
    }
    else {
        Point2d pt2(pt * Matrix2d::rotation(-getAngle(), getCenter()));
        Box2d rect(getRect());
        mgMoveRectHandle(rect, index, pt2);
        if (getFlag(kMgSquare) && (4 == index || 6 == index)) {
            rect = Box2d(rect.center(), rect.height(), rect.height());
        }
        else if (getFlag(kMgSquare)) {
            rect = Box2d(rect.center(), rect.width(), rect.width());
        }
        setRect(rect.leftTop(), rect.rightBottom(), getAngle(), getCenter());
    }
    update();
    return true;
}

bool MgBaseRect::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloatArray("points", &(_points[0].x), 8);
    return ret;
}

bool MgBaseRect::_load(MgStorage* s)
{
    return __super::_load(s) && s->readFloatArray("points", &(_points[0].x), 8) == 8;
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

// MgDiamond
//

MG_IMPLEMENT_CREATE(MgDiamond)

MgDiamond::MgDiamond()
{
}

MgDiamond::~MgDiamond()
{
}

UInt32 MgDiamond::_getHandleCount() const
{
    return 4;
}

Point2d MgDiamond::_getHandlePoint(UInt32 index) const
{
    return MgBaseRect::_getHandlePoint(4 + index % 4);
}

bool MgDiamond::_setHandlePoint(UInt32 index, const Point2d& pt, float tol)
{
    if (!getFlag(kMgFixedLength)) {
        return MgBaseRect::_setHandlePoint(4 + index % 4, pt, tol);
    }
    
    Point2d cen(getCenter());
    Point2d pnt, ptup, ptside;
    
    pnt = pt * Matrix2d::rotation(-getAngle(), cen);
    mgGetRectHandle(getRect(), 4 + (index + 2) % 4, ptup);
    mgGetRectHandle(getRect(), 4 + (index + 1) % 4, ptside);
    
    float len = ptup.distanceTo(ptside);
    float dy = index % 2 == 0 ? pnt.y - ptup.y : pnt.x - ptup.x;
    float ry = mgMin(len, (float)fabs(dy) / 2);
    float rx = (float)sqrt(len * len - ry * ry);
    Box2d rect(cen, rx * 2, ry * 2);
    setRect(rect.leftTop(), rect.rightBottom(), getAngle(), cen);
    
    return true;
}

bool MgDiamond::_rotateHandlePoint(UInt32, const Point2d&)
{
    return false;
}

void MgDiamond::_update()
{
    __super::_update();
    Point2d pts[] = { _getHandlePoint(0), _getHandlePoint(1),
        _getHandlePoint(2), _getHandlePoint(3) };
    _extent.set(4, pts);
}

float MgDiamond::_hitTest(const Point2d& pt, float tol, 
                          Point2d& nearpt, Int32& segment) const
{
    Point2d pts[] = { _getHandlePoint(0), _getHandlePoint(1),
        _getHandlePoint(2), _getHandlePoint(3) };
    return mgLinesHit(4, pts, true, pt, tol, nearpt, segment);
}

bool MgDiamond::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i < 3; i++) {
        if (Box2d(_getHandlePoint(i), _getHandlePoint(i + 1)).isIntersect(rect))
            return true;
    }
    
    return false;
}

bool MgDiamond::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    Point2d pts[] = { _getHandlePoint(0), _getHandlePoint(1),
        _getHandlePoint(2), _getHandlePoint(3) };
    bool ret = gs.drawPolygon(&ctx, 4, pts);
    return __super::_draw(gs, ctx) || ret;
}
