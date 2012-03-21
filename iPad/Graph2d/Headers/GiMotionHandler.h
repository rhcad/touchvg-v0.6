// GiMotionHandler.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

struct MgShapes;
class GiTransform;
class GiGraphics;

// 图形视图协议
@protocol GiView

- (MgShapes*)getShapes;                 // 得到图形列表
- (GiTransform*)getXform;               // 得到坐标系对象
- (GiGraphics*)getGraph;                // 得到图形显示对象

- (void)setShapes:(MgShapes*)data;      // 设置图形列表
- (void)setAnimating:(BOOL)animated;    // 翻转或动画显示时通知视图
- (void)setDrawingDelegate:(id)d;       // 设置动态绘图用的控制器对象

- (void)redraw;                         // 标记视图待更新显示

@end

// 图形视图动作命令协议
@protocol GiMotionHandler
@optional

- (void)dynDraw:(GiGraphics*)gs;        // 动态显示图形
- (BOOL)cancel:(id)view;                // 取消命令
- (BOOL)undoMotion:(id)view;            // 晃动或撤销操作
- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

