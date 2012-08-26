// ViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
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

@interface ViewController : UIViewController<UIGestureRecognizerDelegate> {
    IBOutlet UIView     *_testView;
    IBOutlet UILabel    *_gestureLabel;
    IBOutlet UIView     *_buttonsView;
    UIGestureRecognizer *_recognizers[kGestureMax];
}

- (IBAction)switchPinchGesture:(id)sender;
- (IBAction)switchRotationGesture:(id)sender;
- (IBAction)switchPan2Gesture:(id)sender;
- (IBAction)switchTwoTaps2Gesture:(id)sender;

- (IBAction)switchPan1Gesture:(id)sender;
- (IBAction)switchTap1Gesture:(id)sender;
- (IBAction)switchTwoTaps1Gesture:(id)sender;
- (IBAction)switchLongPressGesture:(id)sender;

- (IBAction)switchSwipeRightGesture:(id)sender;
- (IBAction)switchSwipeLeftGesture:(id)sender;
- (IBAction)switchSwipeUpGesture:(id)sender;
- (IBAction)switchSwipeDownGesture:(id)sender;

@end
