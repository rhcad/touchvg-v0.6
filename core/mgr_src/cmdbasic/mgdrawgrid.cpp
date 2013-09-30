// mgdrawgrid.cpp: 实现网格绘图命令类 MgCmdDrawGrid
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawgrid.h"
#include <mgshapet.h>
#include <mggrid.h>

bool MgCmdDrawGrid::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgGrid>::create, sender);
}

bool MgCmdDrawGrid::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (getStep() == 2) {
        gs->drawHandle(dynshape()->shape()->getHandlePoint(8), 0);
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
    
    dynshape()->shape()->setHandlePoint(8, snapPoint(sender), 0);
    dynshape()->shape()->update();
    
    return _touchMoved(sender);
}

bool MgCmdDrawGrid::touchEnded(const MgMotion* sender)
{
    if (m_step == 1) {
        return MgCmdDrawRect::touchEnded(sender);
    }
    
    if ( ((MgGrid*)dynshape()->shape())->valid(sender->displayMmToModel(1.f)) ) {
        dynshape()->context()->setNoFillColor();
        _addshape(sender);
        _delayClear();
        sender->cancel();
    }
    else {
        dynshape()->shape()->setHandlePoint(8, dynshape()->shape()->getPoint(3), 0);
        m_step = 2;
    }
    
    return _touchEnded(sender);
}

void MgCmdDrawGrid::addRectShape(const MgMotion* sender)
{
    m_step = 2;
    sender->view->redraw();
}
