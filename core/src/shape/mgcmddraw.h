//! \file mgcmddraw.h
//! \brief 定义绘图命令基类 MgCommandDraw 和 MgCmdBaseLines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGCOMMAND_DRAW_H_
#define __GEOMETRY_MGCOMMAND_DRAW_H_

#include <mgcmd.h>

//! 返回线条像素宽度的一半，正数
float mgLineHalfWidth(const MgShape* shape, GiGraphics* gs);

//! 绘图命令基类
/*! \ingroup GEOM_SHAPE
*/
class MgCommandDraw : public MgCommand
{
public:
    MgCommandDraw();
    virtual ~MgCommandDraw();
    
protected:
    bool _initialize(MgShape* (*creator)(), const MgMotion* sender);
    bool _touchBegan(const MgMotion* sender);
    bool _touchMoved(const MgMotion* sender);
    bool _touchEnded(const MgMotion* sender);
    bool _addshape(const MgMotion* sender);
    bool _undo(const MgMotion* sender);

    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    
private:
    UInt32 getStep() { return m_step; }

protected:
    MgShape*    m_shape;
    UInt32      m_step;
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_H_
