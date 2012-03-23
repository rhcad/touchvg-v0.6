// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmddraw.h"

MgCommandDraw::MgCommandDraw()
{
}

MgCommandDraw::~MgCommandDraw()
{
}

bool MgCommandDraw::cancel(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::initialize(const MgMotion* sender)
{
    sender; return true;
}

bool MgCommandDraw::undo(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::draw(const MgMotion* sender, GiGraphics* gs)
{
    sender; return false;
}

bool MgCommandDraw::click(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::doubleClick(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::longPress(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::touchesBegan(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::touchesMoved(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandDraw::touchesEnded(const MgMotion* sender)
{
    sender; return false;
}
