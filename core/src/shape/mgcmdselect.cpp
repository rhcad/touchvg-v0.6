// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdselect.h"
#include <mgbasicsp.h>
#include <string.h>
#include <algorithm>
#include <functional>

extern UInt32 g_newShapeID;

UInt32 MgCommandSelect::getSelection(MgView* view, UInt32 count, MgShape** shapes, bool forChange)
{
    if (forChange && m_cloneShapes.empty()) {
        cloneShapes(view);
    }
    
    UInt32 ret = 0;
    UInt32 maxCount = m_cloneShapes.empty() ? m_selIds.size() : m_cloneShapes.size();
    
    if (count < 1 || !shapes)
        return maxCount;
    
    count = mgMin(count, maxCount);
    for (UInt32 i = 0; i < count; i++) {
        if (m_cloneShapes.empty()) {
            MgShape* shape = view->shapes()->findShape(m_selIds[i]);
            if (shape)
                shapes[ret++] = shape;
        }
        else {
            shapes[ret++] = m_cloneShapes[i];
        }
    }
    m_showSel = false;      // 禁止亮显选中图形，以便外部可动态修改图形属性并原样显示
    
    return ret;
}

bool MgCommandSelect::dynamicChangeEnded(MgView* view, bool apply)
{
    return applyCloneShapes(view, apply);
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

bool MgCommandSelect::initialize(const MgMotion* sender)
{
    m_boxsel = false;
    m_id = 0;
    m_segment = -1;
    m_handleIndex = 0;
    m_showSel = true;
    m_selIds.clear();
    
    MgShape* shape = getShape(g_newShapeID, sender);
    if (shape) {
        m_selIds.push_back(shape->getID());         // 选中最新绘制的图形
        m_id = shape->getID();
        sender->view->redraw(false);
    }
    g_newShapeID = 0;

    return true;
}

bool MgCommandSelect::undo(bool &, const MgMotion* sender)
{
    m_boxsel = false;
    if (!m_cloneShapes.empty()) {                   // 正在拖改
        for (std::vector<MgShape*>::iterator it = m_cloneShapes.begin();
            it != m_cloneShapes.end(); ++it) {
            (*it)->release();
        }
        m_cloneShapes.clear();
        m_insertPoint = false;
        sender->view->redraw(false);
        return true;
    }
    if (m_id != 0 && m_handleIndex > 0) {           // 控制点修改状态
        m_handleIndex = 0;                          // 回退到图形整体选中状态
        sender->view->redraw(false);
        return true;
    }
    if (!m_selIds.empty()) {                        // 图形整体选中状态
        m_id = 0;
        m_segment = -1;
        m_handleIndex = 0;
        m_selIds.clear();
        sender->view->redraw(false);
        return true;
    }
    return false;
}

float mgLineHalfWidthModel(const MgShape* shape, const MgMotion* sender)
{
    float w = shape->context()->getLineWidth();
    GiGraphics* gs = sender->view->graph();
    
    w = w > 0 ? - gs->calcPenWidth(w) : w;
    w = mgMax(1.f, -0.5f * w);
    w = gs->xf().displayToModel(w);
    
    return w;
}

float mgDisplayMmToModel(float mm, const MgMotion* sender)
{
    return sender->view->xform()->displayToModel(mm, true);
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    std::vector<MgShape*> selection;
    const std::vector<MgShape*>& shapes = m_cloneShapes.empty() ? selection : m_cloneShapes;
    std::vector<MgShape*>::const_iterator it;

    for (sel_iterator its = m_selIds.begin(); its != m_selIds.end(); ++its) {
        MgShape* shape = getShape(*its, sender);
        if (shape)
            selection.push_back(shape);
    }
    if (selection.empty() && !m_selIds.empty())
        m_selIds.clear();
    
    if (m_boxsel) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), 
                          isIntersectMode(sender) ? kLineDash : kLineSolid, GiColor(0, 0, 255, 32));
        bool antiAlias = gs->isAntiAliasMode();
        
        gs->setAntiAliasMode(false);
        gs->drawRect(&ctxshap, Box2d(sender->startPointM, sender->pointM));
        gs->setAntiAliasMode(antiAlias);
    }
    
    if (m_showSel) {        // 选中时比原图形宽4像素，控制点修改时仅亮显控制点
        GiContext ctxshape(m_handleIndex > 0 ? 0 : -4, GiColor(0, 0, 255, m_handleIndex > 0 ? 64 : 128));
        for (it = shapes.begin(); it != shapes.end(); ++it) {
            (*it)->draw(*gs, &ctxshape);
        }
    }
    else {
        GiContext ctxbk(-1, gs->getBkColor());
        for (it = selection.begin(); it != selection.end(); ++it)
            (*it)->draw(*gs, &ctxbk);   // 擦掉原图形
        for (it = shapes.begin(); it != shapes.end(); ++it)
            (*it)->draw(*gs);           // 按实际属性动态显示
    }
    
    if (shapes.size() == 1 && m_handleIndex > 0 && m_showSel) {
        GiContext ctxhd(0, GiColor(64, 128, 64, 172), kLineSolid, GiColor(0, 64, 64, 128));
        const MgShape* shape = shapes.front();
        float radius = mgDisplayMmToModel(1, sender);
        float r2 = mgDisplayMmToModel(2, sender);
        
        for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(i), radius);
        }
        if (!m_cloneShapes.empty() || !m_insertPoint) {
            gs->drawEllipse(&ctxhd, shape->shape()->getHandlePoint(m_handleIndex - 1), r2);
            if (!m_cloneShapes.empty() && !selection.empty()) {
                gs->drawEllipse(&ctxhd, selection.front()->shape()->getHandlePoint(m_handleIndex - 1), r2);
            }
        }
        if (m_insertPoint && !m_cloneShapes.empty()) {
            GiContext insertctx(ctxhd);
            insertctx.setFillColor(GiColor(255, 0, 0, 240));
            gs->drawEllipse(&insertctx, m_ptNear, r2);
        }
        if (!m_cloneShapes.empty() && !selection.empty()) {
            gs->drawEllipse(&ctxhd, selection.front()->shape()->getHandlePoint(m_handleIndex-1), radius);
        }
    }
    
    return true;
}

