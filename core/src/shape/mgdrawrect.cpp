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
    Point2d pnt(snapPoint(sender, true));
    ((MgBaseRect*)dynshape()->shape())->setRect(pnt, pnt);
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawRect::touchMoved(const MgMotion* sender)
{
    ((MgBaseRect*)dynshape()->shape())->setRect(sender->startPointM, sender->pointM);
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawRect::touchEnded(const MgMotion* sender)
{
    ((MgBaseRect*)dynshape()->shape())->setRect(sender->startPointM, sender->pointM);
    dynshape()->shape()->update();

    float minDist = mgDisplayMmToModel(1, sender);

    if (! ((MgBaseRect*)dynshape()->shape())->isEmpty(minDist)) {
        _addshape(sender);
    }
    _delayClear();

    return _touchEnded(sender);
}

bool MgCmdDrawEllipse::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgEllipse>::create, sender);
}

bool MgCmdDrawDiamond::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgDiamond>::create, sender);
}

bool MgCmdDrawSquare::initialize(const MgMotion* sender)
{
    bool ret = _initialize(MgShapeT<MgRect>::create, sender);
    
    MgBaseRect* rect = (MgBaseRect*)dynshape()->shape();
    rect->setSquare(true);
    
    return ret;
}

bool MgCmdDrawCircle::initialize(const MgMotion* sender)
{
    bool ret = _initialize(MgShapeT<MgEllipse>::create, sender);
    
    MgBaseRect* rect = (MgBaseRect*)dynshape()->shape();
    rect->setSquare(true);
    
    return ret;
}
