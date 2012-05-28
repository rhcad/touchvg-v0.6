// mglines.cpp: 实现折线图形类 MgLines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>
#include <mgnear.h>
#include <mgstorage.h>

// MgBaseLines
//

MgBaseLines::MgBaseLines()
    : _points(NULL), _maxCount(0), _count(0), _closed(false)
{
}

MgBaseLines::~MgBaseLines()
{
    if (_points)
        delete[] _points;
}

UInt32 MgBaseLines::_getPointCount() const
{
    return _count;
}

Point2d MgBaseLines::_getPoint(UInt32 index) const
{
    return index < _count ? _points[index] : Point2d();
}

void MgBaseLines::_setPoint(UInt32 index, const Point2d& pt)
{
    if (index < _count)
        _points[index] = pt;
}

bool MgBaseLines::_isClosed() const
{
    return _closed;
}

void MgBaseLines::_copy(const MgBaseLines& src)
{
    resize(src._count);
    for (UInt32 i = 0; i < _count; i++)
        _points[i] = src._points[i];
    _closed = src._closed;

    __super::_copy(src);
}

bool MgBaseLines::_equals(const MgBaseLines& src) const
{
    if (_closed != src._closed || _count != src._count)
        return false;

    for (UInt32 i = 0; i < _count; i++)
    {
        if (_points[i] != src._points[i])
            return false;
    }

    return __super::_equals(src);
}

bool MgBaseLines::_isKindOf(UInt32 type) const
{
    return type == Type() || __super::_isKindOf(type);
}

void MgBaseLines::_update()
{
    _extent.set(_count, _points);
    __super::_update();
}

void MgBaseLines::_transform(const Matrix2d& mat)
{
    for (UInt32 i = 0; i < _count; i++)
        _points[i] *= mat;
    __super::_transform(mat);
}

void MgBaseLines::_clear()
{
    _count = 0;
    _closed = false;
    __super::_clear();
}

Point2d MgBaseLines::endPoint() const
{
    return _count > 0 ? _points[_count - 1] : Point2d();
}

bool MgBaseLines::setClosed(bool closed)
{
    _closed = closed;
    return true;
}

bool MgBaseLines::resize(UInt32 count)
{
    if (_maxCount < count)
    {
        _maxCount = (count + 7) / 8 * 8;

        Point2d* pts = new Point2d[_maxCount];

        for (UInt32 i = 0; i < _count; i++)
            pts[i] = _points[i];
        if (_points)
            delete[] _points;
        _points = pts;
    }
    _count = count;
    return true;
}

bool MgBaseLines::addPoint(const Point2d& pt)
{
    resize(_count + 1);
    _points[_count - 1] = pt;
    return true;
}

bool MgBaseLines::insertPoint(Int32 segment, const Point2d& pt)
{
    bool ret = false;
    
    if (segment >= 0 && segment < (Int32)(_count - (_closed ? 0 : 1))) {
        resize(_count + 1);
        for (Int32 i = (Int32)_count - 1; i > segment + 1; i--)
            _points[i] = _points[i - 1];
        _points[segment + 1] = pt;
        ret = true;
    }
    
    return ret;
}

bool MgBaseLines::removePoint(UInt32 index)
{
    bool ret = false;
    
    if (index < _count && _count > 1)
    {
        for (UInt32 i = index + 1; i < _count; i++)
            _points[i - 1] = _points[i];
        _count--;
        ret = true;
    }
    
    return ret;
}

bool MgBaseLines::_setHandlePoint(UInt32 index, const Point2d& pt, float tol)
{
    Int32 preindex = (_closed && 0 == index) ? _count - 1 : index - 1;
    UInt32 postindex = (_closed && index + 1 == _count) ? 0 : index + 1;
    
    float predist = preindex < 0 ? _FLT_MAX : getPoint(preindex).distanceTo(pt);
    float postdist = postindex >= _count ? _FLT_MAX : getPoint(postindex).distanceTo(pt);
    
    if (predist < tol || postdist < tol) {
        removePoint(index);
    }
    else if (!_closed && ((index == 0 && getPoint(_count - 1).distanceTo(pt) < tol)
             || (index == _count - 1 && getPoint(0).distanceTo(pt) < tol))) {
        removePoint(index);
        _closed = true;
    }
    else {
        setPoint(index, pt);
    }
    update();
    
    return true;
}

float MgBaseLines::_hitTest(const Point2d& pt, float tol, 
                            Point2d& nearpt, Int32& segment) const
{
    return mgLinesHit(_count, _points, _closed, pt, tol, nearpt, segment);
}

bool MgBaseLines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (UInt32 i = 0; i + 1 < _count; i++) {
        if (Box2d(_points[i], _points[i + 1]).isIntersect(rect))
            return true;
    }
    
    return _count < 2;
}

bool MgBaseLines::_save(MgStorage* s) const
{
    s->writeBool("closed", _closed);
    s->writeUInt32("count", _count);
    s->writeFloatArray("points", (const float*)_points, _count * 2);
    return true;
}

bool MgBaseLines::_load(MgStorage* s)
{
    _closed = s->readBool("closed", _closed);
    
    UInt32 n = s->readUInt32("count");
    if (n < 1 || n > 9999)
        return false;
    
    resize(n);
    n = s->readFloatArray("points", (float*)_points, _count * 2);
    
    return n == _count * 2;
}

// MgLines
//

MG_IMPLEMENT_CREATE(MgLines)

MgLines::MgLines()
{
}

MgLines::~MgLines()
{
}

bool MgLines::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;
    if (_closed)
        ret = gs.drawPolygon(&ctx, _count, _points);
    else
        ret = gs.drawLines(&ctx, _count, _points);
    return __super::_draw(gs, ctx) || ret;
}
