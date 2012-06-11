//! \file mgdrawline.h
//! \brief 定义线段绘图命令类 MgCmdDrawLine
//! \author pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_DRAW_LINE_H_
#define __GEOMETRY_MGCOMMAND_DRAW_LINE_H_

#include "mgcmddraw.h"

//! 线段绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgLine
*/
class MgCmdDrawLine : public MgCommandDraw
{
public:
    MgCmdDrawLine();
    virtual ~MgCmdDrawLine();

    static const char* Name() { return "line"; }
    static MgCommand* Create() { return new MgCmdDrawLine; }

private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_LINE_H_