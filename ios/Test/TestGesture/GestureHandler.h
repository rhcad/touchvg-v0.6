// ViewGesture.h
// Created by Zhang Yungui on 2012-8-27.
//

#import <UIKit/UIKit.h>

//! 手势类型
typedef enum {
    kGesturePinch,                  //!< 双指放缩
    kGestureRotate,                 //!< 双指旋转
    kGestureTwoPan,                 //!< 双指拖动
    kGestureTwoTap,                 //!< 双指点击
    kGestureTwoDblTaps,             //!< 双指双击
    
    kGesturePan,                    //!< 单指拖动
    kGestureTap,                    //!< 单指点击
    kGestureDblTaps,                //!< 单指双击
    kGestureLongPress,              //!< 单指长按
    
    kGestureSwipeRight,             //!< 右扫
    kGestureSwipeLeft,              //!< 左扫
    kGestureSwipeUp,                //!< 上扫
    kGestureSwipeDown,              //!< 下扫
    kGestureMax
} kGestureType;

//! 手势类型掩码位
typedef enum {
    kGestureBitPinch        = 1 << kGesturePinch,       //!< 双指放缩
    kGestureBitRotate       = 1 << kGestureRotate,      //!< 双指旋转
    kGestureBitTwoPan       = 1 << kGestureTwoPan,      //!< 双指拖动
    kGestureBitTwoTap       = 1 << kGestureTwoTap,      //!< 双指点击
    kGestureBitTwoDblTaps   = 1 << kGestureTwoDblTaps,  //!< 双指双击
    
    kGestureBitPan          = 1 << kGesturePan,         //!< 单指拖动
    kGestureBitTap          = 1 << kGestureTap,         //!< 单指点击
    kGestureBitDblTaps      = 1 << kGestureDblTaps,     //!< 单指双击
    kGestureBitLongPress    = 1 << kGestureLongPress,   //!< 单指长按
    
    kGestureBitSwipeRight   = 1 << kGestureSwipeRight,  //!< 右扫
    kGestureBitSwipeLeft    = 1 << kGestureSwipeLeft,   //!< 左扫
    kGestureBitSwipeUp      = 1 << kGestureSwipeUp,     //!< 上扫
    kGestureBitSwipeDown    = 1 << kGestureSwipeDown,   //!< 下扫
} kGestureBitType;

//! 手势响应者协议
@protocol UIGestureHandler
@optional

//! 任何手势开始，可记忆坐标
- (BOOL)gestureBegan:(UITouch *)touch;
//! 检查手势能否开始
- (BOOL)gestureShouldBegin:(UIGestureRecognizer *)gesture :(SEL)action;

//! 响应放缩手势
- (BOOL)gesturePinch:(UIPinchGestureRecognizer *)gesture;
//! 响应旋转手势
- (BOOL)gestureRotate:(UIRotationGestureRecognizer *)gesture;
//! 响应双指拖动手势
- (BOOL)gestureTwoPan:(UIPanGestureRecognizer *)gesture;
//! 响应双指点击手势
- (BOOL)gestureTwoTap:(UIGestureRecognizer *)gesture;
//! 响应双指双击手势
- (BOOL)gestureTwoDblTaps:(UIGestureRecognizer *)gesture;

//! 响应单指拖动手势
- (BOOL)gesturePan:(UIPanGestureRecognizer *)gesture;
//! 响应点击手势
- (BOOL)gestureTap:(UIGestureRecognizer *)gesture;
//! 响应双击手势
- (BOOL)gestureDblTaps:(UIGestureRecognizer *)gesture;
//! 响应长按手势
- (BOOL)gestureLongPress:(UIGestureRecognizer *)gesture;

//! 响应左扫手势
- (BOOL)gestureSwipeLeft:(UIGestureRecognizer *)gesture;
//! 响应右扫手势
- (BOOL)gestureSwipeRight:(UIGestureRecognizer *)gesture;
//! 响应上扫手势
- (BOOL)gestureSwipeUp:(UIGestureRecognizer *)gesture;
//! 响应下扫手势
- (BOOL)gestureSwipeDown:(UIGestureRecognizer *)gesture;

@end
