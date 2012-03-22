// GiViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

// 图形视图控制器类
@interface GiViewController : UIViewController {
    id      _command;                   // 绘图命令,GiCommandController
    void*   _shapesCreated;             // 创建的图形列表
    BOOL    _gestureRecognizerUsed;     // 是否使用手势识别器处理触摸消息
    
    enum { RECOGNIZER_COUNT = 5 };
    UIGestureRecognizer* _recognizers[RECOGNIZER_COUNT];
}

@property (nonatomic)   BOOL    gestureRecognizerUsed;  // 是否使用手势识别器处理触摸消息

- (void)clearCachedData;                // 清除缓冲数据，下次重新构建显示
- (void)undoMotion;                     // 触发晃动或撤销操作
- (void)dynDraw;                        // 供图形视图动态显示时调用

// 创建图形视图(GiGraphView)和图形列表，不需要图形视图的派生类时使用
- (UIView*)createGraphView:(CGRect)frame backgroundColor:(UIColor*)bkColor;

@end