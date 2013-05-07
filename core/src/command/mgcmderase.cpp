// mgcmderase.cpp: 实现橡皮擦命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmderase.h"
#include <algorithm>
#include <functional>
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgaction.h>

float mgDisplayMmToModel(float mm, const MgMotion* sender);

MgCmdErase::MgCmdErase()
{
}

MgCmdErase::~MgCmdErase()
{
}

bool MgCmdErase::cancel(const MgMotion* sender)
{
    bool recall;
    m_boxsel = false;
    bool ret = undo(recall, sender);
    ret = undo(recall, sender) || ret;
    return undo(recall, sender) || ret;
}

bool MgCmdErase::initialize(const MgMotion* /*sender*/)
{
    m_boxsel = false;
    return true;
}

bool MgCmdErase::undo(bool &enableRecall, const MgMotion* sender)
{
    enableRecall = true;
    if (!m_delIds.empty()) {
        m_delIds.pop_back();
        sender->view->redraw(false);
        return true;
    }
    if (m_boxsel) {
        m_boxsel = false;
        return true;
    }
    return false;
}

bool MgCmdErase::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_boxsel) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 80), 
                          isIntersectMode(sender) ? kGiLineDash : kGiLineSolid, GiColor(0, 0, 255, 24));
        
        bool antiAlias = gs->setAntiAliasMode(false);
        gs->drawRect(&ctxshap, Box2d(sender->startPointM, sender->pointM));
        gs->setAntiAliasMode(antiAlias);
    }
    
    GiContext ctx(-4, GiColor(64, 64, 64, 128));
    
    for (std::vector<int>::const_iterator it = m_delIds.begin(); it != m_delIds.end(); ++it) {
        MgShape* shape = sender->view->shapes()->findShape(*it);
        if (shape)
            shape->draw(1, *gs, &ctx);
    }
    
    return true;
}

void MgCmdErase::gatherShapes(const MgMotion* sender, MgShapes* shapes)
{
    if (m_boxsel) {
        MgShapeT<MgRect> shape;
        
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), 
                          isIntersectMode(sender) ? kGiLineDash : kGiLineSolid, GiColor(0, 0, 255, 32));
        *shape.context() = ctxshap;
        ((MgRect*)shape.shape())->setRect2P(sender->startPointM, sender->pointM);
        shapes->addShape(shape);
    }
}

MgShape* MgCmdErase::hitTest(const MgMotion* sender)
{
    Box2d limits(sender->startPointM, mgDisplayMmToModel(6, sender), 0);
    Point2d nearpt;
    
    return sender->view->shapes()->hitTest(limits, nearpt);
}

bool MgCmdErase::click(const MgMotion* sender)
{
    MgShape* shape = hitTest(sender);
    if (shape && sender->view->shapeWillDeleted(shape)) {
        MgShapesLock locker(sender->view->doc(), MgShapesLock::Remove);
        if (sender->view->removeShape(shape)) {
            shape->release();
            sender->view->regen();
        }
    }
    
    return true;
}

bool MgCmdErase::longPress(const MgMotion* sender)
{
    int actions[] = { 0 };
    return sender->view->showContextActions(0, actions, Box2d(sender->point, 0, 0), NULL);
}

bool MgBaseCommand::longPress(const MgMotion* sender)
{
    int actions[] = { kMgActionCancel, 0 };
    return sender->view->showContextActions(0, isFloatingCommand() ? actions : actions + 1,
                                            Box2d(sender->point, 0, 0), NULL);
}

bool MgCmdErase::touchBegan(const MgMotion* sender)
{
    m_boxsel = true;
    sender->view->redraw(false);
    return true;
}

bool MgCmdErase::isIntersectMode(const MgMotion* sender)
{
    return (sender->startPoint.x < sender->point.x
            && sender->startPoint.y < sender->point.y);
}

bool MgCmdErase::touchMoved(const MgMotion* sender)
{
    Box2d snap(sender->startPointM, sender->pointM);
    void *it = NULL;
    MgShape* shape = m_boxsel ? sender->view->shapes()->getFirstShape(it) : NULL;
    
    m_delIds.clear();
    for (; shape; shape = sender->view->shapes()->getNextShape(it)) {
        if (isIntersectMode(sender) ? shape->shape()->hitTestBox(snap)
            : snap.contains(shape->shape()->getExtent())) {
            m_delIds.push_back(shape->getID());
        }
    }
    sender->view->shapes()->freeIterator(it);
    sender->view->redraw(false);
    
    return true;
}

bool MgCmdErase::touchEnded(const MgMotion* sender)
{
    MgShapes* s = sender->view->shapes();
    
    if (!m_delIds.empty()
        && sender->view->shapeWillDeleted(s->findShape(m_delIds.front()))) {
        MgShapesLock locker(sender->view->doc(), MgShapesLock::Remove);
        int count = 0;
        
        for (std::vector<int>::iterator it = m_delIds.begin(); it != m_delIds.end(); ++it) {
            MgShape* shape = s->findShape(*it);
            if (shape && sender->view->removeShape(shape)) {
                shape->release();
                count++;
            }
        }
        if (count > 0) {
            sender->view->regen();
        }
    }
    
    m_delIds.clear();
    m_boxsel = false;
    sender->view->redraw(false);
    
    return true;
}
