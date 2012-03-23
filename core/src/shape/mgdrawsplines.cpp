// mgdrawsplines.cpp: 实现曲线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgdrawsplines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCmdDrawSplines::MgCmdDrawSplines()
{
}

MgCmdDrawSplines::~MgCmdDrawSplines()
{
}

bool MgCmdDrawSplines::initialize(const MgMotion* sender)
{
    return _initialize(MgShapeT<MgSplines>::create, sender);
}

bool MgCmdDrawSplines::canAddPoint(const MgMotion* sender)
{
    return MgCmdBaseLines::canAddPoint(sender);
}
