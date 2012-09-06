// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdselect.h"
#include <mgbasicsp.h>
#include <string.h>
#include <algorithm>
#include <functional>
#include <mgshapet.h>
#include <mgnear.h>
#include <mgbase.h>

extern UInt32 g_newShapeID;

UInt32 MgCommandSelect::getSelection(MgView* view, UInt32 count,
                                     MgShape** shapes, bool forChange)
{
    if (forChange && m_clones.empty())
        cloneShapes(view);
    
    UInt32 ret = 0;
    UInt32 maxCount = m_clones.empty() ? m_selIds.size() : m_clones.size();
    
    if (count < 1 || !shapes)
        return maxCount;
    
    count = mgMin(count, maxCount);
    for (UInt32 i = 0; i < count; i++) {
        if (m_clones.empty()) {
            MgShape* shape = view->shapes()->findShape(m_selIds[i]);
            if (shape)
                shapes[ret++] = shape;
        }
        else {
            shapes[ret++] = m_clones[i];
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
    m_handleMode = false;
    m_showSel = true;
    m_selIds.clear();
    
    MgShape* shape = getShape(g_newShapeID, sender);
    if (shape) {
        m_selIds.push_back(shape->getID());         // 选中最新绘制的图形
        m_id = shape->getID();
        m_handleMode = true;
        sender->view->redraw(false);
        sender->view->selChanged();
    }
    g_newShapeID = 0;
    
    return true;
}

bool MgCommandSelect::undo(bool &, const MgMotion* sender)
{
    m_boxsel = false;
    m_boxHandle = 99;
    
    if (!m_clones.empty()) {                        // 正在拖改
        for (std::vector<MgShape*>::iterator it = m_clones.begin();
             it != m_clones.end(); ++it) {
            (*it)->release();
        }
        m_clones.clear();
        m_insertPt = false;
        sender->view->redraw(false);
        return true;
    }
    if (m_id != 0 && m_handleMode) {                // 控制点修改状态
        m_handleIndex = 0;                          // 回退到图形整体选中状态
        m_handleMode = false;
        sender->view->redraw(false);
        return true;
    }
    if (!m_selIds.empty()) {                        // 图形整体选中状态
        m_id = 0;
        m_segment = -1;
        m_handleIndex = 0;
        m_handleMode = false;
        m_selIds.clear();
        sender->view->redraw(false);
        sender->view->selChanged();
        return true;
    }
    return false;
}

float mgLineHalfWidthModel(const MgShape* shape, GiGraphics* gs)
{
    float w = shape->contextc()->getLineWidth();
    
    w = w > 0 ? - gs->calcPenWidth(w) : w;
    w = mgMax(1.f, -0.5f * w);
    w = gs->xf().displayToModel(w);
    
    return w;
}

float mgLineHalfWidthModel(const MgShape* shape, const MgMotion* sender)
{
    return mgLineHalfWidthModel(shape, sender->view->graph());
}

float mgDisplayMmToModel(float mm, GiGraphics* gs)
{
    return gs->xf().displayToModel(mm, true);
}

float mgDisplayMmToModel(float mm, const MgMotion* sender)
{
    return sender->view->xform()->displayToModel(
        sender->view->useFinger() ? mm : 2 * mm, sender->view->useFinger());
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    std::vector<MgShape*> selection;
    const std::vector<MgShape*>& shapes = m_clones.empty() ? selection : m_clones;
    std::vector<MgShape*>::const_iterator it;
    Point2d pnt;
    GiContext ctxhd(-2, GiColor(128, 128, 64, 200), 
                    kGiLineSolid, GiColor(172, 172, 172, 128));
    float radius = mgDisplayMmToModel(1.2f, gs);
    float r2x = mgDisplayMmToModel(2, gs);
    bool rorate = (!m_handleMode && m_boxHandle >= 8 && m_boxHandle < 12);
    
    // 从 m_selIds 得到临时图形数组 selection
    for (sel_iterator its = m_selIds.begin(); its != m_selIds.end(); ++its) {
        MgShape* shape = getShape(*its, sender);
        if (shape)
            selection.push_back(shape);
    }
    if (selection.empty() && !m_selIds.empty()) {   // 意外情况导致m_selIds部分ID无效
        m_selIds.clear();
        sender->view->selChanged();
    }
    
    if (!m_showSel || (!m_clones.empty() && !isCloneDrag(sender))) {
        GiContext ctxbk(-1, gs->getBkColor());
        for (it = selection.begin(); it != selection.end(); ++it)
            (*it)->draw(*gs, &ctxbk);               // 用背景色擦掉原图形
        
        if (m_showSel && !rorate) {                 // 拖动提示的参考线
            GiContext ctxshap(-1.05f, GiColor(0, 0, 255, 128), kGiLineDash);
            gs->drawLine(&ctxshap, sender->startPointM, m_ptSnap);
        }
    }
    
    // 外部动态改变图形属性时，或拖动时：原样显示
    if (!m_showSel || !m_clones.empty()) {
        for (it = shapes.begin(); it != shapes.end(); ++it) {
            (*it)->draw(*gs);
        }
    }
    else if (m_clones.empty()) {   // 选中时比原图形宽4像素
        GiContext ctxshape(sender->view->useFinger() ? -4.f : 0, 
                           GiColor(0, 0, 255, 128));
        for (it = shapes.begin(); it != shapes.end(); ++it) {
            GiContext ctx(ctxshape);
            (*it)->draw(*gs, &ctx);
        }
    }
    
    if (m_boxsel) {                 // 显示框选半透明蓝色边框
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), 
                          isIntersectMode(sender) ? kGiLineDash : kGiLineSolid,
                          GiColor(0, 0, 255, 32));
        bool antiAlias = gs->setAntiAliasMode(false);
        gs->drawRect(&ctxshap, Box2d(sender->startPointM, sender->pointM));
        gs->setAntiAliasMode(antiAlias);
    }
    else if (!selection.empty() && !m_handleMode && m_showSel) {
        Box2d selbox(getDragRect(sender));
        
        if (m_clones.empty() && !selbox.isEmpty()) {
            GiContext ctxshap(0, GiColor(0, 0, 255, 128), kGiLineDash);
            
            bool antiAlias = gs->setAntiAliasMode(false);
            gs->drawRect(&ctxshap, selbox);
            gs->setAntiAliasMode(antiAlias);
            
            for (int i = sender->view->shapeCanTransform(shapes.front()) ? 7 : -1;
                 i >= 0; i--) {
                mgGetRectHandle(selbox, i, pnt);
                if (!sender->view->drawHandle(gs, pnt, false))
                    gs->drawEllipse(&ctxhd, pnt, radius);
            }
            for (int j = sender->view->shapeCanRotated(shapes.front()) ? 1 : -1;
                j >= 0; j--) {
                mgGetRectHandle(selbox, j == 0 ? 7 : 5, pnt);
                pnt = pnt.rulerPoint(selbox.center(),
                                     -mgDisplayMmToModel(10, sender), 0);
                
                float w = -1.5f * gs->xf().getWorldToDisplayY(false);
                float r = pnt.distanceTo(selbox.center());
                float sangle = mgMin(30.f, mgMax(10.f, mgRad2Deg(12.f / r)));
                GiContext ctxarc(w, GiColor(0, 255, 0, 128),
                                 j ? kGiLineSolid : kGiLineDot);
                gs->drawArc(&ctxarc, selbox.center(), r, r,
                            j ? -mgDeg2Rad(sangle) : mgDeg2Rad(180.f - sangle), 
                            mgDeg2Rad(2.f * sangle));
                
                if (!sender->view->drawHandle(gs, pnt, false))
                    gs->drawEllipse(&ctxhd, pnt, radius);
            }
        }
        else if (!selbox.isEmpty()) {   // 正在拖动临时图形
            if (rorate) {               // 旋转提示的参考线
                if (!sender->view->drawHandle(gs, selbox.center(), false))
                    gs->drawEllipse(&ctxhd, selbox.center(), radius);
                GiContext ctxshap(0, GiColor(0, 0, 255, 128), kGiLineDash);
                gs->drawLine(&ctxshap, selbox.center(), m_ptSnap);
            }
            else {
                if (!sender->view->drawHandle(gs, sender->startPointM, false))
                    gs->drawEllipse(&ctxhd, sender->startPointM, radius);
                if (!sender->view->drawHandle(gs, m_ptSnap, false))
                    gs->drawEllipse(&ctxhd, m_ptSnap, radius);
            }
        }
    }
    
    // 下面显示控制点. 此时仅选中一个图形、有活动控制点
    if (shapes.size() == 1 && m_handleMode && m_showSel) {
        const MgShape* shape = shapes.front();
        
        for (UInt32 i = 0; i < shape->shapec()->getHandleCount(); i++) {
            pnt = shape->shapec()->getHandlePoint(i);
            if (!sender->view->drawHandle(gs, pnt, false))
                gs->drawEllipse(&ctxhd, pnt, radius);
        }
        
        if (m_handleIndex > 0                   // 不是(还未拖动但可插新点)，显示当前控制点
            && (!m_clones.empty() || !m_insertPt)) {
            pnt = shape->shapec()->getHandlePoint(m_handleIndex - 1);
            if (!sender->view->drawHandle(gs, pnt, true))
                gs->drawEllipse(&ctxhd, pnt, r2x);
        }
        if (m_insertPt && !m_clones.empty()) {  // 在临时图形上显示新插入顶点
            GiContext insertctx(ctxhd);
            insertctx.setFillColor(GiColor(255, 0, 0, 64));
            gs->drawEllipse(&insertctx, m_ptNear, r2x);
            sender->view->drawHandle(gs, m_ptNear, true);
        }
        else if (m_clones.empty() && m_ptNear.distanceTo(pnt) > r2x * 2) {
            gs->drawEllipse(&ctxhd, m_ptNear, radius / 2);  // 显示线上的最近点，以便用户插入新点
        }
    }
    
    return true;
}

