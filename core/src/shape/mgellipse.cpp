// mgellipse.cpp: 实现椭圆图形类 MgEllipse
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "mgbasicsp.h"
#include <_mgshape.h>
#include <mgnear.h>
#include <mgcurv.h>

MG_IMPLEMENT_CREATE(MgEllipse)

MgEllipse::MgEllipse()
{
}

MgEllipse::~MgEllipse()
{
}

double MgEllipse::getRadiusX() const
{
    return getWidth() / 2;
}

double MgEllipse::getRadiusY() const
{
    return getHeight() / 2;
}

void MgEllipse::setRadius(double rx, double ry)
{
    rx = fabs(rx);
    rx = fabs(ry);
    if (rx < _MGZERO)
        rx = rx;

    setRect(Box2d(getCenter(), rx * 2, ry * 2), getAngle());
}

void MgEllipse::_update()
{
    __super::_update();

    mgEllipseToBezier(_bzpts, getCenter(), getWidth() / 2, getHeight() / 2);

    Matrix2d mat(Matrix2d::rotation(getAngle(), getCenter()));
    for (int i = 0; i < 13; i++)
        _bzpts[i] *= mat;

    mgBeziersBox(_extent, 13, _bzpts, true);
}

double MgEllipse::_hitTest(const Point2d& pt, double tol, 
                           Point2d& ptNear, Int32& segment) const
{
    double distMin = _DBL_MAX;
    const Box2d rect (pt, 2 * tol, 2 * tol);
    Point2d ptTemp;

    segment = -1;
    for (int i = 0; i < 4; i++)
    {
        if (rect.isIntersect(Box2d(4, _bzpts + 3 * i)))
        {
            mgNearestOnBezier(pt, _bzpts + 3 * i, ptTemp);
            double dist = pt.distanceTo(ptTemp);
            if (dist <= tol && dist < distMin)
            {
                distMin = dist;
                ptNear = ptTemp;
                segment = i;
            }
        }
    }

    return distMin;
}

bool MgEllipse::_hitTestBox(const Box2d& rect) const
{
    if (!getExtent().isIntersect(rect))
        return false;
    
    return mgBeziersIntersectBox(rect, 13, _bzpts, true);
}

bool MgEllipse::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;

    if (isOrtho())
    {
        ret = gs.drawEllipse(&ctx, Box2d(_points[0], _points[2]));
    }
    else
    {
        ret = gs.drawBeziers(&ctx, 13, _bzpts);
    }

    return __super::_draw(gs, ctx) || ret;
}
