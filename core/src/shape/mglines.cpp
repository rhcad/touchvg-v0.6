// mglines.cpp: 实现折线图形类 MgLines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgbasicsp.h"
#include <_mgshape.h>
#include <mgnear.h>

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
    __super::_clear();
}

Point2d MgBaseLines::endPoint() const
{
    return _count > 0 ? _points[_count - 1] : Point2d();
}

void MgBaseLines::setClosed(bool closed)
{
    _closed = closed;
}

void MgBaseLines::resize(UInt32 count)
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
}

void MgBaseLines::addPoint(const Point2d& pt)
{
    resize(_count + 1);
    _points[_count - 1] = pt;
}

void MgBaseLines::removePoint(UInt32 index)
{
    if (index < _count)
    {
        for (UInt32 i = index + 1; i < _count; i++)
            _points[i - 1] = _points[i];
        _count--;
    }
}

double MgBaseLines::_hitTest(const Point2d& pt, double tol, 
                             Point2d& ptNear, Int32& segment) const
{
    return mgLinesHit(_count, _points, _closed, pt, tol, ptNear, segment);
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
