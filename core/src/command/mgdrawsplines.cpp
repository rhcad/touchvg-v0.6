// mgdrawsplines.cpp: 实现曲线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawsplines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>

MgCmdDrawSplines::MgCmdDrawSplines() : m_freehand(true)
{
}

MgCmdDrawSplines::~MgCmdDrawSplines()
{
}

bool MgCmdDrawSplines::initialize(const MgMotion* sender)
{
    m_freehand = sender->view->useFinger();
    return _initialize(MgShapeT<MgSplines>::create, sender);
}

bool MgCmdDrawSplines::undo(bool &enableRecall, const MgMotion* sender)
{
    enableRecall = m_freehand;
    if (m_step > 1) {                   // freehand: 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)dynshape()->shape())->removePoint(m_freehand ? m_step - 1 : m_step);
        dynshape()->shape()->update();
    }
    
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawSplines::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (getStep() > 0 && !m_freehand) {
        GiContext ctx(0, GiColor(64, 128, 64, 172), kGiLineSolid, GiColor(0, 64, 64, 128));
        float radius = gs->xf().displayToModel(4);
        
        for (int i = 1, n = dynshape()->shape()->getPointCount(); i < 6 && n >= i; i++) {
            gs->drawEllipse(&ctx, dynshape()->shape()->getPoint(n - i), radius);
        }
        gs->drawEllipse(&ctx, dynshape()->shape()->getPoint(0), radius * 1.5f);
    }
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawSplines::touchBegan(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    if (m_step > 0 && !m_freehand) {
        m_step++;
        if (m_step >= dynshape()->shape()->getPointCount()) {
            lines->addPoint(lines->endPoint());
            dynshape()->shape()->update();
        }
        
        return _touchMoved(sender);
    }
    else {
        lines->resize(2);
        lines->setClosed(false);
        m_step = 1;
        m_freehand = sender->view->useFinger();
        Point2d pnt(m_freehand ? sender->startPointM : snapPoint(sender, true));
        dynshape()->shape()->setPoint(0, pnt);
        dynshape()->shape()->setPoint(1, pnt);
        dynshape()->shape()->update();
        
        return _touchBegan(sender);
    }
}

bool MgCmdDrawSplines::mouseHover(const MgMotion* sender)
{
    return !m_freehand && m_step > 0 && touchMoved(sender);
}

bool MgCmdDrawSplines::touchMoved(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    Point2d pnt(!m_freehand ? snapPoint(sender)
        : (sender->pointM + sender->lastPointM) / 2);   // 中点采样法
    
    dynshape()->shape()->setPoint(m_step, pnt);
    if (m_step > 0 && canAddPoint(sender, false)) {
        m_step++;
        if (m_step >= dynshape()->shape()->getPointCount()) {
            lines->addPoint(pnt);
        }
    }
    dynshape()->shape()->update();
    
    return _touchMoved(sender);
}

bool MgCmdDrawSplines::touchEnded(const MgMotion* sender)
{
    if (m_freehand) {
        if (m_step > 1) {
            //MgSplines* splines = (MgSplines*)dynshape()->shape();
            //splines->smooth(mgLineHalfWidthModel(m_shape, sender) + mgDisplayMmToModel(1, sender));
            _addshape(sender);
        }
        else {
            click(sender);  // add a point
        }
        _delayClear();
    }
    else {
        MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
        float dist = lines->endPoint().distanceTo(dynshape()->shape()->getPoint(0));

        while (m_step > 1 && dist < mgDisplayMmToModel(1.f, sender)) {
            lines->setClosed(true);
            lines->removePoint(m_step--);
            dist = lines->endPoint().distanceTo(dynshape()->shape()->getPoint(0));
        }
        if (m_step > 1 && lines->isClosed()) {
            _addshape(sender);
            _delayClear();
        }
        else if (sender->startPointM.distanceTo(sender->pointM) >
                mgDisplayMmToModel(5.f, sender)) {
            m_step++;
            if (m_step >= dynshape()->shape()->getPointCount()) {
                lines->addPoint(lines->endPoint());
                dynshape()->shape()->update();
            }
        }
    }
    
    return _touchEnded(sender);
}

bool MgCmdDrawSplines::doubleClick(const MgMotion* sender)
{
    if (!m_freehand) {
        if (m_step > 1) {
            MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
            float dist = lines->endPoint().distanceTo(dynshape()->shape()->getPoint(m_step - 1));

            if (dist < mgDisplayMmToModel(2.f, sender)) {   // 最后两点重合
                lines->removePoint(m_step--);               // 去掉最末点
            }
        }
        if (m_step > 1) {
            _addshape(sender);
            _delayClear();
        }
        return true;
    }
    return click(sender);
}

bool MgCmdDrawSplines::cancel(const MgMotion* sender)
{
    if (!m_freehand && m_step > 1) {
        MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
        lines->removePoint(m_step--);
        _addshape(sender);
    }
    return MgCommandDraw::cancel(sender);
}

bool MgCmdDrawSplines::canAddPoint(const MgMotion* sender, bool ended)
{
    if (!m_freehand && !ended)
        return false;
    
    if (m_step > 0) {
        float dist = sender->pointM.distanceTo(dynshape()->shape()->getPoint(m_step - 1));
        if (dist < mgDisplayMmToModel(ended ? 0.2f : 0.5f, sender))
            return false;
    }
    
    return true;
}

bool MgCmdDrawSplines::click(const MgMotion* sender)
{
    if (m_freehand) {
        MgShapeT<MgLine> line;
        
        if (sender->view->context()) {
            *line.context() = *sender->view->context();
        }
        
        Point2d pt (sender->pointM);
        
        if (sender->point.distanceTo(sender->startPoint) < 1.f) {
        	pt = (sender->point + Vector2d(1.f, 1.f)) * sender->view->xform()->displayToModel();
        }
        line.shape()->setPoint(0, sender->startPointM);
        line.shape()->setPoint(1, pt);
        
        if (sender->view->shapeWillAdded(&line)) {
            _addshape(sender, &line);
        }
        
        return true;
    }

    if (!m_freehand && m_step == 0) {
        return touchBegan(sender) && touchEnded(sender);
    }
    return MgCommandDraw::click(sender);
}
