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
    
protected:
    Point2d     m_startPt;
};

//! 椭圆绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgEllipse
*/
class MgCmdDrawEllipse : public MgCmdDrawRect
{
public:
    static const char* Name() { return "ellipse"; }
    static MgCommand* Create() { return new MgCmdDrawEllipse; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
};

//! 菱形绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgDiamond
*/
class MgCmdDrawDiamond : public MgCmdDrawEllipse
{
public:
    static const char* Name() { return "diamond"; }
    static MgCommand* Create() { return new MgCmdDrawDiamond; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
};

//! 正方形绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgRect
*/
class MgCmdDrawSquare : public MgCmdDrawRect
{
public:
    static const char* Name() { return "square"; }
    static MgCommand* Create() { return new MgCmdDrawSquare; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
};

//! 圆绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgEllipse
*/
class MgCmdDrawCircle : public MgCmdDrawEllipse
{
public:
    static const char* Name() { return "circle"; }
    static MgCommand* Create() { return new MgCmdDrawCircle; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
};

//! 网格绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgGrid
*/
class MgCmdDrawGrid : public MgCmdDrawRect
{
public:
    static const char* Name() { return "grid"; }
    static MgCommand* Create() { return new MgCmdDrawGrid; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_RECT_H_
