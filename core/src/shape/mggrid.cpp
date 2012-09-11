// mggrid.cpp: 实现网格图形类 MgGrid
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mggrid.h"
#include <mgshape_.h>
#include <mgstorage.h>

MG_IMPLEMENT_CREATE(MgGrid)

MgGrid::MgGrid()
{
    __super::setRotateDisnable(true);
}

MgGrid::~MgGrid()
{
}

void MgGrid::_copy(const MgGrid& src)
{
    __super::_copy(src);
}

bool MgGrid::_equals(const MgGrid& src) const
{
    return __super::_equals(src);
}

void MgGrid::_clear()
{
    __super::_clear();
}

void MgGrid::setRotateDisnable(bool)
{
    __super::setRotateDisnable(true);
}

bool MgGrid::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    GiContext ctxgrid(ctx);
    ctxgrid.setNoFillColor();
    
    bool ret = gs.drawPolygon(&ctxgrid, 4, _points);
    
    ctxgrid.setLineWidth(0);
    ctxgrid.setLineStyle(kGiLineDot);
    
    float xstep = getWidth() / 5.f;
    float ystep = getHeight() / 5.f;
    Vector2d vecx ((getPoint(1) - getPoint(0)).setLength(xstep));
    Vector2d vecy ((getPoint(3) - getPoint(0)).setLength(ystep));
    
    Point2d pts[2] = { getPoint(0), getPoint(3) };
    for (float x = xstep; x < getWidth() - _MGZERO; x += xstep) {
        pts[0] += vecx;
        pts[1] += vecx;
        gs.drawLine(&ctxgrid, pts[0], pts[1]);
    }
    
    pts[0] = getPoint(0);
    pts[1] = getPoint(1);
    for (float y = ystep; y < getHeight() - _MGZERO; y += ystep) {
        pts[0] += vecy;
        pts[1] += vecy;
        gs.drawLine(&ctxgrid, pts[0], pts[1]);
    }
    
    return __super::_draw(gs, ctx) || ret;
}

bool MgGrid::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    return ret;
}

bool MgGrid::_load(MgStorage* s)
{
    bool ret = __super::_load(s);
    return ret;
}

int MgGrid::snap(Point2d& pnt, float& distx, float& disty)
{
    int ret = 0;
    float xstep = getWidth() / 5.f;
    float ystep = getHeight() / 5.f;
    
    for (float x = 0; x < getWidth() + _MGZERO; x += xstep) {
        Point2d base(getPoint(3) + Vector2d(x, 0.f));
        
        if (distx > fabs(pnt.x - base.x)) {
            distx = (float)fabs(pnt.x - base.x);
            pnt.x = base.x;
            ret |= 1;
        }
    }
    for (float y = 0; y < getHeight() + _MGZERO; y += ystep) {
        Point2d base(getPoint(3) + Vector2d(0.f, y));
        if (disty > fabs(pnt.y - base.y)) {
            disty = (float)fabs(pnt.y - base.y);
            pnt.y = base.y;
            ret |= 2;
        }
    }
    
    return ret;
}
