//! \file GiCmdController.h
//! \brief 定义命令控制器类 GiCommandController
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>
#include <Graph2d/gicontxt.h>

struct MgMotion;
class MgViewProxy;

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
    CGPoint     _point;             //!< setTouchPoint
}

@property (nonatomic)   const char*     commandName;    //!< 当前命令名称
@property (nonatomic)   int             lineWidth;      //!< 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
@property (nonatomic)   int             lineStyle;      //!< 线型, 0-Solid, 1-Dash, 2-Dot, 3-DashDot, 4-DashDotdot, 5-Null
@property (nonatomic)   GiColor         lineColor;      //!< 线条颜色，clearColor 表示不画线条
@property (nonatomic)   GiColor         fillColor;      //!< 填充颜色，clearColor 表示不填充

- (id)init:(UIView**)auxview;
- (void)touchesBegan:(CGPoint)point;
- (void)setTouchPoint:(CGPoint)point view:(UIView*)view;
- (CGPoint)getPointModel;

@end
