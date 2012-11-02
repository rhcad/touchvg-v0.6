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
/*! example: mgRegisterCommand(YourCmd::Name(), YourCmd::Create);
    \ingroup GEOM_SHAPE
    \see mgLineHalfWidthModel, mgDisplayMmToModel, MgBaseCommand
 */
class MgCommandDraw : public MgCommand
{
protected:
    MgCommandDraw();
    virtual ~MgCommandDraw();
    
    //static const char* Name() { return "yourcmd"; }
    //static MgCommand* Create() { return new YourCmd; }
    
protected:
    bool _initialize(MgShape* (*creator)(), const MgMotion* sender);
    bool _touchBegan(const MgMotion* sender);
    bool _touchMoved(const MgMotion* sender);
    bool _touchEnded(const MgMotion* sender);
    bool _addshape(const MgMotion* sender, MgShape* shape = NULL);
    bool _undo(const MgMotion* sender);
    bool _click(const MgMotion* sender);
    void _delayClear();
    
    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender);
private:
    virtual bool isDrawingCommand() { return true; }
    
protected:
    MgShape* getCurrentShape(const MgMotion*) { return m_shape; }
    UInt32 getStep() { return m_step; }
    MgShape* dynshape() { return m_shape; }
    Point2d snapPoint(const MgMotion* sender, bool firstStep = false);
    
protected:
    UInt32      m_step;
private:
    MgShape*    m_shape;
    bool        m_needClear;
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_H_
