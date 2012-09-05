// mgline.cpp: 实现线段图形类 MgLine
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>
#include <mgnear.h>
#include <mglnrel.h>
#include <mgstorage.h>

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

UInt32 MgLine::_getHandleCount() const
{
    return 3;
}

Point2d MgLine::_getHandlePoint(UInt32 index) const
{
    return index < 2 ? _getPoint(index) : center();
}

bool MgLine::_setHandlePoint(UInt32 index, const Point2d& pt, float)
{
    if (index < 2)
        _setPoint(index, pt);
    else
        offset(pt - center(), -1);
    return true;
}

float MgLine::_hitTest(const Point2d& pt, float tol, 
                       Point2d& nearpt, Int32& segment) const
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

bool MgLine::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = gs.drawLine(&ctx, _points[0], _points[1]);
    return __super::_draw(gs, ctx) || ret;
}

bool MgLine::_save(MgStorage* s) const
{
    s->writeFloatArray("points", &(_points[0].x), 4);
    return true;
}

bool MgLine::_load(MgStorage* s)
{
    return s->readFloatArray("points", &(_points[0].x), 4) == 4;
}

// MgParallelogram
//

MG_IMPLEMENT_CREATE(MgParallelogram)

MgParallelogram::MgParallelogram()
{
}

MgParallelogram::~MgParallelogram()
{
}

UInt32 MgParallelogram::_getPointCount() const
{
    return 4;
}

Point2d MgParallelogram::_getPoint(UInt32 index) const
{
    return _points[index];
}

void MgParallelogram::_setPoint(UInt32 index, const Point2d& pt)
{
    _points[index] = pt;
}

bool MgParallelogram::_isClosed() const
{
    return true;
}

void MgParallelogram::_copy(const MgParallelogram& src)
{
    for (int i = 0; i < 4; i++)
        _points[i] = src._points[i];
    _fixlen = src._fixlen;
    __super::_copy(src);
}

bool MgParallelogram::_equals(const MgParallelogram& src) const
{
    for (int i = 0; i < 4; i++) {
        if (_points[i] != src._points[i])
            return false;
    }
    return _fixlen == src._fixlen && __super::_equals(src);
}

void MgParallelogram::_update()
{
    _points[3] = _points[0] + _points[2] - _points[1];
    _extent.set(4, _points);
    __super::_update();
}

void MgParallelogram::_transform(const Matrix2d& mat)
{
    for (int i = 0; i < 4; i++)
        _points[i] *= mat;
    __super::_transform(mat);
}

void MgParallelogram::_clear()
{
    for (int i = 1; i < 4; i++)
        _points[i] = _points[0];
    __super::_clear();
}

bool MgParallelogram::_setHandlePoint(UInt32 index, const Point2d& pt, float)
{
    index = index % 4;
    if (_fixlen) {
        Point2d& basept = _points[index > 0 ? index - 1 : index + 1];
        _points[index] = basept.rulerPoint(pt, _points[index].distanceTo(basept), 0);
    }
    else {
        _points[index] = pt;
    }
    switch (index) {
        case 0: _points[3] = _points[0] + _points[2] - _points[1]; break;
        case 1: _points[2] = _points[1] + _points[3] - _points[0]; break;
        case 2: _points[3] = _points[2] + _points[0] - _points[1]; break;
        case 3: _points[0] = _points[3] + _points[1] - _points[2]; break;
    }
    update();
    return true;
}

bool MgParallelogram::_offset(const Vector2d& vec, Int32 segment)
{
    if (segment < 0)
        return __super::_offset(vec, segment);
    return _setHandlePoint(segment, _points[segment] + vec, 0);
}

float MgParallelogram::_hitTest(const Point2d& pt, float tol, 
                                Point2d& nearpt, Int32& segment) const
{
    return mgLinesHit(4, _points, true, pt, tol, nearpt, segment);
}

bool MgParallelogram::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;

    for (int i = 0; i < 3; i++) {
        if (Box2d(_points[i], _points[i + 1]).isIntersect(rect))
            return true;
    }

    return false;
}

bool MgParallelogram::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = gs.drawPolygon(&ctx, 4, _points);
    return __super::_draw(gs, ctx) || ret;
}

bool MgParallelogram::_save(MgStorage* s) const
{
    s->writeBool("fixlen", _fixlen);
    s->writeFloatArray("points", &(_points[0].x), 8);
    return true;
}

bool MgParallelogram::_load(MgStorage* s)
{
    _fixlen = s->readBool("fixlen", _fixlen);
    return s->readFloatArray("points", &(_points[0].x), 8) == 8;
}
