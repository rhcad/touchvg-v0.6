//! \file mgcmd.h
//! \brief 定义绘图命令接口
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_H_
#define __GEOMETRY_MGCOMMAND_H_

#include <gigraph.h>
#include <MgShapes.h>

//! 图形视图接口
struct MgView {
    virtual MgShapes* shapes() = 0;             //!< 得到图形列表
    virtual GiTransform* xform() = 0;           //!<  得到坐标系对象
    virtual GiGraphics* graph() = 0;            //!<  得到图形显示对象
    virtual void regen() = 0;                   //!<  标记视图待重新构建显示
    virtual void redraw() = 0;                  //!<  标记视图待更新显示
};

//! 命令参数
struct MgMotion {
    MgView*     view;                           //!< 图形视图
    POINT       startPoint;                     //!< 开始点，视图坐标
    Point2d     startPointM;                    //!< 开始点，模型坐标
    POINT       lastPoint;                      //!< 上次点，视图坐标
    Point2d     lastPointM;                     //!< 上次点，模型坐标
    POINT       point;                          //!< 当前点，视图坐标
    Point2d     pointM;                         //!< 当前点，模型坐标
};

//! 命令接口
struct MgCommand {
    virtual const char* getName() const = 0;
    virtual void release() = 0;
    virtual bool cancel(const MgMotion* sender) = 0;
    virtual bool initialize(const MgMotion* sender) = 0;
    virtual bool undo(const MgMotion* sender) = 0;
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) = 0;
    virtual bool click(const MgMotion* sender) = 0;
    virtual bool doubleClick(const MgMotion* sender) = 0;
    virtual bool longPress(const MgMotion* sender) = 0;
    virtual bool touchesBegan(const MgMotion* sender) = 0;
    virtual bool touchesMoved(const MgMotion* sender) = 0;
    virtual bool touchesEnded(const MgMotion* sender) = 0;
};

//! 命令管理器接口
struct MgCommandManager {
    virtual const char* getCommandName() = 0;
    virtual MgCommand* getCommand() = 0;
    virtual bool setCommand(const MgMotion* sender, const char* name) = 0;
    virtual void cancel(const MgMotion* sender) = 0;
    virtual void unloadCommands() = 0;
};

//! 返回命令管理器
MgCommandManager* mgGetCommandManager();

#endif // __GEOMETRY_MGCOMMAND_H_
