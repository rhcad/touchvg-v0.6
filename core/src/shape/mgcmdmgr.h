// mgcmdmgr.h: 定义命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_MANAGER_H_
#define __GEOMETRY_MGCOMMAND_MANAGER_H_

#include <mgcmd.h>

class MgCmdManagerImpl : public MgCommandManager
{
public:
    MgCmdManagerImpl();
    virtual ~MgCmdManagerImpl();
    
private:
    virtual UInt32 getCommandID();
    virtual MgCommand* getCommand();
    virtual bool setCommandID(UInt32 cmdID);
};

#endif // __GEOMETRY_MGCOMMAND_MANAGER_H_
