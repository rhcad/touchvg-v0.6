// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdselect.h"

MgCommandSelect::MgCommandSelect()
{
}

MgCommandSelect::~MgCommandSelect()
{
}

bool MgCommandSelect::cancel(const MgMotion* sender)
{
    if (!m_selection.empty())
        return undo(sender);
    return false;
}

bool MgCommandSelect::initialize(const MgMotion* /*sender*/)
{
    m_selection.clear();
    m_id = 0;
    m_segment = -1;

    return true;
}

bool MgCommandSelect::undo(const MgMotion* sender)
{
    if (!m_selection.empty()) {
        m_selection.clear();
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    std::vector<UInt32>::const_iterator it;
    GiContext context(-6, GiColor(0, 0, 255, 50));
    GiContext ctxaux(0, GiColor(64, 64, 64, 128), kLineSolid, GiColor(0, 64, 64, 168));
    double radius = (Vector2d(5, 5) * gs->xf().displayToModel()).length();

    for (it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        const MgShape* shape = sender->view->shapes()->findShape(*it);
        if (shape) {
            shape->draw(*gs, &context);
            for (UInt32 i = 0; i < shape->shape()->getPointCount(); i++) {
                gs->drawEllipse(&ctxaux, shape->shape()->getPoint(i), radius);
            }
        }
    }

    return true;
}

bool MgCommandSelect::click(const MgMotion* sender)
{
    Box2d limits(Point2d(sender->point.x, sender->point.y), 50, 0);
    MgShape* shape;
    Point2d ptNear;
    Int32 segment = -1;
    
    shape = sender->view->shapes()->hitTest(
        limits * sender->view->xform()->displayToModel(),
        ptNear, segment);
    UInt32 id = shape ? shape->getID() : 0;

    if (m_selection.empty() && !shape)
        return true;

    m_selection.clear();
    m_selection.push_back(id);
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

bool MgCommandSelect::touchBegan(const MgMotion* /*sender*/)
{
    return true;
}

bool MgCommandSelect::touchMoved(const MgMotion* /*sender*/)
{
    return true;
}

bool MgCommandSelect::touchEnded(const MgMotion* /*sender*/)
{
    return true;
}
