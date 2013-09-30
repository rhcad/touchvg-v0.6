//! \file mgcmddraw.h
//! \brief 定义绘图命令基类 MgCommandDraw 和 MgCmdBaseLines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_H_
#define TOUCHVG_CMD_DRAW_H_

#include "mgcmd.h"

//! 绘图命令基类
/*! \ingroup CORE_COMMAND
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
    MgShape* _addshape(const MgMotion* sender, MgShape* shape = NULL, bool autolock = true);
    bool _undo(const MgMotion* sender);
    bool _draw(const MgMotion* sender, GiGraphics* gs);
    bool _click(const MgMotion* sender);
    void _delayClear();

    bool _touchBeganStep(const MgMotion* sender);
    bool _touchMovedStep(const MgMotion* sender);
    bool _touchEndedStep(const MgMotion* sender);
    virtual int getMaxStep() { return 3; }
    virtual void setStepPoint(int step, const Point2d& pt);
    
    virtual bool undo(const MgMotion* sender) { return _undo(sender); }
    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) { return _draw(sender, gs); }
    virtual int gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender);
private:
    virtual bool isDrawingCommand() { return true; }
    
public:
    MgShape* getCurrentShape(const MgMotion*) { return m_shape; }
    int getStep() { return m_needClear ? 0 : m_step; }
    MgShape* dynshape() { return m_shape; }
    Point2d snapPoint(const MgMotion* sender, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, bool firstStep = false);
    
protected:
    int         m_step;
private:
    MgShape*    m_shape;
    bool        m_needClear;
};

#endif // TOUCHVG_CMD_DRAW_H_
