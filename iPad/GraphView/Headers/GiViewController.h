// GiViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

// 图形视图控制器类
@interface GiViewController : UIViewController {
    id      _selector;              // 选择编辑命令
    id      _command;               // 当前绘图命令
    void*   _shapesCreated;         // 创建的图形列表
    id      _overlayView;           // 创建的浮动视图
}

- (void)clearCachedData;            // 清除缓冲数据
- (void)undoMotion;                 // 触发晃动或撤销操作
- (void)dynDraw;                    // 供图形视图动态显示时调用

// 设置当前绘图命令，必须支持 GiMotionHandler，可为Nil
- (id)setCommand:(id)cmd;

// 创建图形视图(GiGraphView)和图形列表，不需要图形视图的派生类时使用
- (UIView*)createGraphView:(CGRect)frame backgroundColor:(UIColor*)bkColor;

// 在已有窗口视图上创建浮动图形视图(GiOverlayView)和图形列表，不需要图形视图的派生类时使用
- (id)createOverlayView:(UIView*)view;

@end
