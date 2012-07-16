// mgdrawlines.cpp: 实现直线段绘图命令
// Author: pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawline.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include <mgbase.h>

MgCmdDrawLine::MgCmdDrawLine()
{
}

MgCmdDrawLine::~MgCmdDrawLine()
{
}

bool MgCmdDrawLine::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgLine>::create, sender);
}

bool MgCmdDrawLine::undo(bool &, const MgMotion* sender)
{
    return MgCommandDraw::_undo(sender);
}

bool MgCmdDrawLine::touchBegan(const MgMotion* sender)
{
    m_step = 1;
    dynshape()->shape()->setPoint(0, sender->pointM);
    dynshape()->shape()->setPoint(1, sender->pointM);
    dynshape()->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawLine::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(1, sender->pointM);
    dynshape()->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawLine::touchEnded(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(1, sender->pointM);
    dynshape()->shape()->update();

    if ( ((MgLine*)dynshape()->shape())->length() > mgDisplayMmToModel(2, sender))
    {
        _addshape(sender);
    }
    _delayClear();

    return _touchEnded(sender);
}
