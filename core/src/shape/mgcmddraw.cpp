// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmddraw.h"
#include <assert.h>
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCommandDraw::MgCommandDraw() : m_shape(NULL), m_step(0)
{
}

MgCommandDraw::~MgCommandDraw()
{
    if (m_shape) {
        m_shape->release();
        m_shape = NULL;
    }
}

bool MgCommandDraw::cancel(const MgMotion* sender)
{
    if (m_step > 0) {
        m_step = 0;
        m_shape->shape()->clear();
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::_initialize(MgShape* (*creator)(), const MgMotion* sender)
{
    if (!m_shape)
    {
        m_shape = creator();
        assert(m_shape && m_shape->shape());
    }
    m_step = 0;
    m_shape->shape()->clear();
    if (sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }

    return true;
}

bool MgCommandDraw::_addshape(const MgMotion* sender)
{
    bool ret = sender->view->shapeWillAdded(m_shape);

    if (ret) {
        sender->view->shapes()->addShape(*m_shape);
        sender->view->regen();
    }
    if (sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }

    return ret;
}

bool MgCommandDraw::_undo(const MgMotion* sender)
{
    if (m_step > 1) {
        m_step--;
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::draw(const MgMotion* /*sender*/, GiGraphics* gs)
{
    return m_step > 0 && m_shape->draw(*gs);
}

bool MgCommandDraw::click(const MgMotion* sender)
{
    MgShapeT<MgLine> line;
    double pxlen = sender->view->xform()->displayToModel(1);
    
    if (sender->view->context()) {
        *line.context() = *sender->view->context();
    }
    
    line.shape()->setPoint(0, sender->pointM);
    line.shape()->setPoint(1, sender->pointM + Vector2d(pxlen, pxlen));
        
    if (sender->view->shapeWillAdded(&line)) {
        sender->view->shapes()->addShape(line);
        sender->view->regen();
    }
    
    return true;
}

bool MgCommandDraw::doubleClick(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandDraw::longPress(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandDraw::_touchBegan(const MgMotion* sender)
{
    if (sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::_touchMoved(const MgMotion* sender)
{
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::_touchEnded(const MgMotion* sender)
{
    sender->view->redraw();
    return true;
}
