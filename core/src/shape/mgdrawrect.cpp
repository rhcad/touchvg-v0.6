// mgdrawrect.cpp: 实现矩形绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawrect.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCmdDrawRect::MgCmdDrawRect()
{
}

MgCmdDrawRect::~MgCmdDrawRect()
{
}

bool MgCmdDrawRect::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgRect>::create, sender);
}

bool MgCmdDrawRect::undo(bool &, const MgMotion* sender)
{
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawRect::touchBegan(const MgMotion* sender)
{
    m_step = 1;
    m_startPt = snapPoint(sender, true);
    ((MgBaseRect*)dynshape()->shape())->setRect(m_startPt, m_startPt);
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawRect::touchMoved(const MgMotion* sender)
{
    Point2d pt1(m_startPt);
    Point2d pt2(snapPoint(sender));
    MgBaseRect* shape = (MgBaseRect*)dynshape()->shape();
    
    if (shape->getFlag(kMgSquare)) {
        float len = (float)mgMax(fabs(pt2.x - pt1.x), fabs(pt2.y - pt1.y));
        Box2d rect(m_startPt, 2.f * len, 0);
        pt1 = rect.leftTop();
        pt2 = rect.rightBottom();
    }
    shape->setRect(pt1, pt2);
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawRect::touchEnded(const MgMotion* sender)
{
    Point2d pt1(m_startPt);
    Point2d pt2(snapPoint(sender));
    MgBaseRect* shape = (MgBaseRect*)dynshape()->shape();
    
    if (shape->getFlag(kMgSquare)) {
        float len = (float)mgMax(fabs(pt2.x - pt1.x), fabs(pt2.y - pt1.y));
        Box2d rect(m_startPt, 2.f * len, 0);
        pt1 = rect.leftTop();
        pt2 = rect.rightBottom();
    }
    shape->setRect(pt1, pt2);
    dynshape()->shape()->update();

    float minDist = mgDisplayMmToModel(5, sender);

    if (shape->getWidth() > minDist && shape->getHeight() > minDist) {
        addRectShape(sender);
    }

    return _touchEnded(sender);
}

void MgCmdDrawRect::addRectShape(const MgMotion* sender)
{
    _addshape(sender);
    _delayClear();
}

bool MgCmdDrawEllipse::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgEllipse>::create, sender);
}

bool MgCmdDrawEllipse::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 0 && sender->dragging) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), kGiLineDash);
        bool antiAlias = gs->setAntiAliasMode(false);
        gs->drawRect(&ctxshap, dynshape()->shape()->getExtent());
        gs->setAntiAliasMode(antiAlias);
    }
    return MgCmdDrawRect::draw(sender, gs);
}

bool MgCmdDrawDiamond::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgDiamond>::create, sender);
}

bool MgCmdDrawSquare::initialize(const MgMotion* sender)
{
    bool ret = _initialize(MgShapeT<MgRect>::create, sender);
    
    MgBaseRect* rect = (MgBaseRect*)dynshape()->shape();
    rect->setSquare(true);
    
    return ret;
}

bool MgCmdDrawCircle::initialize(const MgMotion* sender)
{
    bool ret = _initialize(MgShapeT<MgEllipse>::create, sender);
    
    MgBaseRect* rect = (MgBaseRect*)dynshape()->shape();
    rect->setSquare(true);
    
    return ret;
}

#include "mggrid.h"

bool MgCmdDrawGrid::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgGrid>::create, sender);
}

bool MgCmdDrawGrid::cancel(const MgMotion* sender)
{
    if (m_step > 1) {
        _addshape(sender);
        _delayClear();
    }
    return MgCmdDrawRect::cancel(sender);
}

bool MgCmdDrawGrid::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step == 2) {
        sender->view->drawHandle(gs, dynshape()->shape()->getHandlePoint(4), false);
    }
    return MgCmdDrawRect::draw(sender, gs);
}

bool MgCmdDrawGrid::touchBegan(const MgMotion* sender)
{
    if (m_step == 0) {
        return MgCmdDrawRect::touchBegan(sender);
    }
    m_step = 3;
    return _touchBegan(sender);
}

bool MgCmdDrawGrid::touchMoved(const MgMotion* sender)
{
    if (m_step == 1) {
        return MgCmdDrawRect::touchMoved(sender);
    }
    
    dynshape()->shape()->setHandlePoint(4, snapPoint(sender), 0);
    dynshape()->shape()->update();
    
    return _touchMoved(sender);
}

bool MgCmdDrawGrid::touchEnded(const MgMotion* sender)
{
    if (m_step == 1) {
        return MgCmdDrawRect::touchEnded(sender);
    }
    
    _addshape(sender);
    _delayClear();
    
    return _touchEnded(sender);
}

void MgCmdDrawGrid::addRectShape(const MgMotion* sender)
{
    m_step = 2;
    sender->view->redraw(false);
}
