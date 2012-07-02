//! \file mgcmddraw.h
//! \brief 定义绘图命令基类 MgCommandDraw 和 MgCmdBaseLines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_DRAW_H_
#define __GEOMETRY_MGCOMMAND_DRAW_H_

#include <mgcmd.h>

//! 返回线条宽度的一半，模型单位
float mgLineHalfWidthModel(const MgShape* shape, GiGraphics* gs);
float mgLineHalfWidthModel(const MgShape* shape, const MgMotion* sender);

//! 返回屏幕毫米长度对应的模型长度
float mgDisplayMmToModel(float mm, GiGraphics* gs);
float mgDisplayMmToModel(float mm, const MgMotion* sender);

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
    bool _addshape(const MgMotion* sender, MgShape* shape = NULL);
    bool _undo(const MgMotion* sender);
    void _delayClear();
    
    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    
private:
    UInt32 getStep() { return m_step; }
    
protected:
    MgShape*    m_shape;
    UInt32      m_step;
    bool        m_needClear;
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_H_
