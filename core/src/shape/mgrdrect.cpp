// mgrdrect.cpp: 实现圆角矩形类 MgRoundRect
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "mgbasicsp.h"
#include <_mgshape.h>
#include <mgnear.h>
#include <mgcurv.h>

MG_IMPLEMENT_CREATE(MgRoundRect)

MgRoundRect::MgRoundRect() : _rx(0), _ry(0)
{
}

MgRoundRect::~MgRoundRect()
{
}

void MgRoundRect::setRadius(float rx, float ry)
{
    _rx = fabs(rx);
    _ry = fabs(ry);
    if (_ry < _MGZERO)
        _ry = _rx;
}

void MgRoundRect::_copy(const MgRoundRect& src)
{
    _rx = src._rx;
    _ry = src._ry;
    __super::_copy(src);
}

bool MgRoundRect::_equals(const MgRoundRect& src) const
{
    return mgIsZero(_rx - src._rx)
        && mgIsZero(_ry - src._ry)
        && __super::_equals(src);
}

void MgRoundRect::_clear()
{
    _rx = _ry = 0.0;
    __super::_clear();
}

float MgRoundRect::_hitTest(const Point2d& pt, float tol, 
                            Point2d& ptNear, Int32& segment) const
{
    float dist;

    if (isOrtho())
    {
        dist = mgRoundRectHit(Box2d(_points[0], _points[2]), _rx, _ry, pt, tol, ptNear, segment);
    }
    else
    {
        Matrix2d mat(Matrix2d::rotation(getAngle(), getCenter()));
        Box2d rect(Box2d(pt, 2 * tol, 2 * tol) * mat.inverse());

        dist = mgRoundRectHit(getRect(), _rx, _ry, 
            rect.center(), rect.width(), ptNear, segment);
        if (dist < 1e10)
            ptNear *= mat;
    }

    return dist;
}

bool MgRoundRect::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;

    if (isOrtho())
    {
        ret = gs.drawRoundRect(&ctx, Box2d(_points[0], _points[2]), _rx, _ry);
    }
    else
    {
        GiSaveModelTransform xf(&gs.xf(), Matrix2d::rotation(getAngle(), getCenter()));
        ret = gs.drawRoundRect(&ctx, getRect(), _rx, _ry);
    }

    return __super::_draw(gs, ctx) || ret;
}
