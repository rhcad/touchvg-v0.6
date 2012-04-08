//! \file mgdrawlines.h
//! \brief 定义折线绘图命令类 MgCmdDrawLines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGCOMMAND_DRAW_LINES_H_
#define __GEOMETRY_MGCOMMAND_DRAW_LINES_H_

#include "mgcmddraw.h"

//! 折线绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgLines
*/
class MgCmdDrawLines : public MgCommandDraw
{
public:
    MgCmdDrawLines();
    virtual ~MgCmdDrawLines();

    static const char* Name() { return "lines"; }
    static MgCommand* Create() { return new MgCmdDrawLines; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    
private:
    bool canAddPoint(const MgMotion* sender, bool ended);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_LINES_H_
