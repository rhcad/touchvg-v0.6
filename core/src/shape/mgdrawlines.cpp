// mgdrawlines.cpp: 实现折线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgdrawlines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>

MgCmdDrawLines::MgCmdDrawLines()
{
}

MgCmdDrawLines::~MgCmdDrawLines()
{
}

bool MgCmdDrawLines::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgLines>::create, sender);
}

MgCmdBaseLines::MgCmdBaseLines()
{
}

MgCmdBaseLines::~MgCmdBaseLines()
{
}

bool MgCmdBaseLines::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 1) {
        GiContext ctxaux(0, GiColor(64, 64, 64, 128), kLineSolid, GiColor(0, 64, 64, 168));
        double radius = gs->xf().displayToModel(5);
        
        for (UInt32 i = 0; i < m_shape->shape()->getPointCount(); i++) {
            gs->drawEllipse(&ctxaux, m_shape->shape()->getPoint(i), radius);
        }
    }
    
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdBaseLines::touchBegan(const MgMotion* sender)
{
    ((MgBaseLines*)m_shape->shape())->resize(2);
    m_step = 1;
    m_shape->shape()->setPoint(0, sender->pointM);
    m_shape->shape()->setPoint(1, sender->pointM);
    m_shape->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdBaseLines::touchMoved(const MgMotion* sender)
{
    m_shape->shape()->setPoint(m_step, sender->pointM);

    if (m_step > 0 && canAddPoint(sender)) {
        m_step++;
        if (m_step >= m_shape->shape()->getPointCount()) {
            ((MgBaseLines*)m_shape->shape())->addPoint(sender->pointM);
        }
    }
    m_shape->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdBaseLines::touchEnded(const MgMotion* sender)
{
    m_shape->shape()->setPoint(m_step, sender->pointM);
    m_shape->shape()->update();

    if (canAddPoint(sender)) {
        ((MgBaseLines*)m_shape->shape())->removePoint(m_step);
    }

    if (m_step > 2 && canAddShape(sender)) {
        _addshape(sender);
    }

    m_shape->shape()->clear();
    m_step = 0;

    return _touchEnded(sender);
}

const int       MIN_DIST_TWO_POINTS = 20;

bool MgCmdBaseLines::canAddPoint(const MgMotion* sender)
{
    Point2d endPtM   = m_shape->shape()->getPoint(m_step - 1);
    Point2d endPt    = endPtM * sender->view->xform()->modelToDisplay();
    double distToEnd = endPt.distanceTo(Point2d(sender->point.x, sender->point.y));
    double turnAngle = 90;

    if (m_step > 1)
    {
        Point2d lastPtM = m_shape->shape()->getPoint(m_step - 2);
        turnAngle = (endPtM - lastPtM).angleTo(sender->pointM - endPtM);
        turnAngle = mgRad2Deg(fabs(turnAngle));
    }

    if (distToEnd < MIN_DIST_TWO_POINTS
        || sin(turnAngle) * distToEnd < MIN_DIST_TWO_POINTS)
    {
        return false;
    }

    return true;
}

bool MgCmdBaseLines::canAddShape(const MgMotion* /*sender*/)
{
    return true;
}