MgCommandSelect::sel_iterator MgCommandSelect::getSelectedPostion(MgShape* shape)
{
    sel_iterator it = m_selIds.end();
    if (shape) {
        it = std::find_if(m_selIds.begin(), m_selIds.end(),
                          std::bind2nd(std::equal_to<UInt32>(), shape->getID()));
    }
    return it;
}

MgShape* MgCommandSelect::getShape(UInt32 id, const MgMotion* sender) const
{
    return sender->view->shapes()->findShape(id);
}

bool MgCommandSelect::isSelected(MgShape* shape)
{
    return getSelectedPostion(shape) != m_selIds.end();
}

MgShape* MgCommandSelect::hitTestAll(const MgMotion* sender, Point2d &nearpt, Int32 &segment)
{
    Box2d limits(sender->pointM, mgDisplayMmToModel(6, sender), 0);
    return sender->view->shapes()->hitTest(limits, nearpt, segment);
}

MgShape* MgCommandSelect::getSelectedShape(const MgMotion* sender)
{
    MgShape* p = getShape(m_id, sender);
    return (!p && !m_selIds.empty()) ? getShape(m_selIds.front(), sender) : p;
}

bool MgCommandSelect::canSelect(MgShape* shape, const MgMotion* sender)
{
    Box2d limits(sender->startPointM, mgDisplayMmToModel(10, sender), 0);
    return shape && shape->shape()->hitTest(limits.center(), limits.width() / 2, 
                                            m_ptNear, m_segment) <= limits.width() / 2;
}

Int32 MgCommandSelect::hitTestHandles(MgShape* shape, const Point2d& pointM, const MgMotion* sender)
{
    if (!shape)
        return -1;

    UInt32 handleIndex = 0;
    float minDist = _FLT_MAX;
    float nearDist = m_ptNear.distanceTo(pointM);
    
    for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
        float d = pointM.distanceTo(shape->shape()->getHandlePoint(i));
        if (minDist > d) {
            minDist = d;
            handleIndex = i + 1;
        }
    }
    
    if (nearDist < minDist / 3
        && minDist > mgDisplayMmToModel(5, sender)
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        m_insertPoint = true;
    }
    
    return handleIndex;
}

bool MgCommandSelect::click(const MgMotion* sender)
{
    Point2d nearpt;
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
    canSelAgain = (m_selIds.size() == 1  // 多选时不进入热点状态
                   && (m_handleIndex > 0 || shape->getID() == m_id)
                   && canSelect(shape, sender));    // 仅检查这个图形能否选中
    
    if (canSelAgain) {                  // 可再次选中同一图形，不论其他图形能否选中
        handleIndex = hitTestHandles(shape, sender->pointM, sender);    // 检查是否切换热点
        if (m_handleIndex != handleIndex || m_insertPoint) {
            m_handleIndex = handleIndex;
        }
    }
    else if (shape && m_handleIndex > 0) {  // 上次是热点状态，在该图形外的较远处点击
        m_handleIndex = 0;                  // 恢复到整体选中状态
    }
    else {                                  // 上次是整体选中状态或没有选中
        shape = hitTestAll(sender, nearpt, segment);
        
        if (shape && isSelected(shape)) {   // 点击已选图形，从选择集中移除
            m_selIds.erase(getSelectedPostion(shape));
            m_id = 0;
        }
        else if (shape && !isSelected(shape)) { // 点击新图形，加入选择集
            m_selIds.push_back(shape->getID());
            m_id = shape->getID();
        }
        else if (!m_selIds.empty()) {    // 在空白处点击清除选择集
            m_selIds.clear();
            m_id = 0;
        }
        
        m_ptNear = nearpt;
        m_segment = segment;
        m_handleIndex = 0;
    }
    sender->view->redraw(false);
    
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
    cloneShapes(sender->view);
    MgShape* shape = m_cloneShapes.empty() ? NULL : m_cloneShapes.front();
    
    if (!m_showSel) {
        m_showSel = true;
        sender->view->redraw(false);
    }
    
    m_insertPoint = false;                          // setted in hitTestHandles
    if (m_cloneShapes.size() == 1)
        canSelect(shape, sender);   // calc m_ptNear
    m_handleIndex = (m_cloneShapes.size() == 1 && m_handleIndex > 0)
        ? hitTestHandles(shape, sender->pointM, sender) : 0;
    
    if (m_insertPoint && shape->shape()->isKindOf(MgBaseLines::Type())) {
        MgBaseLines* lines = (MgBaseLines*)(shape->shape());
        lines->insertPoint(m_segment, m_ptNear);
        shape->shape()->update();
        m_handleIndex = hitTestHandles(shape, m_ptNear, sender);
    }
    
    if (m_cloneShapes.empty()) {
        m_boxsel = true;
    }
    
    sender->view->redraw(m_cloneShapes.size() < 2);
    
    return true;
}

