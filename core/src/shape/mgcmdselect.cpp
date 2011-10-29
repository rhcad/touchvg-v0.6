// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdselect.h"
#include <mgbasicsp.h>

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
    if (m_clonesp) {                                // 正在拖改
        m_clonesp->release();
        m_clonesp = NULL;
        m_insertPoint = false;
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

int getLineHalfWidth(const MgShape* shape, GiGraphics* gs)
{
    int width = shape->context()->getLineWidth();
    if (width > 0)
        width = - (int)gs->calcPenWidth(width);
    return mgMax(1, - width / 2);
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    const MgShape* spsel = getSelectedShape(sender);
    const MgShape* shape = m_clonesp ? m_clonesp : spsel;
    if (!shape)
        return false;
    
    // 选中时比原图形宽4像素，控制点修改时仅亮显控制点
    GiContext ctxshape(-4, GiColor(0, 0, 255, m_handleIndex > 0 ? 50 : 128));
    GiContext ctxhd(0, GiColor(64, 128, 64, 172), kLineSolid, GiColor(0, 64, 64, 128));
    int radiuspx = mgMin(8, 2 + mgMax(4, getLineHalfWidth(shape, gs)));
    double radius = gs->xf().displayToModel(radiuspx);
    double r2 = gs->xf().displayToModel(4 + radiuspx);
    
    shape->draw(*gs, &ctxshape);
    
    if (m_handleIndex > 0) {
        for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(i), radius);
        }
        if (m_clonesp || !m_insertPoint) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(m_handleIndex - 1), r2);
        }
        if (m_insertPoint) {
            GiContext insertctx(ctxhd);
            insertctx.setFillColor(GiColor(255, 0, 0, 240));
            gs->drawEllipse(&insertctx, m_ptNear, r2);
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
    Box2d limits(Point2d(sender->point.x, sender->point.y), 50, 0);
    limits *= sender->view->xform()->displayToModel();
    double d = shape->shape()->hitTest(limits.center(), limits.width() / 2, m_ptNear, m_segment);
    
    return d <= limits.width() / 2;
}

Int32 MgCommandSelect::hitTestHandles(MgShape* shape, const MgMotion* sender, const Point2d& pointM)
{
    UInt32 handleIndex = 0;
    double minDist = _DBL_MAX;
    
    for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
        double d = pointM.distanceTo(shape->shape()->getHandlePoint(i));
        if (minDist > d) {
            minDist = d;
            handleIndex = i + 1;
        }
    }
    if (minDist > sender->view->xform()->displayToModel(20)
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        m_insertPoint = true;
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
    
    m_insertPoint = false;
    shape = getSelectedShape(sender);
    canSelAgain = (shape && (m_handleIndex > 0 || shape->getID() == m_id)
                   && canSelect(shape, sender));
    
    if (canSelAgain) {
        handleIndex = hitTestHandles(shape, sender, sender->pointM);
        if (m_handleIndex != handleIndex || m_insertPoint) {
            m_handleIndex = handleIndex;
        }
    }
    else if (shape && m_handleIndex > 0) {
        m_handleIndex = 0;
    }
    else {
        shape = hitTestAll(sender, ptNear, segment);
        UInt32 id = shape ? shape->getID() : 0;
        
        if (m_id != id || handleIndex > 0) {
            m_id = id;
            m_ptNear = ptNear;
            m_segment = segment;
            m_handleIndex = 0;
        }
    }
    sender->view->redraw();
    
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
    
    if (m_clonesp)
        m_clonesp->release();
    m_clonesp = shape ? (MgShape*)shape->clone() : NULL;
    
    m_insertPoint = false;
    m_handleIndex = (m_clonesp && m_handleIndex > 0) ? hitTestHandles(m_clonesp, sender, sender->pointM) : 0;
    
    if (m_insertPoint && m_clonesp->shape()->isKindOf(MgBaseLines::Type())) {
        MgBaseLines* lines = (MgBaseLines*)m_clonesp->shape();
        canSelect(m_clonesp, sender);   // calc m_ptNear
        lines->insertPoint(m_segment, m_ptNear);
        m_clonesp->shape()->update();
        m_handleIndex = hitTestHandles(m_clonesp, sender, m_ptNear);
    }
    
    sender->view->redraw();
    
    return true;
}

bool MgCommandSelect::touchMoved(const MgMotion* sender)
{
    MgShape* shape = getSelectedShape(sender);
    
    if (m_clonesp && shape) {
        m_clonesp->shape()->copy(*(shape->shape()));
        if (m_insertPoint && m_clonesp->shape()->isKindOf(MgBaseLines::Type())) {
            MgBaseLines* lines = (MgBaseLines*)m_clonesp->shape();
            lines->insertPoint(m_segment, m_ptNear);
        }
        if (m_handleIndex > 0) {
            double tol = sender->view->xform()->displayToModel(10);
            m_clonesp->shape()->setHandlePoint(m_handleIndex - 1, sender->pointM, tol);
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
        
        if (m_handleIndex > 0) {
            m_insertPoint = false;
            m_handleIndex = hitTestHandles(shape, sender, sender->pointM);
        }
    }
    
    return true;
}