void MgCommandSelect::gatherShapes(const MgMotion* sender, MgShapes* shapes)
{
    if (m_boxsel) {
        MgShapeT<MgRect> shape;
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), 
                          isIntersectMode(sender) ? kGiLineDash : kGiLineSolid,
                          GiColor(0, 0, 255, 32));
        *shape.context() = ctxshap;
        ((MgRect*)shape.shape())->setRect(sender->startPointM, sender->pointM);
        shapes->addShape(shape);
    }
    for (std::vector<MgShape*>::const_iterator it = m_clones.begin();
         it != m_clones.end(); ++it) {
        shapes->addShape(*(*it));
    }
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

MgShape* MgCommandSelect::hitTestAll(const MgMotion* sender, 
                                     Point2d &nearpt, Int32 &segment)
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
    Box2d limits(sender->startPointM, mgDisplayMmToModel(15, sender), 0);
    return shape && shape->shape()->hitTest(limits.center(), limits.width() / 2, 
                                            m_ptNear, m_segment) <= limits.width() / 2;
}

Int32 MgCommandSelect::hitTestHandles(MgShape* shape, const Point2d& pointM,
                                      const MgMotion* sender)
{
    if (!shape)
        return -1;
    
    UInt32 handleIndex = 0;
    float minDist = mgDisplayMmToModel(5, sender);
    float nearDist = m_ptNear.distanceTo(pointM);
    
    for (UInt32 i = 0; i < shape->shape()->getHandleCount(); i++) {
        float d = pointM.distanceTo(shape->shape()->getHandlePoint(i));
        if (minDist > d) {
            minDist = d;
            handleIndex = i + 1;
        }
    }
    
    if (sender->pressDrag && nearDist < minDist / 3
        && minDist > mgDisplayMmToModel(8, sender)
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        m_insertPt = true;
    }
    
    return handleIndex;
}

