// mgline.cpp: 实现线段图形类 MgLine
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgbasicsp.h"
#include <_mgshape.h>
#include <mgnear.h>
#include <mglnrel.h>

MG_IMPLEMENT_CREATE(MgLine)

MgLine::MgLine()
{
}

MgLine::~MgLine()
{
}

UInt32 MgLine::_getPointCount() const
{
    return 2;
}

Point2d MgLine::_getPoint(UInt32 index) const
{
    return _points[index ? 1 : 0];
}

void MgLine::_setPoint(UInt32 index, const Point2d& pt)
{
    _points[index ? 1 : 0] = pt;
}

bool MgLine::_isClosed() const
{
    return false;
}

void MgLine::_copy(const MgLine& src)
{
    _points[0] = src._points[0];
    _points[1] = src._points[1];
    __super::_copy(src);
}

bool MgLine::_equals(const MgLine& src) const
{
    return _points[0] == src._points[0]
        && _points[1] == src._points[1]
        && __super::_equals(src);
}

void MgLine::_update()
{
    _extent.set(_points[0], _points[1]);
    __super::_update();
}

void MgLine::_transform(const Matrix2d& mat)
{
    _points[0] *= mat;
    _points[1] *= mat;
    __super::_transform(mat);
}

void MgLine::_clear()
{
    _points[1] = _points[0];
    __super::_clear();
}

double MgLine::_hitTest(const Point2d& pt, double tol, 
                        Point2d& ptNear, Int32& segment) const
{
    return mgLinesHit(2, _points, false, pt, tol, ptNear, segment);
}

bool MgLine::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    Point2d pts[2] = { _points[0], _points[1] };
    return mgClipLine(pts[0], pts[1], rect);
}

bool MgLine::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = gs.drawLine(&ctx, _points[0], _points[1]);
    return __super::_draw(gs, ctx) || ret;
}

UInt32 MgLine::_getHandleCount() const
{
    return 3;
}

Point2d MgLine::_getHandlePoint(UInt32 index) const
{
    return 1 == index ? center() : _points[index ? 1 : 0];
}

bool MgLine::_setHandlePoint(UInt32 index, const Point2d& pt, double)
{
    if (1 == index) {
        Vector2d off(pt - center());
        _points[0] += off;
        _points[1] += off;
    }
    else {
        _points[index ? 1 : 0] = pt;
    }
    update();
    return true;
}
