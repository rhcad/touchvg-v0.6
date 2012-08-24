// ViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import <UIKit/UIKit.h>

enum kGestureType {
    kGesturePinch,
    kGestureRotate,
    kGestureTwoFingersPan,
    kGestureTwoFingersTwoTaps,
    
    kGesturePan,
    kGestureTap,
    kGestureTwoTaps,
    kGestureLongPress,
    
    kGestureSwipeRight,
    kGestureSwipeLeft,
    kGestureSwipeUp,
    kGestureSwipeDown,
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
