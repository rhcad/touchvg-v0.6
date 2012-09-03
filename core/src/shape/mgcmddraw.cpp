// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmddraw.h"

UInt32      g_newShapeID = 0;

MgCommandDraw::MgCommandDraw() : m_step(0), m_shape(NULL), m_needClear(false)
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
        if (!m_shape || !m_shape->shape())
            return false;
        m_shape->setParent(sender->view->shapes(), 0);
    }
    g_newShapeID = 0;
    m_step = 0;
    m_needClear = false;
    m_shape->shape()->clear();
    if (sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }
    
    return true;
}

bool MgCommandDraw::_addshape(const MgMotion* sender, MgShape* shape)
{
    MgShapesLock locker(sender->view->shapes(), MgShapesLock::Add);
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
    if (m_needClear) {
        m_needClear = false;
        m_step = 0;
        m_shape->shape()->clear();
    }
    return m_step > 0 && m_shape->draw(*gs);
}

void MgCommandDraw::gatherShapes(const MgMotion* /*sender*/, MgShapes* shapes)
{
    if (m_step > 0 && m_shape) {
        shapes->addShape(*m_shape);
    }
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

void MgCommandDraw::_delayClear()
{
    m_needClear = true;
}

bool MgCommandDraw::mouseHover(const MgMotion* sender)
{
    sender->view->redraw(true);
    return true;
}

Point2d MgCommandDraw::autoAlignPoint(const MgMotion* sender)
{
    Point2d lastPt (dynshape()->shape()->getPoint(m_step - 1)
                    * sender->view->xform()->modelToDisplay());
    Point2d newPt (sender->point);
    float angle = (newPt - lastPt).angle();
    const float limitAngle = _M_D2R * 5.f;
    
    if ((float)fabs(angle - M_PI_2) < limitAngle) {
        angle = M_PI_2;
        newPt.x = lastPt.x;
    }
    else if (angle < limitAngle) {
        angle = 0.f;
        newPt.y = lastPt.y;
    }
    else if (_M_PI - angle < limitAngle) {
        angle = _M_PI;
        newPt.y = lastPt.y;
    }
    
    return newPt * sender->view->xform()->displayToModel();
}
