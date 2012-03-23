// mgcmdmgr.cpp: 实现命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdmgr.h"

static MgCmdManagerImpl s_cmds;
MgCommand* mgCreateCommand(const char* name);

MgCommandManager* mgGetCommandManager()
{
    return &s_cmds;
}

MgCmdManagerImpl::MgCmdManagerImpl()
{
}

MgCmdManagerImpl::~MgCmdManagerImpl()
{
    unloadCommands();
}

void MgCmdManagerImpl::unloadCommands()
{
    for (CMDS::iterator it = _cmds.begin(); it != _cmds.end(); ++it)
        it->second->release();
    _cmds.clear();
}

const char* MgCmdManagerImpl::getCommandName()
{
    return _cmdname.c_str();
}

MgCommand* MgCmdManagerImpl::getCommand()
{
    CMDS::iterator it = _cmds.find(_cmdname);
    return it != _cmds.end() ? it->second : NULL;
}

bool MgCmdManagerImpl::setCommand(const MgMotion* sender, const char* name)
{
    cancel(sender);

    CMDS::iterator it = _cmds.find(name);
    _cmdname = (it != _cmds.end()) ? name : "";

    return it != _cmds.end() && it->second->initialize(sender);
}

void MgCmdManagerImpl::cancel(const MgMotion* sender)
{
    CMDS::iterator it = _cmds.find(_cmdname);
    if (it != _cmds.end()) {
        it->second->cancel(sender);
    }
}