bool MgCommandSelect::click(const MgMotion* sender)
{
    m_boxHandle = 99;
    if (sender->pressDrag)
        return false;
    
    Point2d nearpt;
    Int32   segment = -1;
    UInt32  handleIndex = 0;
    MgShape* shape;
    bool    canSelAgain;
    
    if (!m_showSel) {                   // 上次是禁止亮显
        m_showSel = true;               // 恢复亮显选中的图形
        sender->view->regen();          // 可能图形属性已变，重新构建显示
    }
    
    m_insertPt = false;                 // 默认不是插入点，在hitTestHandles中设置
    shape = getSelectedShape(sender);   // 取上次选中的图形
    canSelAgain = (m_selIds.size() == 1 // 多选时不进入热点状态
                   && (m_handleMode || shape->getID() == m_id)
                   && canSelect(shape, sender));    // 仅检查这个图形能否选中
    
    if (canSelAgain) {                  // 可再次选中同一图形，不论其他图形能否选中
        m_handleMode = true;
        handleIndex = hitTestHandles(shape, sender->pointM, sender);    // 检查是否切换热点
        if (m_handleIndex != handleIndex || m_insertPt) {
            m_handleIndex = handleIndex;
        }
    }
    else if (shape && m_handleMode) {       // 上次是热点状态，在该图形外的较远处点击
        m_handleIndex = 0;                  // 恢复到整体选中状态
        m_handleMode = false;
    }
    else {                                  // 上次是整体选中状态或没有选中
        shape = hitTestAll(sender, nearpt, segment);
        bool changed = ((int)m_selIds.size() != (shape ? 1 : 0))
            || (shape && shape->getID() != m_id);

        m_selIds.clear();                   // 清除选择集
        if (shape)
            m_selIds.push_back(shape->getID()); // 选中新图形
        m_id = shape ? shape->getID() : 0;
        
        m_ptNear = nearpt;
        m_segment = segment;
        m_handleIndex = 0;
        m_handleMode = false;

        if (changed)
            sender->view->selChanged();
    }
    sender->view->redraw(false);
    
    if (!sender->pressDrag) {
        sender->view->longPressSelection((MgSelState)getSelectState(sender->view));
    }
    
    return true;
}

