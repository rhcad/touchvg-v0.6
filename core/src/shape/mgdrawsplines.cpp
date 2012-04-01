// mgdrawsplines.cpp: 实现曲线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgdrawsplines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>

MgCmdDrawSplines::MgCmdDrawSplines()
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
    enableRecall = true;
    if (m_step > 2) {                   // 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)m_shape->shape())->removePoint(m_step - 1);
        m_shape->shape()->update();
    }
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawSplines::draw(const MgMotion* sender, GiGraphics* gs)
{
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawSplines::touchBegan(const MgMotion* sender)
{
    ((MgBaseLines*)m_shape->shape())->resize(2);
    m_step = 1;
    m_shape->shape()->setPoint(0, sender->startPointM);
    m_shape->shape()->setPoint(1, sender->pointM);
    m_shape->shape()->update();
    
    return _touchBegan(sender);
}

bool MgCmdDrawSplines::touchMoved(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)m_shape->shape();
    
    double closelen  = sender->view->xform()->displayToModel(20 + getLineHalfWidth(m_shape, sender->view->graph()));
    double closedist = sender->pointM.distanceTo(m_shape->shape()->getPoint(0));
    bool   closed    = (m_step > 2 && closedist < closelen
                        && m_shape->shape()->getExtent().width() > closedist * 1.5
                        && m_shape->shape()->getExtent().height() > closedist * 1.5);
    
    if (m_step > 2 && m_shape->shape()->isClosed() != closed) {
        lines->setClosed(closed);
        if (closed)
            lines->removePoint(m_step);
        else
            lines->addPoint(sender->pointM);
    }
    if (!closed) {
        m_shape->shape()->setPoint(m_step, sender->pointM);
        if (m_step > 0 && canAddPoint(sender, false)) {
            m_step++;
            if (m_step >= m_shape->shape()->getPointCount()) {
                ((MgBaseLines*)m_shape->shape())->addPoint(sender->pointM);
            }
        }
    }
    m_shape->shape()->update();
    
    return _touchMoved(sender);
}

bool MgCmdDrawSplines::touchEnded(const MgMotion* sender)
{
    MgSplines* splines = (MgSplines*)m_shape->shape();
    
    if (m_step > 1) {
        int w = getLineHalfWidth(m_shape, sender->view->graph());
        splines->smooth(sender->view->xform()->displayToModel(3 + w));
        _addshape(sender);
    }
    
    m_shape->shape()->clear();
    m_step = 0;
    
    return _touchEnded(sender);
}

bool MgCmdDrawSplines::canAddPoint(const MgMotion* /*sender*/, bool /*ended*/)
{
    return true;
}
