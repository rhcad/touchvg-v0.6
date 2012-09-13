// mggrid.cpp: 实现网格图形类 MgGrid
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mggrid.h"
#include <mgshape_.h>
#include <mgstorage.h>

MG_IMPLEMENT_CREATE(MgGrid)

MgGrid::MgGrid()
{
    setFlag(kMgRotateDisnable, true);
}

MgGrid::~MgGrid()
{
}

void MgGrid::_copy(const MgGrid& src)
{
    m_cell = src.m_cell;
    __super::_copy(src);
}

bool MgGrid::_equals(const MgGrid& src) const
{
    return m_cell == src.m_cell && __super::_equals(src);
}

void MgGrid::_clear()
{
    m_cell = Vector2d::kIdentity();
    __super::_clear();
}

void MgGrid::setFlag(MgShapeBit bit, bool on)
{
    __super::setFlag(bit, bit == kMgRotateDisnable || on);
}

UInt32 MgGrid::_getHandleCount() const
{
    return 5;
}

Point2d MgGrid::_getHandlePoint(UInt32 index) const
{
    return (index < 4 ? __super::_getHandlePoint(index) : getPoint(3) +
            (m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell));
}

bool MgGrid::_setHandlePoint(UInt32 index, const Point2d& pt, float tol)
{
    if (index < 4) {
        return __super::_setHandlePoint(index, pt, tol);
    }
    
    float cx = (float)fabs(pt.x - getPoint(3).x);
    float cy = (float)fabs(pt.y - getPoint(3).y);
    cx = mgMax(mgMax(cx, tol / 3.f), cy);
    m_cell.set(cx, cx);
    
    return true;
}

bool MgGrid::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    Vector2d cell(m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell);
    int nx = (int)(getWidth() / cell.x + _MGZERO);
    int ny = (int)(getHeight() / cell.y + _MGZERO);
    Box2d rect(getPoint(3), getPoint(3) + Vector2d((float)(cell.x * nx), (float)(cell.y * ny)));
    
    GiContext ctxgrid(0, ctx.getLineColor());
    int ret = 0;
    bool switchx = (nx >= 10 && cell.x < gs.xf().displayToModel(8, true));
    bool switchy = (ny >= 10 && cell.y < gs.xf().displayToModel(8, true));
    Point2d pts[2] = { rect.leftTop(), rect.leftBottom() };
    
    for (int i = 0; i <= nx; i++) {
        if (switchx) {
            ctxgrid.setLineStyle(i % 5 == 0 ? kGiLineSolid : kGiLineDot);
        }
        ret += gs.drawLine(&ctxgrid, pts[0], pts[1]) ? 1 : 0;
        pts[0].x += cell.x;
        pts[1].x += cell.x;
    }
    
    pts[0] = rect.leftBottom();
    pts[1] = rect.rightBottom();
    ctxgrid.setLineStyle(kGiLineSolid);
    for (int j = 0; j <= ny; j++) {
        if (switchy) {
            ctxgrid.setLineStyle(j % 5 == 0 ? kGiLineSolid : kGiLineDot);
        }
        ret += gs.drawLine(&ctxgrid, pts[0], pts[1]) ? 1 : 0;
        pts[0].y += cell.y;
        pts[1].y += cell.y;
    }
    
    return __super::_draw(gs, ctx) || ret > 0;
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
    Point2d newpt(pnt);
    Point2d org(getPoint(3));
    Vector2d cell(m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell);
    
    distx *= 3;
    disty *= 3;
    
    for (float x = cell.x; x < getWidth() - _MGZERO; x += cell.x) {        
        if (distx > fabs(pnt.x - (org.x + x))) {
            newpt.x = org.x + x;
            distx = (float)fabs(pnt.x - newpt.x);
            ret |= 1;
        }
    }
    for (float y = cell.y; y < getHeight() - _MGZERO; y += cell.y) {
        if (disty > fabs(pnt.y - (org.y + y))) {
            newpt.y = org.y + y;
            disty = (float)fabs(pnt.y - newpt.y);
            ret |= 2;
        }
    }
    
    pnt = newpt;
    if ((ret & 1) == 0)
        distx /= 3;
    if ((ret & 2) == 0)
        disty /= 3;
    
    return ret;
}
