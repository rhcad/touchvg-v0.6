// mgdrawlines.cpp: 实现折线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawlines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>

// MgCmdDrawLines
//

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

bool MgCmdDrawLines::undo(bool &enableRecall, const MgMotion* sender)
{
    enableRecall = true;
    if (m_step > 2) {                   // 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)dynshape()->shape())->removePoint(m_step - 1);
        dynshape()->shape()->update();
    }
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawLines::touchBegan(const MgMotion* sender)
{
    if (0 == m_step) {
        m_step = 1;
        ((MgBaseLines*)dynshape()->shape())->resize(2);
        dynshape()->shape()->setPoint(0, sender->startPointM);
        dynshape()->shape()->setPoint(1, sender->pointM);
    }
    else {
        dynshape()->shape()->setPoint(m_step, autoAlignPoint(sender));
    }
    dynshape()->shape()->update();
    
    return _touchBegan(sender);
}

bool MgCmdDrawLines::touchMoved(const MgMotion* sender)
{
    Point2d pnt(autoAlignPoint(sender));
    float distmin = mgDisplayMmToModel(2.f, sender);
    bool closed = m_step > 2 && pnt.distanceTo(dynshape()->shape()->getPoint(0)) < distmin;
    
    dynshape()->shape()->setPoint(m_step, pnt);
    ((MgBaseLines*)dynshape()->shape())->setClosed(closed);
    
    dynshape()->shape()->update();
    
    return _touchMoved(sender);
}

bool MgCmdDrawLines::touchEnded(const MgMotion* sender)
{
    Point2d pnt(autoAlignPoint(sender));
    float distmin = mgDisplayMmToModel(2.f, sender);
    bool closed = m_step > 2 && pnt.distanceTo(dynshape()->shape()->getPoint(0)) < distmin;
    
    dynshape()->shape()->setPoint(m_step, pnt);
    ((MgBaseLines*)dynshape()->shape())->setClosed(closed);
    
    dynshape()->shape()->update();
    
    if (pnt.distanceTo(dynshape()->shape()->getPoint(m_step - 1)) > distmin) {
        if (closed) {
            ((MgBaseLines*)dynshape()->shape())->removePoint(m_step);
            _addshape(sender);
            _delayClear();
            m_step = 0;
        }
        else if (++m_step >= dynshape()->shape()->getPointCount()) {
            ((MgBaseLines*)dynshape()->shape())->addPoint(pnt);
        }
    }
    
    return _touchEnded(sender);
}

bool MgCmdDrawLines::click(const MgMotion* sender)
{
    return touchBegan(sender) && touchEnded(sender);
}

bool MgCmdDrawLines::doubleClick(const MgMotion* sender)
{
    if (m_step > 1) {
        _addshape(sender);
        _delayClear();
        m_step = 0;
    }
    return true;
}

// MgCmdDrawFreeLines
//

MgCmdDrawFreeLines::MgCmdDrawFreeLines()
{
}

MgCmdDrawFreeLines::~MgCmdDrawFreeLines()
{
}

bool MgCmdDrawFreeLines::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgLines>::create, sender);
}

bool MgCmdDrawFreeLines::undo(bool &enableRecall, const MgMotion* sender)
{
    enableRecall = true;
    if (m_step > 2) {                   // 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)dynshape()->shape())->removePoint(m_step - 1);
        dynshape()->shape()->update();
    }
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawFreeLines::draw(const MgMotion* sender, GiGraphics* gs)
{
    /*if (m_step > 1) {
        GiContext ctxaux(0, GiColor(64, 64, 64, 128), kGiLineSolid, GiColor(0, 64, 64, 168));
        float radius = gs->xf().displayToModel(3);
        
        for (UInt32 i = 0; i < dynshape()->shape()->getPointCount(); i++) {
            gs->drawEllipse(&ctxaux, dynshape()->shape()->getPoint(i), radius);
        }
    }*/
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawFreeLines::touchBegan(const MgMotion* sender)
{
    ((MgBaseLines*)dynshape()->shape())->resize(2);
    m_step = 1;
    dynshape()->shape()->setPoint(0, sender->startPointM);
    dynshape()->shape()->setPoint(1, sender->pointM);
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawFreeLines::touchMoved(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    float closelen  = mgLineHalfWidthModel(dynshape(), sender) + mgDisplayMmToModel(5, sender);
    float closedist = sender->pointM.distanceTo(dynshape()->shape()->getPoint(0));
    bool  closed    = (m_step > 2 && closedist < closelen
        && dynshape()->shape()->getExtent().width() > closedist * 1.5f
        && dynshape()->shape()->getExtent().height() > closedist * 1.5f);
    
    if (m_step > 2 && dynshape()->shape()->isClosed() != closed) {
        lines->setClosed(closed);
        if (closed)
            lines->removePoint(m_step);
        else
            lines->addPoint(sender->pointM);
    }
    if (!closed) {
        dynshape()->shape()->setPoint(m_step, sender->pointM);
        if (m_step > 0 && canAddPoint(sender, false)) {
            m_step++;
            if (m_step >= dynshape()->shape()->getPointCount()) {
                ((MgBaseLines*)dynshape()->shape())->addPoint(sender->pointM);
            }
        }
    }
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawFreeLines::touchEnded(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    float closelen  = mgLineHalfWidthModel(dynshape(), sender) + mgDisplayMmToModel(5, sender);
    float closedist = sender->pointM.distanceTo(dynshape()->shape()->getPoint(0));
    bool  closed    = (m_step > 2 && closedist < closelen
        && dynshape()->shape()->getExtent().width() > closedist * 1.5f
        && dynshape()->shape()->getExtent().height() > closedist * 1.5f);
    
    if (m_step > 2 && dynshape()->shape()->isClosed() != closed) {
        lines->setClosed(closed);
        if (closed)
            lines->removePoint(m_step);
        else
            lines->addPoint(sender->pointM);
    }
    if (!closed) {
        dynshape()->shape()->setPoint(m_step, sender->pointM);
        if (m_step > 0 && !canAddPoint(sender, true))
            lines->removePoint(m_step);
    }
    dynshape()->shape()->update();
    
    if (m_step > 1) {
        _addshape(sender);
    }
    else {
        click(sender);  // add a point
    }
    _delayClear();

    return _touchEnded(sender);
}

bool MgCmdDrawFreeLines::canAddPoint(const MgMotion* /*sender*/, bool /*ended*/)
{
    /*float minDist = mgDisplayMmToModel(3, sender);
    Point2d endPt  = dynshape()->shape()->getPoint(m_step - 1);
    float distToEnd = endPt.distanceTo(sender->pointM);
    float turnAngle = 90;
    
    if (m_step > 1)
    {
        Point2d lastPt = dynshape()->shape()->getPoint(m_step - 2);
        turnAngle = (endPt - lastPt).angleTo(sender->pointM - endPt);
        turnAngle = mgRad2Deg(fabs(turnAngle));
    }
    
    if (distToEnd < minDist * (ended ? 0.25 : 1))
        return false;
    if (!ended && sin(turnAngle) * distToEnd < 5)
        return false;*/
    
    return true;
}
