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
    
    if ( ((MgBaseRect*)dynshape()->shape())->getFlag(kMgSquare) ) {
        float len = (float)mgMax(pt2.x - pt1.x, pt2.y - pt1.y);
        Box2d rect(m_startPt, 2.f * len, 0);
        pt1 = rect.leftTop();
        pt2 = rect.rightBottom();
    }
    ((MgBaseRect*)dynshape()->shape())->setRect(pt1, pt2);
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawRect::touchEnded(const MgMotion* sender)
{
    Point2d pt1(m_startPt);
    Point2d pt2(snapPoint(sender));
    MgBaseRect* shape = (MgBaseRect*)dynshape()->shape();
    
    if (shape->getFlag(kMgSquare)) {
        float len = (float)mgMax(pt2.x - pt1.x, pt2.y - pt1.y);
        Box2d rect(m_startPt, 2.f * len, 0);
        pt1 = rect.leftTop();
        pt2 = rect.rightBottom();
    }
    shape->setRect(pt1, pt2);
    dynshape()->shape()->update();

    float minDist = mgDisplayMmToModel(5, sender);

    if (shape->getWidth() > minDist && shape->getHeight() > minDist) {
        _addshape(sender);
    }
    _delayClear();

    return _touchEnded(sender);
}

bool MgCmdDrawEllipse::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgEllipse>::create, sender);
}

bool MgCmdDrawEllipse::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 0) {
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
