//! \file GiCmdController.h
//! \brief 定义命令控制器类 GiCommandController
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiMotionHandler.h"
#include <mgcmd.h>

struct MgMotion;
class MgViewProxyIos;

//! 命令控制器类，代理调用内部命令(MgCommand)
/*! \ingroup GRAPH_IOS
*/
@interface GiCommandController : NSObject<GiMotionHandler> {
@private
    MgMotion    *_motion;           //!< 当前命令参数
    MgViewProxyIos *_mgview;        //!< 命令所用视图
    BOOL        _moved;             //!< 是否已触发touchBegan命令消息
    int         _clickFingers;      //!< 是否已触发单指点击或双击事件
    BOOL        _undoFired;         //!< 是否已触发Undo操作
    int         _touchCount;        //!< 开始触摸时的手指数
    BOOL        _twoFingersHandled; //!< 选择状态下双指触摸模式
}

@property (nonatomic,readonly)  MgMotion* motion;       //!< 当前命令参数
@property (nonatomic,assign)  NSObject* editDelegate;   //!< 编辑代理,GiEditAction
@property (nonatomic)   const char*     commandName;    //!< 当前命令名称
@property (nonatomic)   BOOL            currentShapeFixedLength;    //!< 当前选中图形是否固定边长

//! 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
@property (nonatomic)   float           lineWidth;
//! 线型, 0-Solid, 1-Dash, 2-Dot, 3-DashDot, 4-DashDotdot, 5-Null
@property (nonatomic)   int             lineStyle;

@property (nonatomic)   GiColor         lineColor;      //!< 线条颜色，clearColor 表示不画线条
@property (nonatomic)   GiColor         fillColor;      //!< 填充颜色，clearColor 表示不填充
@property (nonatomic)   BOOL            autoFillColor;  //!< 填充颜色随线条颜色自动变化

@property (nonatomic,readonly)   BOOL   dragging;       //!< 返回是否正拖动

//! 给定辅助视图初始化本对象，auxviews是以Nil结束的多个视图
- (id)initWithViews:(UIView**)auxviews;

//! 开始触摸时调用，避免Pan手势开始时丢失开始触摸位置
- (void)touchesBegan:(CGPoint)point view:(UIView*)view count:(int)count;

//! 正在触摸移动时调用，视图控制器在手势识别失败时调用
- (BOOL)touchesMoved:(CGPoint)point view:(UIView*)view count:(int)count;

//! 触摸移动结束时调用，视图控制器在手势识别失败时调用
- (BOOL)touchesEnded:(CGPoint)point view:(UIView*)view count:(int)count;

//! 延迟检测点击事件
- (BOOL)delayTap:(CGPoint)point view:(UIView*)view;

//! 返回当前点，世界坐标
- (CGPoint)getPointW;

//! 退出动态修改模式，应用修改结果
- (BOOL)dynamicChangeEnded:(BOOL)apply;

//! 得到临时动态图形
- (void)getDynamicShapes:(MgShapes*)shapes;

//! 判断是否新重绘过，可用于判断是否需要调用 getDynamicShapes
- (BOOL)isDynamicChanged:(BOOL)reset;

//! 隐藏上下文菜单
+ (void)hideContextActions;

//! 执行默认的上下文动作
- (void)doContextAction:(int)action;

@end
