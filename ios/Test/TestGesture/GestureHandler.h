// ViewGesture.h
// Created by Zhang Yungui on 2012-8-27.
//

#import <UIKit/UIKit.h>

//! 手势类型
enum kGestureType {
    kGesturePinch,                  //!< 放缩
    kGestureRotate,                 //!< 旋转
    kGestureTwoFingersPan,          //!< 双指拖动
    kGestureTwoFingersTwoTaps,      //!< 双指双击
    
    kGesturePan,                    //!< 拖动
    kGestureTap,                    //!< 点击
    kGestureTwoTaps,                //!< 双击
    kGestureLongPress,              //!< 长按
    
    kGestureSwipeRight,             //!< 右扫
    kGestureSwipeLeft,              //!< 左扫
    kGestureSwipeUp,                //!< 上扫
    kGestureSwipeDown,              //!< 下扫
    kGestureMax
};

//! 手势响应者协议
@protocol UIGestureHandler <NSObject>
@optional

//! 放缩手势
- (id)twoFingersPinch:(UIPinchGestureRecognizer *)gesture;
//! 旋转手势
- (id)twoFingersRotate:(UIRotationGestureRecognizer *)gesture;
//! 双指拖动手势
- (id)twoFingersPan:(UIPanGestureRecognizer *)gesture;
//! 双指双击手势
- (id)twoFingersTwoTaps:(UIGestureRecognizer *)gesture;

//! 任何手势开始，给定触点起始屏幕坐标
- (id)gestureBegan:(UIGestureRecognizer *)gesture type:(int)gestureType point:(CGPoint)screenPt;
//! 任何手势结束
- (id)gestureEnded:(UIGestureRecognizer *)gesture type:(int)gestureType;
//! 任何手势取消
- (id)gestureCancelled:(UIGestureRecognizer *)gesture;

//! 单指拖动手势
- (id)oneFingerPan:(UIPanGestureRecognizer *)gesture;
//! 点击手势
- (id)oneFingerOneTap:(UIGestureRecognizer *)gesture;
//! 双击手势
- (id)oneFingerTwoTaps:(UIGestureRecognizer *)gesture;
//! 长按手势
- (id)longPressGesture:(UIGestureRecognizer *)gesture;

//! 左扫手势
- (id)swipeLeftGesture:(UIGestureRecognizer *)gesture;
//! 右扫手势
- (id)swipeRightGesture:(UIGestureRecognizer *)gesture;
//! 上扫手势
- (id)swipeUpGesture:(UIGestureRecognizer *)gesture;
//! 下扫手势
- (id)swipeDownGesture:(UIGestureRecognizer *)gesture;

@end
