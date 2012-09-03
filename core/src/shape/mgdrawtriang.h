//! \file mgdrawtriang.h
//! \brief 定义三角形绘图命令
//! \author pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_DRAW_TRIANGLE_H_
#define __GEOMETRY_MGCOMMAND_DRAW_TRIANGLE_H_

#include "mgcmddraw.h"

//! 三角形绘图命令
/*! \ingroup GEOM_SHAPE
*/
class MgCmdDrawTriangle : public MgCommandDraw
{
protected:
    MgCmdDrawTriangle();
    virtual ~MgCmdDrawTriangle();
    
public:
    static const char* Name() { return "triangle"; }
    static MgCommand* Create() { return new MgCmdDrawTriangle; }

private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool click(const MgMotion* sender);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_TRIANGLE_H_
