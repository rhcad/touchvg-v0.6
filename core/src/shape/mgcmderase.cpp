// mgcmderase.cpp: 实现橡皮擦命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmderase.h"
#include <algorithm>
#include <string>

MgCommandErase::MgCommandErase()
{
}

MgCommandErase::~MgCommandErase()
{
}

bool MgCommandErase::cancel(const MgMotion* sender)
{
    bool recall;
    return undo(recall, sender);
}

bool MgCommandErase::initialize(const MgMotion* /*sender*/)
{
    m_down = false;
    return true;
}

bool MgCommandErase::undo(bool &enableRecall, const MgMotion* sender)
{
    enableRecall = true;
    if (!m_deleted.empty()) {
        m_deleted.pop_back();
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandErase::draw(const MgMotion* sender, GiGraphics* gs)
{
    GiContext ctx(0, GiColor(64, 64, 64, 192), kLineDot);
    
    if (m_down) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 64), kLineSolid, GiColor(0, 0, 255, 64));
        Box2d limits(Point2d(sender->point.x, sender->point.y), 20, 0);
        gs->drawRect(&ctxshap, limits * sender->view->xform()->displayToModel());
    }
    for (std::vector<MgShape*>::const_iterator it = m_deleted.begin();
         it != m_deleted.end(); ++it) {
        (*it)->draw(*gs, &ctx);
    }
    
    return true;
}

MgShape* MgCommandErase::hitTest(const MgMotion* sender)
{
    Box2d limits(Point2d(sender->point.x, sender->point.y), 20, 0);
    limits *= sender->view->xform()->displayToModel();
    
    Point2d ptNear;
    Int32 segment;
    
    return sender->view->shapes()->hitTest(limits, ptNear, segment);
}

bool MgCommandErase::click(const MgMotion* sender)
{
    MgShape* shape = hitTest(sender);
    if (shape) {
        shape = sender->view->shapes()->removeShape(shape->getID());
        shape->release();
        sender->view->regen();
    }
    
    return true;
}

bool MgCommandErase::doubleClick(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandErase::longPress(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandErase::touchBegan(const MgMotion* sender)
{
    m_down = true;
    sender->view->redraw();
    return true;
}

bool MgCommandErase::touchMoved(const MgMotion* sender)
{
    MgShape* shape = hitTest(sender);
    
    if (shape && std::find_if(m_deleted.begin(), m_deleted.end(),
                              std::bind2nd(std::equal_to<MgShape*>(), shape)) == m_deleted.end()) {
        m_deleted.push_back(shape);
        sender->view->redraw();
    }
    sender->view->redraw();
    
    return true;
}

bool MgCommandErase::touchEnded(const MgMotion* sender)
{
    for (std::vector<MgShape*>::iterator it = m_deleted.begin();
         it != m_deleted.end(); ++it) {
        MgShape* shape = *it;
        shape = sender->view->shapes()->removeShape(shape->getID());
        shape->release();
    }
    if (!m_deleted.empty()) {
        sender->view->regen();
        m_deleted.clear();
    }
    m_down = false;
    sender->view->redraw();
    
    return true;
}
