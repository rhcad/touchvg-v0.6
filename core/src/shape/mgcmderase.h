//! \file mgcmderase.h
//! \brief 定义橡皮擦命令类 MgCmdErase
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_ERASE_H_
#define __GEOMETRY_MGCOMMAND_ERASE_H_

#include <mgcmd.h>
#include <vector>

//! 橡皮擦命令类
/*! \ingroup CORE_COMMAND
*/
class MgCmdErase : public MgBaseCommand
{
protected:
    MgCmdErase();
    virtual ~MgCmdErase();

public:
    static const char* Name() { return "erase"; }
    static MgCommand* Create() { return new MgCmdErase; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);

private:
    int getStep() { return 0; }
    MgShape* hitTest(const MgMotion* sender);
    bool isIntersectMode(const MgMotion* sender);
    
    std::vector<int>     m_delIds;
    bool                    m_boxsel;
};

#endif // __GEOMETRY_MGCOMMAND_ERASE_H_
