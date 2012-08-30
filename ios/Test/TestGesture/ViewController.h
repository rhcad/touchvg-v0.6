// ViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import "GestureHandler.h"

@interface ViewController : UIViewController<UIGestureRecognizerDelegate,UIScrollViewDelegate> {
    id                  _lockedHandler;
    UIGestureRecognizer *_recognizers[kGestureMax];
    IBOutlet UIScrollView   *_testView;
    IBOutlet UILabel    *_gestureLabel;
    IBOutlet UIView     *_buttonsView;
}

- (IBAction)switchPinchGesture:(id)sender;
- (IBAction)switchRotationGesture:(id)sender;
- (IBAction)switchPan2Gesture:(id)sender;
- (IBAction)switchTap2Gesture:(id)sender;
- (IBAction)switchDblTaps2Gesture:(id)sender;

- (IBAction)switchPan1Gesture:(id)sender;
- (IBAction)switchTap1Gesture:(id)sender;
- (IBAction)switchDblTaps1Gesture:(id)sender;
- (IBAction)switchLongPressGesture:(id)sender;

- (IBAction)switchSwipeRightGesture:(id)sender;
- (IBAction)switchSwipeLeftGesture:(id)sender;
- (IBAction)switchSwipeUpGesture:(id)sender;
- (IBAction)switchSwipeDownGesture:(id)sender;

@end
