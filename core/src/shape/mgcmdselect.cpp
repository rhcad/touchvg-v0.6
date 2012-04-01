// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdselect.h"
#include <mgbasicsp.h>
#include <string.h>
#include <algorithm>
#include <functional>

UInt32 mgGetSelection(MgCommand* cmd, MgView* view, UInt32 count, MgShape** shapes)
{
    if (cmd && strcmp(cmd->getName(), MgCommandSelect::Name()) == 0) {
        MgCommandSelect* sel = (MgCommandSelect*)cmd;
        return sel->getSelection(view, count, shapes);
    }
    return 0;
}

UInt32 MgCommandSelect::getSelection(MgView* view, UInt32 count, MgShape** shapes)
{
    if (count < 1 || !shapes)
        return m_selection.size();
    
    count = mgMin(count, m_selection.size());
    for (UInt32 i = 0; i < count; i++)
        shapes[i] = m_selection[i];
    m_showSel = false;      // 禁止亮显选中图形，以便外部可动态修改图形属性并原样显示
    
    return count;
}

MgCommandSelect::MgCommandSelect()
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
    m_showSel = true;
    m_selection.clear();

    return true;
}

bool MgCommandSelect::undo(bool &, const MgMotion* sender)
{
    if (!m_cloneShapes.empty()) {                   // 正在拖改
        for (sel_iterator it = m_cloneShapes.begin(); it != m_cloneShapes.end(); ++it)
            (*it)->release();
        m_cloneShapes.clear();
        m_insertPoint = false;
        sender->view->redraw();
        return true;
    }
    if (m_id != 0 && m_handleIndex > 0) {           // 控制点修改状态
        m_handleIndex = 0;                          // 回退到图形整体选中状态
        sender->view->redraw();
        return true;
    }
    if (!m_selection.empty()) {                     // 图形整体选中状态
        m_id = 0;
        m_segment = -1;
        m_handleIndex = 0;
        m_selection.clear();
        sender->view->redraw();
        return true;
    }
    return false;
}

int getLineHalfWidth(const MgShape* shape, GiGraphics* gs)
{
    Int16 width = shape->context()->getLineWidth();
    if (width > 0)
        width = - (Int16)gs->calcPenWidth(width);
    return mgMax(1, - width / 2);
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    const std::vector<MgShape*>& shapes = m_cloneShapes.empty() ? m_selection : m_cloneShapes;
    std::vector<MgShape*>::const_iterator it;
    
    if (m_showSel) {        // 选中时比原图形宽4像素，控制点修改时仅亮显控制点
        GiContext ctx(m_handleIndex > 0 ? 0 : -4, GiColor(0, 0, 255, m_handleIndex > 0 ? 50 : 128));
        for (it = shapes.begin(); it != shapes.end(); ++it)
            (*it)->draw(*gs, &ctx);
    }
    else {
        GiContext ctxbk(0, gs->getBkColor());
        for (it = shapes.begin(); it != shapes.end(); ++it)
            (*it)->draw(*gs, &ctxbk);   // 擦掉原图形
        for (it = shapes.begin(); it != shapes.end(); ++it)
            (*it)->draw(*gs);           // 按实际属性动态显示
    }
    
    if (shapes.size() == 1 && m_handleIndex > 0 && m_showSel) {
        GiContext ctxhd(0, GiColor(64, 128, 64, 172), kLineSolid, GiColor(0, 64, 64, 128));
        const MgShape* shape = shapes.front();
        int radiuspx = mgMin(8, 2 + mgMax(4, getLineHalfWidth(shape, gs)));
        double radius = gs->xf().displayToModel(radiuspx);
        double r2 = gs->xf().displayToModel(4 + radiuspx);
        
        for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(i), radius);
        }
        if (!m_cloneShapes.empty() || !m_insertPoint) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(m_handleIndex - 1), r2);
            if (!m_cloneShapes.empty()) {
                gs->drawEllipse(&ctxhd, m_selection.front()->shape()->getHandlePoint(m_handleIndex - 1), r2);
            }
        }
        if (m_insertPoint && !m_cloneShapes.empty()) {
            GiContext insertctx(ctxhd);
            insertctx.setFillColor(GiColor(255, 0, 0, 240));
            gs->drawEllipse(&insertctx, m_ptNear, r2);
        }
        if (!m_cloneShapes.empty()) {
            gs->drawEllipse(&ctxhd, m_selection.front()->shape()->getHandlePoint(m_handleIndex-1), radius);
        }
    }
    
    return true;
}

MgCommandSelect::sel_iterator MgCommandSelect::getSelectedPostion(MgShape* shape)
{
    sel_iterator it = m_selection.end();
    if (shape) {
        it = std::find_if(m_selection.begin(), m_selection.end(),
                          std::bind2nd(std::equal_to<MgShape*>(), shape));
    }
    return it;
}

bool MgCommandSelect::isSelected(MgShape* shape)
{
    return getSelectedPostion(shape) != m_selection.end();
}

MgShape* MgCommandSelect::hitTestAll(const MgMotion* sender, Point2d &ptNear, Int32 &segment)
{
    Box2d limits(Point2d(sender->point.x, sender->point.y), 50, 0);
    limits *= sender->view->xform()->displayToModel();
    
    return sender->view->shapes()->hitTest(limits, ptNear, segment);
}

MgShape* MgCommandSelect::getSelectedShape(const MgMotion* sender)
{
    MgShape* p = sender->view->shapes()->findShape(m_id);
    return (!p && !m_selection.empty()) ? m_selection.front() : p;
}