bool MgCommandSelect::doubleClick(const MgMotion* /*sender*/)
{
    return false;
}

bool MgCommandSelect::longPress(const MgMotion* sender)
{
    bool ret = false;
    
    if (m_selIds.empty()) {
        ret = click(sender);
    }
    if (sender->view->longPressSelection((MgSelState)getSelectState(sender->view))) {
        ret = true;
    }
    
    return ret;
}

bool MgCommandSelect::touchBegan(const MgMotion* sender)
{
    cloneShapes(sender->view);
    MgShape* shape = m_clones.empty() ? NULL : m_clones.front();
    
    if (!m_showSel) {
        m_showSel = true;
        sender->view->redraw(false);
    }
    
    m_insertPt = false;                          // setted in hitTestHandles
    if (m_clones.size() == 1)
        canSelect(shape, sender);   // calc m_ptNear
    m_handleIndex = (m_clones.size() == 1 && m_handleMode)
    ? hitTestHandles(shape, sender->pointM, sender) : 0;
    
    if (m_insertPt && shape->shape()->isKindOf(MgBaseLines::Type())) {
        MgBaseLines* lines = (MgBaseLines*)(shape->shape());
        lines->insertPoint(m_segment, m_ptNear);
        shape->shape()->update();
        m_handleIndex = hitTestHandles(shape, m_ptNear, sender);
    }
    
    if (m_clones.empty())
        m_boxsel = true;
    m_boxHandle = 99;
    
    sender->view->redraw(m_clones.size() < 2);
    
    return true;
}

bool MgCommandSelect::isIntersectMode(const MgMotion* sender)
{
    return (sender->startPoint.x < sender->point.x
            && sender->startPoint.y < sender->point.y);
}

Box2d MgCommandSelect::getDragRect(const MgMotion* sender)
{
    Box2d selbox;
    
    for (size_t i = 0; i < m_selIds.size(); i++) {
        MgShape* shape = getShape(m_selIds[i], sender);
        if (shape)
            selbox.unionWith(shape->shape()->getExtent());
    }

    float minDist = sender->view->xform()->displayToModel(8);
    if (!m_selIds.empty() && selbox.width() < minDist)
        selbox.inflate(minDist / 2, 0);
    if (!m_selIds.empty() && selbox.height() < minDist)
        selbox.inflate(0, minDist / 2);
    
    Box2d rcview(Box2d(0, 0, sender->view->xform()->getWidth(),
                       sender->view->xform()->getHeight() )
                 * sender->view->xform()->displayToModel());
    
    rcview.deflate(mgDisplayMmToModel(12, sender));
    selbox.intersectWith(rcview);
    
    return selbox;
}