bool MgCommandSelect::isIntersectMode(const MgMotion* sender)
{
    return (sender->startPoint.x < sender->point.x
            && sender->startPoint.y < sender->point.y);
}

bool MgCommandSelect::touchMoved(const MgMotion* sender)
{
    Point2d pointM(sender->pointM);
    
    if (m_insertPoint && pointM.distanceTo(m_ptNear) < mgDisplayMmToModel(5, sender)) {
        pointM = m_ptNear;  // 拖动刚新加的点到起始点时取消新增
    }
    for (size_t i = 0; i < m_cloneShapes.size(); i++) {
        MgBaseShape* shape = m_cloneShapes[i]->shape();
        MgShape* basesp = getShape(m_selIds[i], sender);
        
        if (basesp)
            shape->copy(*basesp->shape());
        if (m_insertPoint && shape->isKindOf(MgBaseLines::Type())) {
            MgBaseLines* lines = (MgBaseLines*)shape;
            lines->insertPoint(m_segment, m_ptNear);
        }
        if (m_handleIndex > 0) {
            float tol = mgDisplayMmToModel(5, sender);
            shape->setHandlePoint(m_handleIndex - 1, pointM, tol);
        }
        else {
            shape->offset(pointM - sender->startPointM, m_segment);
        }
        shape->update();
        sender->view->redraw(m_cloneShapes.size() < 2);
    }
    
    if (m_cloneShapes.empty() && m_boxsel) {    // 没有选中图形时就滑动多选
        Box2d snap(sender->startPointM, sender->pointM);
        void *it;
        MgShape* shape = sender->view->shapes()->getFirstShape(it);
        
        m_selIds.clear();
        for (; shape; shape = sender->view->shapes()->getNextShape(it)) {
            if (isIntersectMode(sender) ? shape->shape()->hitTestBox(snap)
                : snap.contains(shape->shape()->getExtent())) {
                m_selIds.push_back(shape->getID());
            }
        }
        sender->view->redraw(true);
    }
    
    return true;
}

bool MgCommandSelect::touchEnded(const MgMotion* sender)
{
    if (m_insertPoint && sender->pointM.distanceTo(m_ptNear)
        < mgDisplayMmToModel(5, sender)) {  // 拖动刚新加的点到起始点时取消新增
        m_cloneShapes[0]->release();
        m_cloneShapes.clear();
    }
    
    applyCloneShapes(sender->view, true);
    
    m_insertPoint = false;
    if (m_handleIndex > 0) {
        m_handleIndex = hitTestHandles(getShape(m_selIds[0], sender), sender->pointM, sender);
        sender->view->redraw(true);
    }
    
    return true;
}

void MgCommandSelect::cloneShapes(MgView* view)
{
    for (std::vector<MgShape*>::iterator it = m_cloneShapes.begin();
         it != m_cloneShapes.end(); ++it) {
        (*it)->release();
    }
    m_cloneShapes.clear();
    
    for (sel_iterator its = m_selIds.begin(); its != m_selIds.end(); ++its) {
        MgShape* shape = view->shapes()->findShape(*its);
        if (shape) {
            shape = (MgShape*)(shape->clone());
            m_cloneShapes.push_back(shape);
        }
    }
}

bool MgCommandSelect::applyCloneShapes(MgView* view, bool apply)
{
    bool changed = false;
    bool cloned = !m_cloneShapes.empty();
    
    if (!m_cloneShapes.empty()) {
        MgShapesLock locker(view->shapes());
        
        for (size_t i = 0; i < m_cloneShapes.size(); i++) {
            MgShape* shape = i < m_selIds.size() ? view->shapes()->findShape(m_selIds[i]) : NULL;
            
            if (shape && apply) {
                shape->shape()->copy(*m_cloneShapes[i]->shape());
                shape->shape()->update();
                changed = true;
            }
            
            m_cloneShapes[i]->release();
            m_cloneShapes[i] = NULL;
        }
        m_cloneShapes.clear();
    }
    if (changed) {
        view->regen();
    }
    else {
        view->redraw(true);
    }
    m_boxsel = false;
    
    return changed || cloned;
}
