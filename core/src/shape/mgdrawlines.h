//! \file mgdrawlines.h
//! \brief 定义折线绘图命令类 MgCmdDrawLines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_DRAW_LINES_H_
#define __GEOMETRY_MGCOMMAND_DRAW_LINES_H_

#include "mgcmddraw.h"

//! 自由折线绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgLines
*/
class MgCmdDrawFreeLines : public MgCommandDraw
{
protected:
    MgCmdDrawFreeLines();
    virtual ~MgCmdDrawFreeLines();
    
public:
    static const char* Name() { return "freelines"; }
    static MgCommand* Create() { return new MgCmdDrawFreeLines; }
    
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

//! 折线绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgLines
*/
class MgCmdDrawLines : public MgCommandDraw
{
protected:
    MgCmdDrawLines();
    virtual ~MgCmdDrawLines();
    
public:
    static const char* Name() { return "lines"; }
    static MgCommand* Create() { return new MgCmdDrawLines; }
    
private:
    virtual bool needCheckClosed() { return true; }
    virtual bool needEnded() { return false; }
    
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
};

//! 多边形绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgLines
*/
class MgCmdDrawPolygon : public MgCmdDrawLines
{
protected:
    MgCmdDrawPolygon(int maxEdges = 20) : _maxEdges(maxEdges) {}
    
public:
    static const char* Name() { return "polygon"; }
    static MgCommand* Create() { return new MgCmdDrawPolygon; }
    
private:
    virtual bool needCheckClosed() { return false; }
    virtual bool needEnded() { return m_step >= _maxEdges - 1; }
    
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
    
    int     _maxEdges;
};

//! 四边形绘图命令类
/*! \ingroup GEOM_SHAPE
    \see MgLines
*/
class MgCmdDrawQuadrangle : public MgCmdDrawPolygon
{
public:
    static const char* Name() { return "quadrangle"; }
    static MgCommand* Create() { return new MgCmdDrawQuadrangle; }
    
private:
    MgCmdDrawQuadrangle() : MgCmdDrawPolygon(4) {}
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
}; 

#endif // __GEOMETRY_MGCOMMAND_DRAW_LINES_H_
