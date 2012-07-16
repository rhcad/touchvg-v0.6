// mgcmdmgr.cpp: 实现命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr.h"
#include "mgcmdselect.h"

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
    _cmdname = "";
}

void MgCmdManagerImpl::registerCommand(const char* name, MgCommand* (*factory)())
{
    _factories[name] = factory;
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
        MgCommand* cmd = NULL;
        Factories::iterator itf = _factories.find(name);
        
        if (itf != _factories.end()) {
            cmd = itf->second ? (itf->second)() : NULL;
        }
        if (!cmd) {
            cmd = mgCreateCommand(name);
        }
        if (cmd) {
            _cmds[name] = cmd;
            it = _cmds.find(name);
        }
    }
    
    bool ret = (it != _cmds.end() && it->second->initialize(sender));
    _cmdname = ret ? name : "";     // change it at end of initialization

    return ret;
}

bool MgCmdManagerImpl::cancel(const MgMotion* sender)
{
    CMDS::iterator it = _cmds.find(_cmdname);
    if (it != _cmds.end()) {
        return it->second->cancel(sender);
    }
    return false;
}

UInt32 MgCmdManagerImpl::getSelection(MgView* view, UInt32 count, MgShape** shapes, bool forChange)
{
    if (_cmdname == MgCommandSelect::Name() && view) {
        MgCommandSelect* sel = (MgCommandSelect*)getCommand();
        return sel ? sel->getSelection(view, count, shapes, forChange) : 0;
    }
    return 0;
}

bool MgCmdManagerImpl::dynamicChangeEnded(MgView* view, bool apply)
{
    bool changed = false;
    if (_cmdname == MgCommandSelect::Name() && view) {
        MgCommandSelect* sel = (MgCommandSelect*)getCommand();
        changed = sel && sel->dynamicChangeEnded(view, apply);
    }
    return changed;
}

MgSelection* MgCmdManagerImpl::getSelection(MgView* view)
{
    return view && _cmdname == MgCommandSelect::Name()
        ? (MgCommandSelect*)getCommand() : NULL;
}
