//! \file mgcmd.h
//! \brief 定义绘图命令接口
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGCOMMAND_H_
#define __GEOMETRY_MGCOMMAND_H_

#include <gigraph.h>
#include <mgshapes.h>

//! 图形视图接口
/*! \ingroup GEOM_SHAPE
    \interface MgView
*/
struct MgView {
    virtual MgShapes* shapes() = 0;             //!< 得到图形列表
    virtual GiTransform* xform() = 0;           //!< 得到坐标系对象
    virtual GiGraphics* graph() = 0;            //!< 得到图形显示对象
    virtual void regen() = 0;                   //!< 标记视图待重新构建显示
    virtual void redraw() = 0;                  //!< 标记视图待更新显示
    virtual GiContext* context() = 0;           //!< 得到当前绘图属性
    virtual bool shapeWillAdded(MgShape* shape) = 0;    //!< 通知将添加图形
    virtual bool shapeWillDeleted(MgShape* shape) = 0;  //!< 通知将删除图形
};

//! 命令参数
/*! \ingroup GEOM_SHAPE
*/
struct MgMotion {
    MgView*     view;                           //!< 图形视图
    Point2d     startPoint;                     //!< 开始点，视图坐标
    Point2d     startPointM;                    //!< 开始点，模型坐标
    Point2d     lastPoint;                      //!< 上次点，视图坐标
    Point2d     lastPointM;                     //!< 上次点，模型坐标
    Point2d     point;                          //!< 当前点，视图坐标
    Point2d     pointM;                         //!< 当前点，模型坐标
};

//! 命令接口
/*! \ingroup GEOM_SHAPE
    \interface MgCommand
*/
struct MgCommand {
    virtual const char* getName() const = 0;            //!< 返回命令名称
    virtual void release() = 0;                         //!< 销毁对象
    virtual bool cancel(const MgMotion* sender) = 0;    //!< 取消命令
    virtual bool initialize(const MgMotion* sender) = 0;    //!< 开始命令
    virtual bool undo(bool &enableRecall, const MgMotion* sender) = 0;  //!< 回退一步
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) = 0;  //!< 显示动态图形
    virtual bool click(const MgMotion* sender) = 0;         //!< 点击
    virtual bool doubleClick(const MgMotion* sender) = 0;   //!< 双击
    virtual bool longPress(const MgMotion* sender) = 0;     //!< 长按
    virtual bool touchBegan(const MgMotion* sender) = 0;    //!< 开始滑动
    virtual bool touchMoved(const MgMotion* sender) = 0;    //!< 正在滑动
    virtual bool touchEnded(const MgMotion* sender) = 0;    //!< 滑动结束
};

//! 命令管理器接口
/*! \ingroup GEOM_SHAPE
    \interface MgCommandManager
    \see mgGetCommandManager
*/
struct MgCommandManager {
    virtual const char* getCommandName() = 0;
    virtual MgCommand* getCommand() = 0;
    virtual bool setCommand(const MgMotion* sender, const char* name) = 0;
    virtual bool cancel(const MgMotion* sender) = 0;
    virtual void unloadCommands() = 0;
    virtual UInt32 getSelection(MgView* view, UInt32 count, MgShape** shapes) = 0;
};

//! 返回命令管理器
/*! \ingroup GEOM_SHAPE
*/
MgCommandManager* mgGetCommandManager();

#endif // __GEOMETRY_MGCOMMAND_H_
