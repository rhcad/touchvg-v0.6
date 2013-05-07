// mgline.cpp: 实现线段图形类 MgLine
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>
#include <mgnear.h>
#include <mglnrel.h>
#include <mgstorage.h>
#include <mgbase.h>

MG_IMPLEMENT_CREATE(MgLine)

MgLine::MgLine()
{
}

MgLine::~MgLine()
{
}

int MgLine::_getPointCount() const
{
    return 2;
}

Point2d MgLine::_getPoint(int index) const
{
    return _points[index ? 1 : 0];
}

void MgLine::_setPoint(int index, const Point2d& pt)
{
    _points[index ? 1 : 0] = pt;
}

int MgLine::_getHandleCount() const
{
    return 3;
}

Point2d MgLine::_getHandlePoint(int index) const
{
    return index < 2 ? _getPoint(index) : center();
}

bool MgLine::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    return index < 2 && __super::_setHandlePoint(index, pt, tol);
}

bool MgLine::_isHandleFixed(int index) const
{
    return index >= 2;
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
    if (_extent.isEmpty())
        _extent.set(_points[0], 2 * Tol::gTol().equalPoint(), 0);
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

float MgLine::_hitTest(const Point2d& pt, float tol, 
                       Point2d& nearpt, int& segment) const
{
    return mgLinesHit(2, _points, false, pt, tol, nearpt, segment);
}

bool MgLine::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    Point2d pts[2] = { _points[0], _points[1] };
    return mgClipLine(pts[0], pts[1], rect);
}

bool MgLine::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    bool ret = gs.drawLine(&ctx, _points[0], _points[1]);
    return __super::_draw(mode, gs, ctx, segment) || ret;
}

bool MgLine::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloatArray("points", &(_points[0].x), 4);
    return ret;
}

bool MgLine::_load(MgStorage* s)
{
    bool ret = __super::_load(s);
    return s->readFloatArray("points", &(_points[0].x), 4) == 4 && ret;
}

int MgLine::_getDimensions(const Matrix2d& m2w, float* vars, char* types, int count) const
{
    int ret = 0;
    
    if (count > ret) {
        types[ret] = 'l';
        vars[ret++] = fabsf(length() * m2w.m11);
    }
    if (count > ret) {
        types[ret] = 'a';
        vars[ret++] = mgRad2Deg(mgTo0_2PI(angle() * (m2w.m22 < 0 ? -1.f : 1.f)));
    }
    
    return ret;
}
