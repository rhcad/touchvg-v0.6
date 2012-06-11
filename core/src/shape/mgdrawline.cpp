// mgdrawlines.cpp: 实现线段绘图命令类
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
    m_shape->shape()->setPoint(0, sender->pointM);
    m_shape->shape()->setPoint(1, sender->pointM);
    m_shape->shape()->update();

    return _touchBegan(sender);
}

bool MgCmdDrawLine::touchMoved(const MgMotion* sender)
{
    m_shape->shape()->setPoint(1, sender->pointM);
    m_shape->shape()->update();

    return _touchMoved(sender);
}

bool MgCmdDrawLine::touchEnded(const MgMotion* sender)
{
    m_shape->shape()->setPoint(1, sender->pointM);
    m_shape->shape()->update();

    if ( ((MgLine*)m_shape->shape())->length() > mgDisplayMmToModel(2, sender))
    {
        _addshape(sender);
    }
    m_shape->shape()->clear();
    m_step = 0;

    return _touchEnded(sender);
}