bool MgCommandSelect::isDragRectCorner(const MgMotion* sender, Matrix2d& mat)
{
    m_boxHandle = 99;
    m_ptSnap = sender->pointM;
    
    if (m_handleMode || m_selIds.empty() || m_boxsel)
        return false;
    
    Box2d selbox(getDragRect(sender));
    if (selbox.isEmpty())
        return false;
    
    Point2d pnt;
    int i;
    float mindist = mgDisplayMmToModel(5, sender);
    
    for (i = sender->view->shapeCanTransform(getShape(m_selIds[0], sender)) ? 7 : -1;
         i >= 0; i--) {
        mgGetRectHandle(selbox, i, pnt);
        float addlen = i < 4 ? 0.f : mgDisplayMmToModel(1, sender); // 边中点优先1毫米
        if (mindist > sender->startPointM.distanceTo(pnt) - addlen) {
            mindist = sender->startPointM.distanceTo(pnt) - addlen;
            m_boxHandle = i;
        }
    }

    for (i = sender->view->shapeCanRotated(getShape(m_selIds[0], sender)) ? 1 : -1;
        i >= 0; i--) {
        mgGetRectHandle(selbox, i == 0 ? 7 : 5, pnt);
        pnt = pnt.rulerPoint(selbox.center(), -mgDisplayMmToModel(10, sender), 0);
        if (mindist > sender->startPointM.distanceTo(pnt)) {
            mindist = sender->startPointM.distanceTo(pnt);
            m_boxHandle = 8 + i;
        }
    }
    if (m_boxHandle < 8) {
        Box2d newbox(selbox);
        mgMoveRectHandle(newbox, m_boxHandle, sender->pointM);
        
        if (!selbox.isEmpty() && !newbox.isEmpty()) {
            mat = Matrix2d::scaling((newbox.xmax - newbox.xmin) / selbox.width(),
                                    (newbox.ymax - newbox.ymin) / selbox.height(),
                                    selbox.leftBottom())
            * Matrix2d::translation(newbox.leftBottom() - selbox.leftBottom());
        }
    }
    else if (m_boxHandle < 10) {
        mgGetRectHandle(selbox, m_boxHandle == 8 ? 7 : 5, pnt);
        pnt = pnt.rulerPoint(selbox.center(), -mgDisplayMmToModel(10, sender), 0);
        float angle = (pnt - selbox.center()).angleTo2(sender->pointM - selbox.center());
        
        if (m_boxHandle == 8) {
            angle = mgDeg2Rad(mgRound(mgRad2Deg(angle)) / 15 * 15.f);
        }
        mat = Matrix2d::rotation(angle, selbox.center());
        m_ptSnap = selbox.center().polarPoint(angle + (pnt - selbox.center()).angle2(),
                                              sender->pointM.distanceTo(selbox.center()));
    }
    
    return m_boxHandle < 10;
}

bool MgCommandSelect::touchMoved(const MgMotion* sender)
{
    Point2d pointM(sender->pointM);
    Matrix2d mat;
    bool dragCorner = isDragRectCorner(sender, mat);
    
    if (m_insertPt && pointM.distanceTo(m_ptNear) < mgDisplayMmToModel(5, sender)) {
        pointM = m_ptNear;  // 拖动刚新加的点到起始点时取消新增
    }
    
    for (size_t i = 0; i < m_clones.size(); i++) {
        MgBaseShape* shape = m_clones[i]->shape();
        MgShape* basesp = getShape(m_selIds[i], sender);
        
        if (basesp) {
            shape->copy(*basesp->shape());
        }
        if (m_insertPt && shape->isKindOf(MgBaseLines::Type())) {
            MgBaseLines* lines = (MgBaseLines*)shape;
            lines->insertPoint(m_segment, m_ptNear);
        }
        if (m_handleIndex > 0) {
            float tol = mgDisplayMmToModel(5, sender);
            shape->setHandlePoint(m_handleIndex - 1, pointM, tol);
        }
        else if (dragCorner) {
            shape->transform(mat);
        }
        else {
            shape->offset(pointM - sender->startPointM, m_segment);
            sender->view->shapeMoved(m_clones[i], m_segment);
        }
        shape->update();
        sender->view->redraw(m_clones.size() < 2);
    }
    
    if (m_clones.empty() && m_boxsel) {    // 没有选中图形时就滑动多选
        Box2d snap(sender->startPointM, sender->pointM);
        void *it = NULL;
        MgShape* shape = sender->view->shapes()->getFirstShape(it);
        
        m_selIds.clear();
        m_id = 0;
        for (; shape; shape = sender->view->shapes()->getNextShape(it)) {
            if (isIntersectMode(sender) ? shape->shape()->hitTestBox(snap)
                : snap.contains(shape->shape()->getExtent())) {
                m_selIds.push_back(shape->getID());
                m_id = shape->getID();
            }
        }
        sender->view->shapes()->freeIterator(it);
        sender->view->redraw(true);
    }
    
    return true;
}

