// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdselect.h"

MgCommandSelect::MgCommandSelect() : m_clonesp(NULL)
{
}

MgCommandSelect::~MgCommandSelect()
{
}

bool MgCommandSelect::cancel(const MgMotion* sender)
{
    bool recall;
    bool ret = undo(recall, sender);
    ret = undo(recall, sender) || ret;
    return undo(recall, sender) || ret;
}

bool MgCommandSelect::initialize(const MgMotion* /*sender*/)
{
    m_id = 0;
    m_segment = -1;
    m_handleIndex = 0;

    return true;
}

bool MgCommandSelect::undo(bool &, const MgMotion* sender)
{
    if (m_clonesp) {                             // 正在拖改
        m_clonesp->release();
        m_clonesp = NULL;
        sender->view->redraw();
        return true;
    }
    if (m_id != 0 && m_handleIndex != 0) {          // 控制点修改状态
        m_handleIndex = 0;                          // 回退到图形整体选中状态
        sender->view->redraw();
        return true;
    }
    if (m_id != 0) {                                // 图形整体选中状态
        m_id = 0;
        m_segment = -1;
        m_handleIndex = 0;
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    const MgShape* spsel = getSelectedShape(sender);
    const MgShape* shape = m_clonesp ? m_clonesp : spsel;
    if (!shape)
        return false;
    
    // 选中时比原图形宽4像素，控制点修改时仅亮显控制点
    GiContext ctxshape(-4, GiColor(0, 0, 255, m_handleIndex > 0 ? 50 : 128));
    GiContext ctxhd(0, GiColor(64, 64, 64, 128), kLineSolid, GiColor(0, 64, 64, 64));
    double radius = gs->xf().displayToModel(4);
    
    shape->draw(*gs, &ctxshape);
    
    if (m_handleIndex > 0) {
        for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(i), 
                            m_handleIndex == i+1 ? radius*2 : radius);
        }
        if (m_clonesp) {
            gs->drawEllipse(&ctxhd, spsel->shape()->getHandlePoint(m_handleIndex-1), radius);
        }
    }
    
    return true;
}

MgShape* MgCommandSelect::hitTestAll(const MgMotion* sender, Point2d &ptNear, Int32 &segment)
{
    Box2d limits(Point2d(sender->point.x, sender->point.y), 50, 0);
    limits *= sender->view->xform()->displayToModel();
    
    return sender->view->shapes()->hitTest(limits, ptNear, segment);
}

MgShape* MgCommandSelect::getSelectedShape(const MgMotion* sender)
{
    return sender->view->shapes()->findShape(m_id);
}

bool MgCommandSelect::canSelect(MgShape* shape, const MgMotion* sender)
{
    Box2d limits(Point2d(sender->point.x, sender->point.y), 20, 0);
    limits *= sender->view->xform()->displayToModel();
    
    Point2d ptNear;
    Int32 segment;
    double d = shape->shape()->hitTest(limits.center(), limits.width() / 2, ptNear, segment);
    
    return d <= limits.width() / 2;
}

Int32 MgCommandSelect::hitTestHandles(MgShape* shape, const MgMotion* sender)
{
    UInt32 handleIndex = 0;
    double minDist = _DBL_MAX;
    
    for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
        double d = sender->pointM.distanceTo(shape->shape()->getHandlePoint(i));
        if (minDist > d) {
            minDist = d;
            handleIndex = i + 1;
        }
    }
    
    return handleIndex;
}

bool MgCommandSelect::click(const MgMotion* sender)
{
    Point2d ptNear;
    Int32   segment = -1;
    UInt32  handleIndex = 0;
    MgShape* shape;
    bool    canSelAgain;
    
    shape = getSelectedShape(sender);
    canSelAgain = (shape && (m_handleIndex > 0 || shape->getID() == m_id)
                   && canSelect(shape, sender));
    
    if (canSelAgain) {
        handleIndex = hitTestHandles(shape, sender);
        if (m_handleIndex != handleIndex) {
            m_handleIndex = handleIndex;
            sender->view->redraw();
        }
    }
    else if (shape && m_handleIndex > 0) {
        m_handleIndex = 0;
        sender->view->redraw();
    }
    else {
        shape = hitTestAll(sender, ptNear, segment);
        UInt32 id = shape ? shape->getID() : 0;
        
        if (m_id != id || handleIndex > 0) {
            m_id = id;
            m_ptNear = ptNear;
            m_segment = segment;
            m_handleIndex = 0;
            sender->view->redraw();
        }
    }
    
    return true;
}

bool MgCommandSelect::doubleClick(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandSelect::longPress(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandSelect::touchBegan(const MgMotion* sender)
{
    MgShape* shape = getSelectedShape(sender);
    
    if (shape && m_handleIndex > 0) {
        m_handleIndex = hitTestHandles(shape, sender);
    }
    if (m_clonesp)
        m_clonesp->release();
    m_clonesp = shape ? (MgShape*)shape->clone() : NULL;
    
    sender->view->redraw();
    
    return true;
}

bool MgCommandSelect::touchMoved(const MgMotion* sender)
{
    MgShape* shape = getSelectedShape(sender);
    
    if (m_clonesp && shape) {
        m_clonesp->shape()->copy(*(shape->shape()));
        if (m_handleIndex > 0) {
            m_clonesp->shape()->setHandlePoint(m_handleIndex - 1, sender->pointM);
        }
        else {
            m_clonesp->shape()->offset(sender->pointM - sender->startPointM, m_segment);
        }
        m_clonesp->shape()->update();
        sender->view->redraw();
    }
    
    return true;
}

bool MgCommandSelect::touchEnded(const MgMotion* sender)
{
    MgShape* shape = getSelectedShape(sender);
    
    if (m_clonesp && shape) {
        shape->shape()->copy(*(m_clonesp->shape()));
        shape->shape()->update();
        sender->view->regen();
        m_clonesp->release();
        m_clonesp = NULL;
    }
    
    return true;
}
