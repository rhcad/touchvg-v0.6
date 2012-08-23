// ViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController<UIGestureRecognizerDelegate> {
    UIGestureRecognizer *_recognizers[12];
}

@property (retain, nonatomic) IBOutlet UILabel *gestureLabel;

@end
