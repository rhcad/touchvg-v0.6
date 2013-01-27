// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmddraw.h"
#include <mgsnap.h>
#include <mgselect.h>
#include <mgaction.h>

int      g_newShapeID = 0;

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
    if (!m_shape) {
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

MgShape* MgCommandDraw::_addshape(const MgMotion* sender, MgShape* shape)
{
    MgShapesLock locker(sender->view->shapes(), MgShapesLock::Add);
    shape = shape ? shape : m_shape;
    MgShape* newsp = NULL;
    
    if (locker.locked() && sender->view->shapeWillAdded(shape)) {
        newsp = sender->view->shapes()->addShape(*shape);
        sender->view->shapeAdded(newsp);
        g_newShapeID = newsp->getID();
    }
    if (m_shape && sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }
    
    return newsp;
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

bool MgCommandDraw::_draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_needClear) {
        m_needClear = false;
        m_step = 0;
        m_shape->shape()->clear();
    }
    bool ret = m_step > 0 && m_shape->draw(0, *gs);
    //if (m_step > 0 && sender->dragging) {
    //    sender->view->drawHandle(gs, sender->pointM, true);
    //}
    return mgGetCommandManager()->getSnap()->drawSnap(sender, gs) || ret;
}

void MgCommandDraw::gatherShapes(const MgMotion* /*sender*/, MgShapes* shapes)
{
    if (m_step > 0 && m_shape) {
        shapes->addShape(*m_shape);
    }
}

bool MgCommandDraw::click(const MgMotion* sender)
{
    return (m_step == 0 ? _click(sender)
            : touchBegan(sender) && touchEnded(sender));
}

bool MgCommandDraw::_click(const MgMotion* sender)
{
    Box2d limits(sender->pointM, mgDisplayMmToModel(10, sender), 0);
    Point2d nearpt;
    MgShape* shape = sender->view->shapes()->hitTest(limits, nearpt);
    
    if (shape) {
        g_newShapeID = shape->getID();
        mgGetCommandManager()->setCommand(sender, "select");
    }
    
    return shape || longPress(sender);
}

bool MgCommandDraw::doubleClick(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandDraw::longPress(const MgMotion* sender)
{
    return mgGetCommandManager()->getActionDispatcher()->showInDrawing(sender, m_shape);
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

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, bool firstStep)
{
    return mgGetCommandManager()->getSnap()->snapPoint(sender, firstStep ? NULL : m_shape, m_step);
}

void MgCommandDraw::setStepPoint(int step, const Point2d& pt)
{
    if (step > 0) {
        dynshape()->shape()->setHandlePoint(step, pt, 0);
    }
}

bool MgCommandDraw::_touchBegan2(const MgMotion* sender)
{
    if (0 == m_step) {
        m_step = 1;
        Point2d pnt(snapPoint(sender, true));
        for (int i = dynshape()->shape()->getPointCount() - 1; i >= 0; i--) {
            dynshape()->shape()->setPoint(i, pnt);
        }
        setStepPoint(0, pnt);
    }
    else {
        setStepPoint(m_step, snapPoint(sender));
    }
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCommandDraw::_touchMoved2(const MgMotion* sender)
{
    setStepPoint(m_step, snapPoint(sender));
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCommandDraw::_touchEnded2(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender));
    float distmin = mgDisplayMmToModel(2.f, sender);
    
    setStepPoint(m_step, pnt);
    dynshape()->shape()->update();
    
    if (pnt.distanceTo(dynshape()->shape()->getPoint(m_step - 1)) > distmin) {
        m_step++;
        if (m_step >= getMaxStep()) {
            _addshape(sender);
            _delayClear();
            m_step = 0;
        }
    }

    return _touchEnded(sender);
}
