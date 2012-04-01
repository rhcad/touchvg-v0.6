//! \file GiGraphView.h
//! \brief 定义图形视图类 GiGraphView
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>

//! 图形视图类
/*! \ingroup _GRAPH_IOS_
*/
@interface GiGraphView : UIView<GiView, GiMotionHandler> {
@protected
    MgShapes*       _shapes;                //!< 图形列表
    GiTransform*    _xform;                 //!< 坐标系对象
    GiGraphics*     _graph;                 //!< 图形显示对象
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
/*! \ingroup _GRAPH_IOS_
 */
@interface GiMagnifierView : UIView<GiView> {
    GiTransform*    _xform;                 //!< 坐标系对象
    GiGraphics*     _graph;                 //!< 图形显示对象
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

- (id)initWithFrame:(CGRect)frame graphView:(id<GiView>)gview;
- (void)zoomPan:(CGPoint)ranslation;
- (void)setPointWandRedraw:(CGPoint)pt;

- (void)draw:(GiGraphics*)gs;
- (void)dynDraw:(GiGraphics*)gs;
- (BOOL)isActiveView;

@end
