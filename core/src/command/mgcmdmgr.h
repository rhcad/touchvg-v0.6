//! \file mgcmdmgr.h
//! \brief 定义命令管理器实现类 MgCmdManagerImpl
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_MANAGER_H_
#define __GEOMETRY_MGCOMMAND_MANAGER_H_

#include <mgcmd.h>
#include <mgsnap.h>
#include <mgaction.h>
#include <map>
#include <string>

//! 命令管理器实现类
/*! \ingroup CORE_COMMAND
*/
class MgCmdManagerImpl
    : public MgCommandManager
    , public MgSnap
    , public MgActionDispatcher
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
    virtual int getSelection(MgView* view, int count, MgShape** shapes, bool forChange = false);
    virtual bool dynamicChangeEnded(MgView* view, bool apply);
    virtual MgSelection* getSelection(MgView* view);
    virtual MgActionDispatcher* getActionDispatcher();
    virtual void doContextAction(const MgMotion* sender, int action);
    virtual MgSnap* getSnap();
    
private:
    virtual bool drawSnap(const MgMotion* sender, GiGraphics* gs);
    virtual Point2d snapPoint(const MgMotion* sender, const MgShape* shape,
                              int hotHandle, int ignoreHandle = -1,
                              const int* ignoreids = NULL);
    virtual int getSnappedType();
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt);
    virtual bool getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc);
    virtual void clearSnap();
    
    virtual bool showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox);
    virtual bool showInDrawing(const MgMotion* sender, const MgShape* shape);
    virtual void doAction(const MgMotion* sender, int action);
    
private:
    void eraseWnd(const MgMotion* sender);
    MgCommand* findCommand(const char* name);

private:
    typedef std::map<std::string, MgCommand*> CMDS;
    CMDS            _cmds;
    std::string     _cmdname;
    std::string     _drawcmd;
    
    Point2d         _ptSnap;
    Point2d         _snapBase[2];
    int             _snapType[2];
    int             _snapShapeId;
    int             _snapHandle;
    int             _snapHandleSrc;
};

#endif // __GEOMETRY_MGCOMMAND_MANAGER_H_
