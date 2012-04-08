//! \file mgcmdmgr.h
//! \brief 定义命令管理器实现类 MgCmdManagerImpl
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGCOMMAND_MANAGER_H_
#define __GEOMETRY_MGCOMMAND_MANAGER_H_

#include <mgcmd.h>
#include <map>
#include <string>

//! 命令管理器实现类
/*! \ingroup GEOM_SHAPE
*/
class MgCmdManagerImpl : public MgCommandManager
{
public:
    MgCmdManagerImpl();
    virtual ~MgCmdManagerImpl();
    
private:
    virtual const char* getCommandName();
    virtual MgCommand* getCommand();
    virtual bool setCommand(const MgMotion* sender, const char* name);
    virtual bool cancel(const MgMotion* sender);
    virtual void unloadCommands();
    virtual UInt32 getSelection(MgView* view, UInt32 count, MgShape** shapes);

private:
    typedef std::map<std::string, MgCommand*> CMDS;
    CMDS            _cmds;
    std::string     _cmdname;
};

#endif // __GEOMETRY_MGCOMMAND_MANAGER_H_
