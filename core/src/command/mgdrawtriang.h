//! \file mgdrawtriang.h
//! \brief 定义三角形绘图命令
//! \author pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_DRAW_TRIANGLE_H_
#define __GEOMETRY_MGCOMMAND_DRAW_TRIANGLE_H_

#include "mgcmddraw.h"

//! 三角形绘图命令
/*! \ingroup CORE_COMMAND
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
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

//! 三点圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
*/
class MgCmdArc3P : public MgCommandDraw
{
protected:
    MgCmdArc3P() {}
public:
    static const char* Name() { return "arc3p"; }
    static MgCommand* Create() { return new MgCmdArc3P; }
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender) { return _touchBegan2(sender); }
    virtual bool touchMoved(const MgMotion* sender) { return _touchMoved2(sender); }
    virtual bool touchEnded(const MgMotion* sender) { return _touchEnded2(sender); }
    virtual void setStepPoint(int step, const Point2d& pt);

protected:
    Point2d _points[3];
};

//! 圆心+起点+终点圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
*/
class MgCmdArcCSE : public MgCmdArc3P
{
protected:
    MgCmdArcCSE() {}
public:
    static const char* Name() { return "arc-cse"; }
    static MgCommand* Create() { return new MgCmdArcCSE; }
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual void setStepPoint(int step, const Point2d& pt);
};

//! 切线圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
*/
class MgCmdArcTan : public MgCmdArc3P
{
protected:
    MgCmdArcTan() {}
public:
    static const char* Name() { return "arc-tan"; }
    static MgCommand* Create() { return new MgCmdArcTan; }
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual void setStepPoint(int step, const Point2d& pt);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_TRIANGLE_H_
