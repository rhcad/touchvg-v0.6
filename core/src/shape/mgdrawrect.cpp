// mgdrawrect.cpp: 实现矩形绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawrect.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCmdDrawRect::MgCmdDrawRect()
{
}

MgCmdDrawRect::~MgCmdDrawRect()
{
}

bool MgCmdDrawRect::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgRect>::create, sender);
}

bool MgCmdDrawRect::undo(bool &, const MgMotion* sender)
{
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawRect::touchBegan(const MgMotion* sender)
{
    m_step = 1;
    ((MgBaseRect*)m_shape->shape())->setRect(Box2d(sender->pointM, sender->pointM));
    m_shape->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawRect::touchMoved(const MgMotion* sender)
{
    ((MgBaseRect*)m_shape->shape())->setRect(Box2d(sender->startPointM, sender->pointM));
    m_shape->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawRect::touchEnded(const MgMotion* sender)
{
    ((MgBaseRect*)m_shape->shape())->setRect(Box2d(sender->startPointM, sender->pointM));
    m_shape->shape()->update();

    float minDist = sender->view->xform()->displayToModel(10);

    if (! ((MgBaseRect*)m_shape->shape())->isEmpty(minDist)) {
        _addshape(sender);
    }

    m_shape->shape()->clear();
    m_step = 0;

    return _touchEnded(sender);
}
