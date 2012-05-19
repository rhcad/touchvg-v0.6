// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmddraw.h"
#include <assert.h>

UInt32      g_newShapeID = 0;

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
        sender->view->redraw(false);
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
    g_newShapeID = 0;
    m_step = 0;
    m_shape->shape()->clear();
    if (sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }
    
    return true;
}

bool MgCommandDraw::_addshape(const MgMotion* sender, MgShape* shape)
{
    MgShapesLock locker(sender->view->shapes());
    shape = shape ? shape : m_shape;
    bool ret = sender->view->shapeWillAdded(shape);
    
    if (ret) {
        MgShape* newsp = sender->view->shapes()->addShape(*shape);
        sender->view->shapeAdded(newsp);
        g_newShapeID = newsp->getID();
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
        sender->view->redraw(false);
        return true;
    }
    return false;
}

bool MgCommandDraw::draw(const MgMotion* /*sender*/, GiGraphics* gs)
{
    return m_step > 0 && m_shape->draw(*gs);
}

bool MgCommandDraw::click(const MgMotion* /*sender*/)
{
    return false;
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
    sender->view->redraw(true);
    return true;
}

bool MgCommandDraw::_touchMoved(const MgMotion* sender)
{
    sender->view->redraw(true);
    return true;
}

bool MgCommandDraw::_touchEnded(const MgMotion* sender)
{
    sender->view->redraw(true);
    return true;
}
