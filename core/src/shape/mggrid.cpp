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
    return (getFlag(kMgFixedLength) || getFlag(kMgShapeLocked)) ? 4 : 5;
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
    cx = (float)( (int)(cx * 100) / 10 ) * 0.1f;
    m_cell.set(cx, cx);
    
    return true;
}

bool MgGrid::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    Vector2d cell(m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell / 2);
    if (cell.x < _MGZERO || cell.y < _MGZERO) {
        return __super::_draw(gs, ctx);
    }
    
    int nx = (int)(getWidth() / cell.x + _MGZERO);
    int ny = (int)(getHeight() / cell.y + _MGZERO);
    Box2d rect(getPoint(3), getPoint(3) + Vector2d((float)(cell.x * nx), (float)(cell.y * ny)));
    
    float w = gs.calcPenWidth(ctx.getLineWidth(), ctx.isAutoScale()) / -2.f;
    GiContext ctxgrid(w, ctx.getLineColor());
    
    bool antiAlias = gs.setAntiAliasMode(false);
    int ret = gs.drawRect(&ctxgrid, rect) ? 1 : 0;
    
    bool switchx = (nx >= 10 && cell.x < gs.xf().displayToModel(20, true));
    bool switchy = (ny >= 10 && cell.y < gs.xf().displayToModel(20, true));
    Point2d pts[2] = { rect.leftTop(), rect.leftBottom() };
    
    for (int i = 1; i < nx; i++) {
        pts[0].x += cell.x;
        pts[1].x += cell.x;
        ctxgrid.setLineWidth(!switchx || i%5 > 0 ? w/2 : w, false);
        ctxgrid.setLineAlpha(-w < 0.9f && (!switchx || i%5 > 0) ?
            ctx.getLineAlpha() / 2 : ctx.getLineAlpha());
        ret += gs.drawLine(&ctxgrid, pts[0], pts[1]) ? 1 : 0;
    }
    
    pts[0] = rect.leftBottom();
    pts[1] = rect.rightBottom();
    for (int j = 1; j < ny; j++) {
        pts[0].y += cell.y;
        pts[1].y += cell.y;
        ctxgrid.setLineWidth(!switchy || j%5 > 0 ? w/2 : w, false);
        ctxgrid.setLineAlpha(-w < 0.9f && (!switchy || j%5 > 0) ?
            ctx.getLineAlpha() / 2 : ctx.getLineAlpha());
        ret += gs.drawLine(&ctxgrid, pts[0], pts[1]) ? 1 : 0;
    }

    gs.setAntiAliasMode(antiAlias);
    
    return __super::_draw(gs, ctx) || ret > 0;
}

bool MgGrid::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloat("cellw", m_cell.x);
    s->writeFloat("celly", m_cell.y);
    return ret;
}

bool MgGrid::_load(MgStorage* s)
{
    bool ret = __super::_load(s);
    m_cell.set(s->readFloat("cellw", m_cell.x), s->readFloat("celly", m_cell.y));
    return ret;
}

int MgGrid::snap(Point2d& pnt, float& distx, float& disty)
{
    int ret = 0;
    Point2d newpt(pnt);
    Point2d org(getPoint(3));
    Vector2d cell(m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell / 2);
    
    if (cell.x < _MGZERO || cell.y < _MGZERO)
        return ret;
    
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
