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
    ((MgBaseRect*)dynshape()->shape())->setRect(Box2d(sender->pointM, sender->pointM));
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawRect::touchMoved(const MgMotion* sender)
{
    ((MgBaseRect*)dynshape()->shape())->setRect(Box2d(sender->startPointM, sender->pointM));
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawRect::touchEnded(const MgMotion* sender)
{
    ((MgBaseRect*)dynshape()->shape())->setRect(Box2d(sender->startPointM, sender->pointM));
    dynshape()->shape()->update();

    float minDist = mgDisplayMmToModel(1, sender);

    if (! ((MgBaseRect*)dynshape()->shape())->isEmpty(minDist)) {
        _addshape(sender);
    }
    _delayClear();

    return _touchEnded(sender);
}
