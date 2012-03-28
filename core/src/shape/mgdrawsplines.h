//! \file mgdrawsplines.h
//! \brief 定义样条曲线绘图命令类 MgCmdDrawSplines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_DRAW_SPLINES_H_
#define __GEOMETRY_MGCOMMAND_DRAW_SPLINES_H_

#include "mgcmddraw.h"

//! 样条曲线绘图命令类
/*! \ingroup _GEOM_SHAPE_
*/
class MgCmdDrawSplines : public MgCommandDraw
{
public:
    MgCmdDrawSplines();
    virtual ~MgCmdDrawSplines();

    static const char* Name() { return "splines"; }
    static MgCommand* Create() { return new MgCmdDrawSplines; }
    
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

#endif // __GEOMETRY_MGCOMMAND_DRAW_SPLINES_H_
