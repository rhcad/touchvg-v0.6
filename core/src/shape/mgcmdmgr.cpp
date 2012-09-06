// mgcmdmgr.cpp: 实现命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr.h"
#include "mgcmdselect.h"
#include <mgbasicsp.h>

MgCommand* mgCreateCoreCommand(const char* name);
float mgDisplayMmToModel(float mm, const MgMotion* sender);

typedef std::map<std::string, MgCommand* (*)()> Factories;
static Factories    _factories;
static MgCmdManagerImpl* s_manager = NULL;
static MgCmdManagerImpl s_tmpmgr(true);

void mgRegisterCommand(const char* name, MgCommand* (*factory)())
{
    if (!factory) {
        _factories.erase(name);
    }
    else {
        _factories[name] = factory;
    }
}

MgCommandManager* mgGetCommandManager()
{
    return s_manager ? s_manager : &s_tmpmgr;
}

MgCmdManagerImpl::MgCmdManagerImpl(bool tmpobj)
{
    if (!s_manager && !tmpobj)
        s_manager = this;
}

MgCmdManagerImpl::~MgCmdManagerImpl()
{
    unloadCommands();
    if (s_manager == this)
        s_manager = NULL;
}

void MgCmdManagerImpl::unloadCommands()
{
    for (CMDS::iterator it = _cmds.begin(); it != _cmds.end(); ++it)
        it->second->release();
    _cmds.clear();
    _cmdname = "";
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
            cmd = mgCreateCoreCommand(name);
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

int MgCmdManagerImpl::snapHandlePoint(MgMotion* sender, float mm)
{
    MgCommand* cmd = getCommand();
    if (!sender || !sender->view || !cmd)
        return -1;
    
    Box2d snapbox(Box2d(sender->point, 2 * mm, 0) * sender->view->xform()->displayToModel());
    void* it = NULL;
    float mindist = snapbox.width();
    MgShape* spfound = NULL;
    MgShape* exceptsp = sender->point != sender->startPoint ? cmd->getCurrentShape(sender) : NULL;
    
    for (MgShape* sp = sender->view->shapes()->getFirstShape(it);
         sp; sp = sender->view->shapes()->getNextShape(it)) {
        
        if (sp != exceptsp && sp->shape()->getExtent().isIntersect(snapbox)) {
            UInt32 n = sp->shape()->getHandleCount();
            bool curve = sp->shape()->isKindOf(MgSplines::Type());
            
            for (UInt32 i = 0; i < n; i++) {
                if (curve && i > 0 && i + 1 < n)
                    continue;
                Point2d pnt(sp->shape()->getHandlePoint(i));
                float dist = pnt.distanceTo(snapbox.center());
                if (mindist > dist) {
                    mindist = dist;
                    spfound = sp;
                    sender->pointM = pnt;
                }
            }
        }
    }
    sender->view->shapes()->freeIterator(it);
    
    if (spfound) {
        sender->point = sender->pointM * sender->view->xform()->modelToDisplay();
    }
    
    return spfound ? 1 : -1;
}