bool MgCommandSelect::canSelect(MgShape* shape, const MgMotion* sender)
{
    Box2d limits(Point2d(sender->startPoint.x, sender->startPoint.y), 50, 0);
    limits *= sender->view->xform()->displayToModel();
    return shape && shape->shape()->hitTest(limits.center(), limits.width() / 2, 
                                            m_ptNear, m_segment) <= limits.width() / 2;
}

Int32 MgCommandSelect::hitTestHandles(MgShape* shape, const Point2d& pointM)
{
    UInt32 handleIndex = 0;
    double minDist = _DBL_MAX;
    double nearDist = m_ptNear.distanceTo(pointM);
    
    for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
        double d = pointM.distanceTo(shape->shape()->getHandlePoint(i));
        if (minDist > d) {
            minDist = d;
            handleIndex = i + 1;
        }
    }
    
    if (nearDist < minDist / 3
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
    
    if (!m_showSel) {                   // 上次是禁止亮显
        m_showSel = true;               // 恢复亮显选中的图形
        sender->view->regen();          // 可能图形属性已变，重新构建显示
    }
    
    m_insertPoint = false;              // 默认不是插入点，在hitTestHandles中设置
    shape = getSelectedShape(sender);   // 取上次选中的图形
    canSelAgain = (m_selection.size() == 1  // 多选时不进入热点状态
                   && (m_handleIndex > 0 || shape->getID() == m_id)
                   && canSelect(shape, sender));    // 仅检查这个图形能否选中
    
    if (canSelAgain) {                  // 可再次选中同一图形，不论其他图形能否选中
        handleIndex = hitTestHandles(shape, sender->pointM);    // 检查是否切换热点
        if (m_handleIndex != handleIndex || m_insertPoint) {
            m_handleIndex = handleIndex;
        }
    }
    else if (shape && m_handleIndex > 0) {  // 上次是热点状态，在该图形外的较远处点击
        m_handleIndex = 0;                  // 恢复到整体选中状态
    }
    else {                                  // 上次是整体选中状态或没有选中
        shape = hitTestAll(sender, ptNear, segment);
        
        if (shape && isSelected(shape)) {   // 点击已选图形，从选择集中移除
            m_selection.erase(getSelectedPostion(shape));
            m_id = 0;
        }
        else if (shape && !isSelected(shape)) { // 点击新图形，加入选择集
            m_selection.push_back(shape);
            m_id = shape->getID();
        }
        else if (!m_selection.empty()) {    // 在空白处点击清除选择集
            m_selection.clear();
            m_id = 0;
        }
        
        m_ptNear = ptNear;
        m_segment = segment;
        m_handleIndex = 0;
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
    sel_iterator it;
    MgShape* shape = NULL;
    
    for (it = m_cloneShapes.begin(); it != m_cloneShapes.end(); ++it)
        (*it)->release();
    m_cloneShapes.clear();
    for (it = m_selection.begin(); it != m_selection.end(); ++it) {
        shape = (MgShape*)((*it)->clone());
        m_cloneShapes.push_back(shape);
    }
    
    if (!m_showSel) {
        m_showSel = true;
        sender->view->regen();
    }
    
    m_insertPoint = false;                          // setted in hitTestHandles
    if (m_cloneShapes.size() == 1)
        canSelect(shape, sender);   // calc m_ptNear
    m_handleIndex = (m_cloneShapes.size() == 1 && m_handleIndex > 0)
        ? hitTestHandles(shape, sender->pointM) : 0;
    
    if (m_insertPoint && shape->shape()->isKindOf(MgBaseLines::Type())) {
        MgBaseLines* lines = (MgBaseLines*)(shape->shape());
        lines->insertPoint(m_segment, m_ptNear);
        shape->shape()->update();
        m_handleIndex = hitTestHandles(shape, m_ptNear);
    }
    
    sender->view->redraw();
    
    return true;
}

bool MgCommandSelect::touchMoved(const MgMotion* sender)
{
    for (size_t i = 0; i < m_cloneShapes.size(); i++) {
        MgBaseShape* shape = m_cloneShapes[i]->shape();
        
        shape->copy(*m_selection[i]->shape());
        if (m_insertPoint && shape->isKindOf(MgBaseLines::Type())) {
            MgBaseLines* lines = (MgBaseLines*)shape;
            lines->insertPoint(m_segment, m_ptNear);
        }
        if (m_handleIndex > 0) {
            double tol = sender->view->xform()->displayToModel(10);
            shape->setHandlePoint(m_handleIndex - 1, sender->pointM, tol);
        }
        else {
            shape->offset(sender->pointM - sender->startPointM, m_segment);
        }
        shape->update();
        sender->view->redraw();
    }
    
    if (m_cloneShapes.empty()) {            // 没有选中图形时就滑动多选
        MgShape* shape = hitTestAll(sender, m_ptNear, m_segment);
        if (shape && !isSelected(shape)) {
            m_selection.push_back(shape);
            sender->view->redraw();
        }
    }
    
    return true;
}

bool MgCommandSelect::touchEnded(const MgMotion* sender)
{
    MgShape* shape = NULL;
    
    for (size_t i = 0; i < m_cloneShapes.size(); i++) {
        shape = m_selection[i];
        
        shape->shape()->copy(*m_cloneShapes[i]->shape());
        shape->shape()->update();
        sender->view->regen();
        
        m_cloneShapes[i]->release();
        m_cloneShapes[i] = NULL;
    }
    m_cloneShapes.clear();
    
    if (m_handleIndex > 0) {
        m_insertPoint = false;
        m_handleIndex = hitTestHandles(shape, sender->pointM);
    }
    
    return true;
}
