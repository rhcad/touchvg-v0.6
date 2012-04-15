//! \file GiGraphView.h
//! \brief 定义图形视图类 GiGraphView
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchdraw

#import "GiMotionHandler.h"

class GiGraphIos;

//! 图形视图类
/*! \ingroup GRAPH_IOS
*/
@interface GiGraphView : UIView<GiView, GiMotionHandler> {
@protected
    MgShapes*       _shapes;                //!< 图形列表
    GiTransform*    _xform;                 //!< 坐标系对象
    GiGraphics*     _graph;                 //!< 图形显示对象
    GiGraphIos*     _adapter;               //!< 显示适配对象
    id              _drawingDelegate;       //!< 动态绘图用的委托控制器对象
    
    CGPoint         _firstPoint;            //!< 动态放缩用的开始点
    CGPoint         _lastPoint;             //!< 动态放缩用的上次点
    
    BOOL            _enableZoom;            //!< 是否允许放缩或平移
    BOOL            _zooming;               //!< 是否正在动态放缩或平移
    double          _lastViewScale;         //!< 动态放缩前的显示比例
    CGPoint         _lastCenterW;           //!< 动态放缩前的视图中心世界坐标
    
    BOOL            _doubleZoomed;          //!< 是否为局部放大状态
    double          _scaleBeforeDbl;        //!< 局部放大前的显示比例
    CGPoint         _centerBeforeDbl;       //!< 局部放大前的视图中心世界坐标
}

@property (nonatomic)          MgShapes*    shapes;     //!< 图形列表
@property (nonatomic,readonly) GiTransform* xform;      //!< 坐标系对象
@property (nonatomic,readonly) GiGraphics*  graph;      //!< 图形显示对象
@property (nonatomic)          BOOL         enableZoom; //!< 是否允许放缩或平移
@property (nonatomic,readonly) BOOL         zooming;    //!< 是否正在动态放缩或平移

- (void)afterCreated;                                   //!< 视图窗口后内部调用
- (void)draw:(GiGraphics*)gs;                           //!< 显示全部图形内部调用

@end

//! 放大镜视图类
/*! \ingroup GRAPH_IOS
 */
@interface GiMagnifierView : UIView<GiView> {
@protected
    GiTransform*    _xform;                 //!< 坐标系对象
    GiGraphics*     _graph;                 //!< 图形显示对象
    GiGraphIos*     _adapter;               //!< 显示适配对象
    id<GiView>      _gview;                 //!< 实际图形视图
    UIResponder*    _drawingDelegate;       //!< 动态绘图用的委托控制器对象
    CGPoint         _pointW;                //!< 实际图形视图的当前点，世界坐标
    CGFloat         _scale;                 //!< 放大倍数
    BOOL            _lockRedraw;            //!< 禁止放大镜动态显示
}

@property (nonatomic,readonly) GiGraphics*  graph;      //!< 图形显示对象
@property (nonatomic)          CGPoint      pointW;     //!< 实际图形视图的当前点，世界坐标
@property (nonatomic)          CGFloat      scale;      //!< 放大倍数
@property (nonatomic)          BOOL         lockRedraw; //!< 禁止放大镜动态显示

//! 给定视图外框和实际图形视图初始化本视图对象
- (id)initWithFrame:(CGRect)frame graphView:(id<GiView>)gview;

//! 在本视图内平移显示
- (void)zoomPan:(CGPoint)ranslation;

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender;

//! 设置实际图形视图的当前点（世界坐标）并刷新显示
- (void)setPointWandRedraw:(CGPoint)ptw;

//! 根据实际图形视图中的当前位置自动移动上级视图，以便当前位置不被本视图遮挡
- (BOOL)automoveSuperview:(CGPoint)point fromView:(UIView*)view;

- (void)draw:(GiGraphics*)gs;                           //!< 显示全部图形内部调用
- (void)dynDraw:(GiGraphics*)gs;                        //!< 动态显示时内部调用
- (BOOL)isActiveView;                                   //!< 返回本视图是否为当前交互视图

@end
