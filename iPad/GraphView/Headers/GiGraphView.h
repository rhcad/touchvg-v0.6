// GiGraphView.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>

// 独占显示的图形视图类
@interface GiGraphView : UIView<GiView, GiMotionHandler> {
    MgShapes*       _shapes;                // 图形列表
    GiTransform*    _xform;                 // 坐标系对象
    GiGraphics*     _graph;                 // 图形显示对象
    id              _drawingDelegate;       // 动态绘图用的委托控制器对象
    
    CGPoint         _firstPoint;            // 动态放缩用的开始点
    CGPoint         _lastPoint;             // 动态放缩用的上次点
    
    BOOL            _zooming;               // 是否正在动态放缩或平移
    double          _lastViewScale;         // 动态放缩前的显示比例
    CGPoint         _lastCenterW;           // 动态放缩前的视图中心世界坐标
    
    BOOL            _doubleZoomed;          // 是否为局部放大状态
    double          _scaleBeforeDbl;        // 局部放大前的显示比例
    CGPoint         _centerBeforeDbl;       // 局部放大前的视图中心世界坐标
}

@property (nonatomic)          MgShapes*    shapes;     // 图形列表
@property (nonatomic,readonly) GiTransform* xform;      // 坐标系对象
@property (nonatomic,readonly) GiGraphics*  graph;      // 图形显示对象
@property (nonatomic,readonly) BOOL         zooming;    // 是否正在动态放缩或平移

- (void)afterCreated;
- (void)draw:(GiGraphics*)gs;

@end

// 附加在其他窗口视图上的图形视图类
@interface GiOverlayView : NSObject<GiView> {
    UIView*         _view;                  // 宿主视图
    MgShapes*       _shapes;                // 图形列表
    GiTransform*    _xform;                 // 坐标系对象
    GiGraphics*     _graph;                 // 图形显示对象
    id              _drawingDelegate;       // 动态绘图用的委托控制器对象
}

@property (nonatomic)          MgShapes*    shapes;     // 图形列表
@property (nonatomic,readonly) GiTransform* xform;      // 坐标系对象
@property (nonatomic,readonly) GiGraphics*  graph;      // 图形显示对象

- (id)initWithView:(UIView*)view;           // 给定已有窗口视图初始化本对象
- (void)drawOnView:(CGContextRef)context;   // 显示图形
- (void)draw:(GiGraphics*)gs;               // 显示图形，由drawOnView调用

@end
