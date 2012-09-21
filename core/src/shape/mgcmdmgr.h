//! \file mgcmdmgr.h
//! \brief 定义命令管理器实现类 MgCmdManagerImpl
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_MANAGER_H_
#define __GEOMETRY_MGCOMMAND_MANAGER_H_

#include <mgcmd.h>
#include <mgsnap.h>
#include <map>
#include <string>

//! 命令管理器实现类
/*! \ingroup GEOM_SHAPE
*/
class MgCmdManagerImpl : public MgCommandManager, public MgSnap
{
public:
    MgCmdManagerImpl(bool tmpobj = false);
    virtual ~MgCmdManagerImpl();
    
private:
    virtual const char* getCommandName();
    virtual MgCommand* getCommand();
    virtual bool setCommand(const MgMotion* sender, const char* name);
    virtual bool cancel(const MgMotion* sender);
    virtual void unloadCommands();
    virtual UInt32 getSelection(MgView* view, UInt32 count, MgShape** shapes, bool forChange = false);
    virtual bool dynamicChangeEnded(MgView* view, bool apply);
    virtual MgSelection* getSelection(MgView* view);
    virtual MgSnap* getSnap();
    
private:
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual Point2d snapPoint(const MgMotion* sender, MgShape* hotShape, int hotHandle);
    virtual int getSnappedType();
    virtual void clearSnap();

private:
    typedef std::map<std::string, MgCommand*> CMDS;
    CMDS            _cmds;
    std::string     _cmdname;
    
    Point2d         _ptSnap;
    Point2d         _snapBase[2];
    int             _snapType[2];
};

#endif // __GEOMETRY_MGCOMMAND_MANAGER_H_
