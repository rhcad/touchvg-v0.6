// mgcmdmgr.cpp: 实现命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgcmdmgr.h"

static MgCmdManagerImpl s_cmds;
MgCommand* mgCreateCommand(const char* name);
int mgGetSelection(MgCommand* cmd, MgView* view, int count, MgShape** shapes);

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
    if (it == _cmds.end())
    {
        MgCommand* cmd = mgCreateCommand(name);
        if (cmd) {
            _cmds[name] = cmd;
            it = _cmds.find(name);
        }
    }
    _cmdname = (it != _cmds.end()) ? name : "";

    return it != _cmds.end() && it->second->initialize(sender);
}

bool MgCmdManagerImpl::cancel(const MgMotion* sender)
{
    CMDS::iterator it = _cmds.find(_cmdname);
    if (it != _cmds.end()) {
        return it->second->cancel(sender);
    }
    return false;
}

int MgCmdManagerImpl::getSelection(MgView* view, int count, MgShape** shapes)
{
    return view ? mgGetSelection(getCommand(), view, count, shapes) : 0;
}