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

- (id)twoFingersPinch:(UIPinchGestureRecognizer *)gesture;
- (id)twoFingersRotate:(UIRotationGestureRecognizer *)gesture;
- (id)twoFingersPan:(UIPanGestureRecognizer *)gesture;
- (id)twoFingersTwoTaps:(UIGestureRecognizer *)gesture;

- (id)oneFingerPan:(UIPanGestureRecognizer *)gesture;
- (id)oneFingerOneTap:(UIGestureRecognizer *)gesture;
- (id)oneFingerTwoTaps:(UIGestureRecognizer *)gesture;
- (id)longPressGesture:(UIGestureRecognizer *)gesture;

- (id)swipeLeftGesture:(UIGestureRecognizer *)gesture;
- (id)swipeRightGesture:(UIGestureRecognizer *)gesture;
- (id)swipeUpGesture:(UIGestureRecognizer *)gesture;
- (id)swipeDownGesture:(UIGestureRecognizer *)gesture;

@end
