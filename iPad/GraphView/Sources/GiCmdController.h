//! \file GiCmdController.h
//! \brief 定义命令控制器类 GiCommandController
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>

struct MgMotion;
class MgViewProxy;
class GiContext;

//! 命令控制器类，代理调用内部命令(MgCommand)
/*! \ingroup _GRAPH_IOS_
*/
@interface GiCommandController : NSObject<GiMotionHandler> {
@private
    MgMotion    *_motion;           //!< 当前命令参数
    MgViewProxy *_mgview;           //!< 命令所用视图
    GiContext   *_context;          //!< 当前绘图属性
    BOOL        _undoFired;         //!< 是否已向命令触发Undo消息(滑动中变为双指)
    CGPoint     _downPoint;         //!< 开始触摸时的位置
}

@property (nonatomic,readonly)  GiContext*      context;        //!< 当前绘图属性
@property (nonatomic)           const char*     commandName;    //!< 当前命令名称

- (void)touchesBegan:(NSSet *)touches;

@end
