// mgdrawtriang.cpp: 实现三角形绘图命令
// Author: pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawtriang.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>
#include <stdio.h>

MgCmdDrawTriangle::MgCmdDrawTriangle()
{
}

MgCmdDrawTriangle::~MgCmdDrawTriangle()
{
}

bool MgCmdDrawTriangle::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgLines>::create, sender);
}

bool MgCmdDrawTriangle::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step == 2) {
        const MgBaseShape* sp = dynshape()->shapec();
        int n = sp->getPointCount();
        float rmax = gs->xf().displayToModel(8, true);
        float rf = 1e10f;
        
        for (int t = 0; t < 2; t++) {
            for (int i = 0; i < n; i++) {
                Point2d center(sp->getHandlePoint(i));
                Point2d pt1(sp->getHandlePoint((i + 1) % n));
                Point2d pt2(sp->getHandlePoint((i - 1 + n) % n));
                float r = mgMin(rmax, 0.3f *
                                mgMin(pt1.distanceTo(center), pt2.distanceTo(center)));
                
                if (t == 0) {
                    rf = mgMin(rf, r);
                }
                else {
                    r = rf;
                    
                    GiContext ctx;
                    ctx.setFillColor(GiColor::Black());
                    gs->drawRect(&ctx, Box2d::kIdentity());
                    
                    Point2d pc1(center.rulerPoint(pt1, r, 0));
                    Point2d pc2(center.rulerPoint(pt2, r, 0));
                    Point2d pnt((center + pc1 + pc2) / 3);
                    
                    pnt *= gs->xf().modelToDisplay();
                    r *= fabs(gs->xf().modelToDisplay().m22);
                    
                    char text[5] = "";
                    float angle = fabs((pt1 - center).angleTo2(pt2 - center) * _M_R2D);
                    
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
                    sprintf_s(text, sizeof(text), "%d", mgRound(angle));
#else
                    sprintf(text, "%d", mgRound(angle));
#endif
                    gs->rawTextCenter(text, pnt.x, pnt.y, r * 0.7f);
                }
            }
        }
    }
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawTriangle::touchBegan(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    if (0 == m_step) {
        m_step = 1;
        lines->setClosed(true);
        lines->resize(3);
        Point2d pnt(snapPoint(sender, true));
        for (int i = 0; i < 3; i++) {
            dynshape()->shape()->setPoint(i, pnt);
        }
    }
    else {
        dynshape()->shape()->setPoint(m_step, snapPoint(sender));
    }
    
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawTriangle::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(m_step, snapPoint(sender));
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawTriangle::touchEnded(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender));
    float distmin = mgDisplayMmToModel(2.f, sender);
    
    dynshape()->shape()->setPoint(m_step, pnt);
    dynshape()->shape()->update();
    
    if (pnt.distanceTo(dynshape()->shape()->getPoint(m_step - 1)) > distmin) {
        m_step++;
        if (3 == m_step) {
            _addshape(sender);
            _delayClear();
            m_step = 0;
        }
    }

    return _touchEnded(sender);
}

// MgCmdArc3P
//

bool MgCmdArc3P::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgArc>::create, sender);
}

bool MgCmdArc3P::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 0) {
        for (int i = 0; i <= m_step && i < 3; i++) {
            sender->view->drawHandle(gs, _points[i], true);
        }
        if (m_step >= 1) {
            GiContext ctx(0, GiColor(0, 126, 0, 64), kGiLineDashDot);
            gs->drawLine(&ctx, _points[0], _points[1]);
            MgArc* arc = (MgArc*)dynshape()->shape();
            gs->drawEllipse(&ctx, arc->getCenter(), arc->getRadius());
        }
    }
    return _draw(sender, gs);
}

void MgCmdArc3P::setStepPoint(int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();

    if (step == 0) {
        _points[0] = pt;
    }
    else if (step == 1) {
        _points[1] = pt;
        _points[2] = pt;
        arc->setStartMidEnd(_points[0], pt, pt);
    }
    else if (step == 2) {
        _points[2] = pt;
        arc->setStartMidEnd(_points[0], _points[1], pt);
    }
}

void MgCmdArcCSE::setStepPoint(int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();

    if (step == 0) {
        _points[0] = pt;    // 记下圆心
    }
    else if (step == 1) {
        _points[1] = pt;    // 记下起点
        _points[2] = pt;    // 起点与终点重合
        arc->setCenterStartEnd(_points[0], pt, pt); // 初始转角为0
    }
    else if (step == 2) {
        arc->setCenterStartEnd(_points[0], _points[1], pt);
        _points[2] = pt;    // 记下终点
    }
}

void MgCmdArcTan::setStepPoint(int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();

    if (step == 0) {
        _points[0] = pt;
    }
    else if (step == 1) {
        _points[1] = pt;
        _points[2] = pt;
        arc->setTanStartEnd(_points[1] - _points[0], pt, pt);
    }
    else if (step == 2) {
        arc->setTanStartEnd(_points[1] - _points[0], _points[1], pt);
        _points[2] = pt;
    }
}
