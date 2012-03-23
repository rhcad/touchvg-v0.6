// mgcmdmgr.h: 定义命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_MANAGER_H_
#define __GEOMETRY_MGCOMMAND_MANAGER_H_

#include <mgcmd.h>
#include <map>
#include <string>

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

private:
    typedef std::map<std::string, MgCommand*> CMDS;
    CMDS            _cmds;
    std::string     _cmdname;
};

#endif // __GEOMETRY_MGCOMMAND_MANAGER_H_
