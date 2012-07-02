//! \file GiZoom.h
//! \brief 定义图形视图放缩显示协议 GiZoom
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

//! 图形视图放缩回调协议
/*! \ingroup GRAPH_IOS
 */
@protocol GiZoomCallback

- (BOOL)onZoomed;

@end

//! 图形视图放缩显示协议
/*! \ingroup GRAPH_IOS
 */
@protocol GiZoom

//! 设置模型坐标系相对于世界坐标系(mm)的变换矩阵
- (void)setModelTransform:(CGAffineTransform)transform;

//! 放缩模型页面到指定的视图区域
- (void)zoomModel:(CGRect)pageRect to:(CGRect)pxRect save:(BOOL)save;

//! 平移显示
- (void)zoomPan:(CGPoint)offset;

//! 得到显示比例
- (float)getViewScale;

//! 得到视图范围的模型坐标矩形
- (CGRect)getModelRect;

//! 设置放缩回调对象
- (void)setZoomCallback:(id<GiZoomCallback>)obj;

//! 设置背景图
- (void)setBackgroundImage:(UIImage*)image;

@end
