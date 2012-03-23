// mgcmdselect.h: 定义选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_SELECT_H_
#define __GEOMETRY_MGCOMMAND_SELECT_H_

#include <mgcmd.h>

class MgCommandSelect : public MgCommand
{
public:
    MgCommandSelect();
    virtual ~MgCommandSelect();

    static const char* Name() { return "select"; }
    static MgCommand* Create() { return new MgCommandSelect; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchesBegan(const MgMotion* sender);
    virtual bool touchesMoved(const MgMotion* sender);
    virtual bool touchesEnded(const MgMotion* sender);
};

#endif // __GEOMETRY_MGCOMMAND_SELECT_H_
