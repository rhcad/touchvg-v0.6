// mgcmdmgr.cpp: 实现命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdmgr.h"

static MgCmdManagerImpl s_cmds;

MgCommandManager* mgGetCommandManager()
{
    return &s_cmds;
}

MgCmdManagerImpl::MgCmdManagerImpl()
{
}

MgCmdManagerImpl::~MgCmdManagerImpl()
{
}

UInt32 MgCmdManagerImpl::getCommandID()
{
    return 0;
}

MgCommand* MgCmdManagerImpl::getCommand()
{
    return NULL;
}

bool MgCmdManagerImpl::setCommandID(UInt32 cmdID)
{
    return false;
}