bool MgCommandSelect::isCloneDrag(const MgMotion* sender)
{
    float dist = sender->pointM.distanceTo(sender->startPointM);
    return (!m_handleMode && m_boxHandle > 16 && sender->pressDrag
            && dist > mgDisplayMmToModel(5, sender));
}

bool MgCommandSelect::touchEnded(const MgMotion* sender)
{
    // 拖动刚新加的点到起始点时取消新增
    if (m_insertPt && m_clones.size() == 1
        && sender->pointM.distanceTo(m_ptNear) < mgDisplayMmToModel(5, sender)) {
        m_clones[0]->release();
        m_clones.clear();
    }
    
    applyCloneShapes(sender->view, true, isCloneDrag(sender));
    
    m_insertPt = false;
    m_ptNear = sender->pointM;
    m_boxHandle = 99;
    
    if (m_handleMode) {
        m_handleIndex = hitTestHandles(getShape(m_selIds[0], sender), sender->pointM, sender);
        sender->view->redraw(true);
    }
    if (m_boxsel) {
        m_boxsel = false;
        if (!m_selIds.empty())
            sender->view->selChanged();
    }
    
    return true;
}

void MgCommandSelect::cloneShapes(MgView* view)
{
    for (std::vector<MgShape*>::iterator it = m_clones.begin();
         it != m_clones.end(); ++it) {
        (*it)->release();
    }
    m_clones.clear();
    
    for (sel_iterator its = m_selIds.begin(); its != m_selIds.end(); ++its) {
        MgShape* shape = view->shapes()->findShape(*its);
        if (shape) {
            shape = (MgShape*)(shape->clone());
            if (shape)
                m_clones.push_back(shape);
        }
    }
}

bool MgCommandSelect::applyCloneShapes(MgView* view, bool apply, bool addNewShapes)
{
    bool changed = false;
    bool cloned = !m_clones.empty();
    
    if (!m_clones.empty()) {
        MgShapesLock locker(view->shapes(), !apply ? MgShapesLock::ReadOnly
                            : (addNewShapes ? MgShapesLock::Add : MgShapesLock::Edit));
        
        if (apply && addNewShapes) {
            m_selIds.clear();
            m_id = 0;
        }
        for (size_t i = 0; i < m_clones.size(); i++) {
            if (apply && addNewShapes) {
                MgShape* newsp = view->shapes()->addShape(*(m_clones[i]));
                if (newsp) {
                    view->shapeAdded(newsp);
                    m_selIds.push_back(newsp->getID());
                    m_id = newsp->getID();
                    changed = true;
                }
            }
            else if (apply) {
                MgShape* shape = (i < m_selIds.size() ?
                                  view->shapes()->findShape(m_selIds[i]) : NULL);
                if (shape) {
                    shape->copy(*m_clones[i]);
                    shape->shape()->update();
                    changed = true;
                }
            }
            
            m_clones[i]->release();
            m_clones[i] = NULL;
        }
        m_clones.clear();
    }
    if (changed) {
        view->regen();
        if (addNewShapes)
            view->selChanged();
    }
    else {
        view->redraw(true);
    }
    m_boxsel = false;
    
    return changed || cloned;
}

MgSelState MgCommandSelect::getSelectState(MgView* view)
{
    MgSelState state = kMgSelNone;
    
    if (m_handleMode) {
        MgShape* shape = view->shapes()->findShape(m_id);
        state = m_handleIndex > 0 && shape && shape->shape()->isKindOf(MgBaseLines::Type()) ?
            kMgSelVertex : kMgSelVertexes;
    }
    else if (!m_selIds.empty()) {
        state = m_selIds.size() > 1 ? kMgSelMultiShapes : kMgSelOneShape;
    }
    
    return state;
}

bool MgCommandSelect::selectAll(MgView* view)
{
    size_t oldn = m_selIds.size();
    void* it = NULL;
    
    m_selIds.clear();
    m_handleIndex = 0;
    m_handleMode = false;
    m_insertPt = false;
    m_boxsel = false;
    
    for (MgShape* shape = view->shapes()->getFirstShape(it);
         shape; shape = view->shapes()->getNextShape(it)) {
        m_selIds.push_back(shape->getID());
        m_id = shape->getID();
    }
    view->shapes()->freeIterator(it);
    view->redraw(false);

    if (oldn != m_selIds.size() || !m_selIds.empty()) {
        view->selChanged();
    }
    
    return oldn != m_selIds.size();
}

