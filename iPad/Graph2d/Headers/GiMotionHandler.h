//! \file GiMotionHandler.h
//! \brief 定义图形视图协议 GiView 和 GiMotionHandler
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

struct MgShapes;
struct MgShape;
class GiTransform;
class GiGraphics;

//! 图形视图协议
/*! \ingroup GRAPH_IOS
*/
@protocol GiView

- (MgShapes*)shapes;                    //!< 得到图形列表
- (GiTransform*)xform;                  //!< 得到坐标系对象
- (GiGraphics*)graph;                   //!< 得到图形显示对象
- (UIView*)ownerView;                   //!< 得到视图对象

- (void)setShapes:(MgShapes*)data;      //!< 设置图形列表
- (void)setAnimating:(BOOL)animated;    //!< 翻转或动画显示时通知视图
- (void)setDrawingDelegate:(id)d;       //!< 设置动态绘图用的控制器对象

- (void)shapeAdded:(MgShape*)shape;     //!< 通知已添加图形，由视图重新构建显示
- (void)regen;                          //!< 标记视图待重新构建显示
- (void)redraw;                         //!< 标记视图待更新显示
- (BOOL)isZooming;                      //!< 是否正在动态放缩或平移

@end

//! 图形视图动作命令协议
/*! \ingroup GRAPH_IOS
*/
@protocol GiMotionHandler
@optional

- (void)dynDraw:(GiGraphics*)gs;        //!< 动态显示图形
- (BOOL)cancel;                         //!< 取消命令
- (BOOL)undoMotion;                     //!< 晃动或撤销操作
- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender; //!< 双指放缩手势
- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender;     //!< 双指滑动手势
- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender;      //!< 单指滑动手势
- (BOOL)twoFingersTwoTaps:(UITapGestureRecognizer *)sender; //!< 双指双击手势
- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;  //!< 单指双击手势
- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender;   //!< 单指单击手势

@end

#ifdef __GEOMETRY_GICOLOR_H_
GiColor giFromCGColor(CGColorRef color);    // CGColor转为RGB颜色
#endif
