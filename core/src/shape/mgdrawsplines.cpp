// mgdrawsplines.cpp: 实现曲线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawsplines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>

MgCmdDrawSplines::MgCmdDrawSplines() : m_freehand(true)
{
}

MgCmdDrawSplines::~MgCmdDrawSplines()
{
}

bool MgCmdDrawSplines::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgSplines>::create, sender);
}

bool MgCmdDrawSplines::undo(bool &enableRecall, const MgMotion* sender)
{
    enableRecall = m_freehand;
    if (m_step > 1) {                   // freehand: 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)m_shape->shape())->removePoint(m_freehand ? m_step - 1 : m_step);
        m_shape->shape()->update();
    }
    
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawSplines::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 0 && !m_freehand) {
        GiContext ctx(0, GiColor(64, 128, 64, 172), kLineSolid, GiColor(0, 64, 64, 128));
        float radius = gs->xf().displayToModel(4);
        
        for (UInt32 i = 1, n = m_shape->shape()->getPointCount(); i < 6 && n >= i; i++) {
            gs->drawEllipse(&ctx, m_shape->shape()->getPoint(n - i), radius);
        }
        gs->drawEllipse(&ctx, m_shape->shape()->getPoint(0), radius * 1.5f);
    }
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawSplines::touchBegan(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)m_shape->shape();
    
    if (m_step > 0 && !m_freehand) {
        m_step++;
        if (m_step >= m_shape->shape()->getPointCount()) {
            lines->addPoint(sender->pointM);
            m_shape->shape()->update();
        }
        
        return _touchMoved(sender);
    }
    else {
        lines->resize(2);
        m_freehand = !sender->pressDrag;
        m_step = 1;
        m_shape->shape()->setPoint(0, sender->startPointM);
        m_shape->shape()->setPoint(1, sender->pointM);
        m_shape->shape()->update();
        
        return _touchBegan(sender);
    }
}

bool MgCmdDrawSplines::touchMoved(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)m_shape->shape();
    
    m_shape->shape()->setPoint(m_step, sender->pointM);
    if (m_step > 0 && canAddPoint(sender, false)) {
        m_step++;
        if (m_step >= m_shape->shape()->getPointCount()) {
            lines->addPoint(sender->pointM);
        }
    }
    m_shape->shape()->update();
    
    return _touchMoved(sender);
}

bool MgCmdDrawSplines::touchEnded(const MgMotion* sender)
{
    if (m_freehand) {
        if (m_step > 1) {
            //MgSplines* splines = (MgSplines*)m_shape->shape();
            //splines->smooth(mgLineHalfWidthModel(m_shape, sender) + mgDisplayMmToModel(1, sender));
            _addshape(sender);
        }
        else {
            click(sender);  // add a point
        }
        _delayClear();
    }
    
    return _touchEnded(sender);
}

bool MgCmdDrawSplines::doubleClick(const MgMotion* sender)
{
    if (!m_freehand) {
        if (m_step > 1) {
            _addshape(sender);
        }
        _delayClear();
        return true;
    }
    return click(sender);
}

bool MgCmdDrawSplines::canAddPoint(const MgMotion* sender, bool ended)
{
    if (!m_freehand && !ended)
        return false;
    
    if (m_step > 0 && mgDisplayMmToModel(ended ? 0.2f : 0.5f, sender)
        > sender->pointM.distanceTo(m_shape->shape()->getPoint(m_step - 1))) {
        return false;
    }
    
    return true;
}

bool MgCmdDrawSplines::click(const MgMotion* sender)
{
    if (m_freehand) {
        MgShapeT<MgLine> line;
        
        if (sender->view->context()) {
            *line.context() = *sender->view->context();
        }
        
        line.shape()->setPoint(0, sender->startPointM);
        line.shape()->setPoint(1, sender->pointM);
        
        if (sender->view->shapeWillAdded(&line)) {
            _addshape(sender, &line);
        }
        
        return true;
    }
    return MgCommandDraw::click(sender);
}
