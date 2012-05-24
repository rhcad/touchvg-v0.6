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
    m_undoFired = false;
    
    return true;
}

bool MgCommandDraw::touchMoved(const MgMotion* sender)
{
    bool ret = false;
    
    if (sender->touchCount > 1) {                       // 滑动时有两个手指
        bool recall = false;
        float dist = mgHypot(sender->point.x - sender->lastPoint.x, 
                             sender->point.y - sender->lastPoint.y);
        if (!m_undoFired && dist > 10) {                // 双指滑动超过10像素可再触发Undo操作
            ret = true;
            if (undo(recall, sender) && !recall)        // 触发一次Undo操作
                m_undoFired = true;                     // 另一个手指不松开也不再触发Undo操作
        }
    }
    if (!ret) {
        ret = touchMoved_(sender);
        m_undoFired = false;                            // 允许再触发Undo操作
    }
    
    return ret;
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
