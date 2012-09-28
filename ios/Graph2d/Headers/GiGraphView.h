//! \file GiGraphView.h
//! \brief 定义图形视图类 GiGraphView
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiMotionHandler.h"
#import "GiZoom.h"

class GiGraphIos;

//! 图形视图类
/*! \ingroup GRAPH_IOS
*/
@interface GiGraphView : UIView<GiView, GiMotionHandler, GiZoom> {
@protected
    MgShapes*       _shapes;                //!< 图形列表
    MgShapes*       _playShapes;            //!< 临时播放的图形列表
    GiGraphIos*     _graph;                 //!< 图形显示对象
    id              _drawingDelegate;       //!< 动态绘图用的委托控制器对象
    MgShape*        _shapeAdded;            //!< 待添加显示的图形
    int             _buffered;              //!< 刷新显示时是否使用缓冲图
    BOOL            _scaleReaded;           //!< 是否已从图形列表取出放缩状态
    id<GiZoomCallback>  _zoomCallback;      //!< 外部的放缩回调对象
    UIImage*        _bkImg;                 //!< 背景图
    
    CGPoint         _firstPoint;            //!< 动态放缩用的开始点    
    BOOL            _enableZoom;            //!< 是否允许放缩或平移
    BOOL            _zooming;               //!< 是否正在动态放缩或平移
    float           _lastViewScale;         //!< 动态放缩前的显示比例
    CGPoint         _lastCenterW;           //!< 动态放缩前的视图中心世界坐标
    float           _lastDistPan;           //!< 上次的平移双指距离，用于放缩
    float           _lastScalePan;          //!< 上次的平移双指放大倍数
    
    BOOL            _doubleZoomed;          //!< 是否为局部放大状态
    float           _scaleBeforeDbl;        //!< 局部放大前的显示比例
    CGPoint         _centerBeforeDbl;       //!< 局部放大前的视图中心世界坐标
    float           _initialScale;          //!< 初始显示放缩比例
}

@property (nonatomic)          BOOL         enableZoom; //!< 是否允许放缩或平移
@property (nonatomic,readonly) BOOL         zooming;    //!< 是否正在动态放缩或平移
@property (nonatomic,readonly) MgShape*     shapeAdded; //!< 待添加显示的图形
@property (nonatomic)          BOOL         bufferEnabled;  //!< 是否允许缓冲绘图
@property (nonatomic,readonly) float        initialScale;   //!< 初始显示放缩比例

- (CGImageRef)cachedBitmap:(BOOL)invert;    //!< 当前缓存位图，上下翻转时由调用者释放
- (MgShapes*)getPlayShapes:(BOOL)clear;     //!< 设置临时播放的图形列表

- (void)afterCreated;                       //!< 视图窗口后内部调用
- (BOOL)draw:(GiGraphics*)gs;               //!< 显示全部图形内部调用

@end

//! 放大镜视图类
/*! \ingroup GRAPH_IOS
 */
@interface GiMagnifierView : UIView<GiView> {
@protected
    GiGraphIos*     _graph;                 //!< 图形显示对象
    id<GiView>      _gview;                 //!< 实际图形视图
    UIResponder*    _drawingDelegate;       //!< 动态绘图用的委托控制器对象
    MgShape*        _shapeAdded;            //!< 待添加显示的图形
    BOOL            _cachedDraw;            //!< 刷新显示时是否使用缓冲图
    CGPoint         _pointW;                //!< 实际图形视图的当前点，世界坐标
    CGFloat         _scale;                 //!< 放大倍数
    BOOL            _lockRedraw;            //!< 禁止放大镜动态显示
    BOOL            _zooming;               //!< 是否正在动态放缩或平移
    CGPoint         _lastPt;                //!< 上次双指位置
    CGPoint         _zoomCenter;
}

@property (nonatomic)          CGPoint      pointW;     //!< 实际图形视图的当前点，世界坐标
@property (nonatomic)          CGFloat      scale;      //!< 放大倍数
@property (nonatomic)          BOOL         lockRedraw; //!< 禁止放大镜动态显示

//! 给定视图外框和实际图形视图初始化本视图对象
- (id)initWithFrame:(CGRect)frame graphView:(id<GiView>)gview;

//! 在本视图内平移显示
- (void)zoomPan:(CGPoint)ranslation;

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender;

//! 设置实际图形视图的当前点（世界坐标）并刷新显示
- (void)setPointWandRedraw:(CGPoint)ptw :(BOOL)fromClick;

//! 根据实际图形视图中的当前位置自动移动上级视图，以便当前位置不被本视图遮挡
- (BOOL)automoveSuperview:(CGPoint)point fromView:(UIView*)view;

@end
