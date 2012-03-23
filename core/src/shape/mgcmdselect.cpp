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
    sender; return false;
}

bool MgCommandSelect::initialize(const MgMotion* sender)
{
    sender; return true;
}

bool MgCommandSelect::undo(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    sender; return false;
}

bool MgCommandSelect::click(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandSelect::doubleClick(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandSelect::longPress(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandSelect::touchesBegan(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandSelect::touchesMoved(const MgMotion* sender)
{
    sender; return false;
}

bool MgCommandSelect::touchesEnded(const MgMotion* sender)
{
    sender; return false;
}
