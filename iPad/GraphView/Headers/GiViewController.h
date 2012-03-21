// GiViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

@class GiSelectController;

// 图形视图的控制器类
@interface GiViewController : UIViewController {
    GiSelectController* _selector;  // 选择编辑命令
    id      _command;               // 当前绘图命令，必须支持 GiMotionHandler
}

- (void)clearCachedData;            // 清除缓冲数据
- (void)undoMotion;                 // 触发晃动或撤销操作
- (void)dynDraw;                    // 供图形视图动态显示时调用

// 设置当前绘图命令，必须支持 GiMotionHandler，可为Nil
- (id)setCommand:(id)cmd;

@end
