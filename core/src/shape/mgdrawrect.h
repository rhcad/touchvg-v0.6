//! \file mgdrawrect.h
//! \brief 定义矩形绘图命令类 MgCmdDrawRect
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_DRAW_RECT_H_
#define __GEOMETRY_MGCOMMAND_DRAW_RECT_H_

#include "mgcmddraw.h"

//! 矩形绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgRect
*/
class MgCmdDrawRect : public MgCommandDraw
{
protected:
    MgCmdDrawRect();
    virtual ~MgCmdDrawRect();
    
public:
    static const char* Name() { return "rect"; }
    static MgCommand* Create() { return new MgCmdDrawRect; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_RECT_H_