bool MgCommandSelect::deleteSelection(MgView* view)
{
    MgShapesLock locker(view->shapes(), MgShapesLock::Edit);
    int count = 0;
    
    applyCloneShapes(view, false);
    for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
        MgShape* shape = view->shapes()->removeShape(*it);
        if (shape) {
            shape->release();
            count++;
        }
    }
    m_selIds.clear();
    m_id = 0;
    m_handleIndex = 0;
    m_handleMode = false;
    
    if (count > 0) {
        view->regen();
        view->selChanged();
    }
    
    return count > 0;
}

bool MgCommandSelect::cloneSelection(MgView* view)
{
    cloneShapes(view);
    
    float dist = mgDisplayMmToModel(10, view->graph());
    for (size_t i = 0; i < m_clones.size(); i++) {
        m_clones[i]->shape()->offset(Vector2d(dist, -dist), -1);
    }
    
    return applyCloneShapes(view, true, true);
}

void MgCommandSelect::resetSelection(MgView* view)
{
    bool has = !m_selIds.empty();
    applyCloneShapes(view, false);
    m_selIds.clear();
    m_id = 0;
    m_handleIndex = 0;
    m_handleMode = false;
    if (has) {
        view->selChanged();
    }
}

bool MgCommandSelect::addSelection(MgView* view, UInt32 shapeID)
{
    MgShape* shape = view->shapes()->findShape(shapeID);
    
    if (shape && !isSelected(shape))
    {
        m_selIds.push_back(shape->getID());
        m_id = shape->getID();
        view->redraw(true);
        view->selChanged();
    }

    return shape != NULL;
}

bool MgCommandSelect::deleteVertext(const MgMotion* sender)
{
    MgShape* shape = getSelectedShape(sender);
    bool ret = false;
    
    if (shape && m_handleIndex > 0
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        MgShapesLock locker(sender->view->shapes(), MgShapesLock::Edit);
        MgBaseLines *lines = (MgBaseLines *)shape->shape();
        
        ret = lines->removePoint(m_handleIndex - 1);
        if (ret) {
            shape->shape()->update();
            sender->view->regen();
            m_handleIndex = hitTestHandles(shape, m_ptNear, sender);
        }
    }
    m_insertPt = false;
    
    return ret;
}

bool MgCommandSelect::insertVertext(const MgMotion* sender)
{
    MgShape* shape = getSelectedShape(sender);
    bool ret = false;
    
    if (shape && m_handleMode
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        MgShapesLock locker(sender->view->shapes(), MgShapesLock::Edit);
        MgBaseLines *lines = (MgBaseLines *)shape->shape();
        float dist = m_ptNear.distanceTo(shape->shape()->getPoint(m_segment));
        
        ret = (dist > mgDisplayMmToModel(1, sender)
               && lines->insertPoint(m_segment, m_ptNear));
        if (ret) {
            shape->shape()->update();
            sender->view->regen();
            m_handleIndex = hitTestHandles(shape, m_ptNear, sender);
        }
    }
    m_insertPt = false;
    
    return ret;
}

bool MgCommandSelect::switchClosed(MgView* view)
{
    MgShape* shape = view->shapes()->findShape(m_id);
    bool ret = false;
    
    if (shape && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        MgShapesLock locker(view->shapes(), MgShapesLock::Edit);
        MgBaseLines *lines = (MgBaseLines *)shape->shape();
        
        lines->setClosed(!lines->isClosed());
        shape->shape()->update();
        view->regen();
        ret = true;
    }
    
    return ret;
}

bool MgCommandSelect::isFixedLength(MgView* view)
{
    MgShape* shape = view->shapes()->findShape(m_id);
    return shape && shape->shape()->isFixedLength();
}

bool MgCommandSelect::setFixedLength(MgView* view, bool fixed)
{
    MgShapesLock locker(view->shapes(), MgShapesLock::Edit);
    int count = 0;
    
    for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
        MgShape* shape = view->shapes()->findShape(*it);
        if (shape) {
            shape->shape()->setFixedLength(fixed);
            count++;
        }
    }
    if (count > 0) {
        view->regen();
    }
    
    return count > 0;
}
