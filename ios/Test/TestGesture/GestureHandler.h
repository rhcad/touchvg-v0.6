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

- (id)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (id)twoFingersRotate:(UIRotationGestureRecognizer *)sender;
- (id)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (id)twoFingersTwoTaps:(UIGestureRecognizer *)sender;

- (id)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (id)oneFingerOneTap:(UIGestureRecognizer *)sender;
- (id)oneFingerTwoTaps:(UIGestureRecognizer *)sender;
- (id)longPressGesture:(UIGestureRecognizer *)sender;

- (id)swipeLeftGesture:(UIGestureRecognizer *)sender;
- (id)swipeRightGesture:(UIGestureRecognizer *)sender;
- (id)swipeUpGesture:(UIGestureRecognizer *)sender;
- (id)swipeDownGesture:(UIGestureRecognizer *)sender;

@end
