// ViewController.m
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import "ViewController.h"

@interface ViewController ()
@end

@implementation ViewController
@synthesize gestureLabel;

- (void)dealloc {
    [gestureLabel release];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    for (int i = 0; i < kGestureMax; i++)
        _recognizers[i] = nil;
    [self addGestureRecognizers];
	
    gestureLabel.text = @"Ready to check gestures.";
}

- (void)viewDidUnload
{
    [self setGestureLabel:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES; // 允许转屏
}

- (IBAction)switchPinchGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGesturePinch].enabled = button.on;
}

- (IBAction)switchRotationGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureRotate].enabled = button.on;
}

- (IBAction)switchPan2Gesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureTwoFingersPan].enabled = button.on;
}

- (IBAction)switchTwoTaps2Gesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureTwoFingersTwoTaps].enabled = button.on;
}

- (IBAction)switchPan1Gesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGesturePan].enabled = button.on;
}

- (IBAction)switchTap1Gesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureTap].enabled = button.on;
}

- (IBAction)switchTwoTaps1Gesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureTwoTaps].enabled = button.on;
}

- (IBAction)switchLongPressGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureLongPress].enabled = button.on;
}

- (IBAction)switchSwipeRightGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureSwipeRight].enabled = button.on;
}

- (IBAction)switchSwipeLeftGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureSwipeLeft].enabled = button.on;
}

- (IBAction)switchSwipeUpGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureSwipeUp].enabled = button.on;
}

- (IBAction)switchSwipeDownGesture:(id)sender
{
    UISwitch *button = (UISwitch *)sender;
    _recognizers[kGestureSwipeDown].enabled = button.on;
}

- (void)addGestureRecognizers
{
    // 双指捏合手势
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    _recognizers[kGesturePinch] = twoFingersPinch;
    
    // 双指旋转手势
    UIRotationGestureRecognizer *twoFingersRotate =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersRotate:)];
    _recognizers[kGestureRotate] = twoFingersRotate;
    
    // 双指滑动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    _recognizers[kGestureTwoFingersPan] = twoFingersPan;
    
    // 双指双击手势
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    twoFingersTwoTaps.numberOfTapsRequired = 2;
    twoFingersTwoTaps.numberOfTouchesRequired = 2;
    _recognizers[kGestureTwoFingersTwoTaps] = twoFingersTwoTaps;
    
    // 单指滑动手势
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    oneFingerPan.maximumNumberOfTouches = 1;
    oneFingerPan.delaysTouchesBegan = YES;
    _recognizers[kGesturePan] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [oneFingerOneTap requireGestureRecognizerToFail:oneFingerPan];  // 不是滑动才算点击
    _recognizers[kGestureTap] = oneFingerOneTap;
    
    // 单指双击手势
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    oneFingerTwoTaps.numberOfTapsRequired = 2;
    _recognizers[kGestureTwoTaps] = oneFingerTwoTaps;
    
    // 单指长按手势
    UILongPressGestureRecognizer *longPressGesture =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGesture:)];
    longPressGesture.minimumPressDuration = 0.8;
    _recognizers[kGestureLongPress] = longPressGesture;
    
    UISwipeGestureRecognizer *swipeGesture;
    
    // 单指横向轻扫手势
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeRightGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionRight;
    _recognizers[kGestureSwipeRight] = swipeGesture;
    
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeLeftGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionLeft;
    _recognizers[kGestureSwipeLeft] = swipeGesture;
    
    // 单指纵向轻扫手势
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeUpGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionUp;
    _recognizers[kGestureSwipeUp] = swipeGesture;
    
    swipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeDownGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionDown;
    _recognizers[kGestureSwipeDown] = swipeGesture;
    
    for (int i = 0; i < kGestureMax; i++) {
        if (_recognizers[i]) {
            _recognizers[i].delegate = self;
            [self.view addGestureRecognizer:_recognizers[i]];
        }
    }
}

- (void)showGesture:(UIGestureRecognizer *)sender :(NSString *)name :(NSString *)info
{
    CGPoint pt = [sender locationInView:sender.view];
    NSString *str = [NSString stringWithFormat:@"%@\n%@\nstate:%d\npoints:%d (%6.1f, %6.1f)",
                     name, info ? info : @"", sender.state, 
                     sender.numberOfTouches, pt.x, pt.y];    
    gestureLabel.text = str;
}

- (void)longPressGesture:(UILongPressGestureRecognizer *)sender
{
    [self showGesture:sender :@"longPressGesture" :nil];
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    [self showGesture:sender :@"oneFingerOneTap" :nil];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    [self showGesture:sender :@"oneFingerTwoTaps" :nil];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"oneFingerPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersPinch"
                     :[NSString stringWithFormat:@"scale:%6.2f\nv:%7.2f",
                       sender.scale, sender.velocity]];
}

- (void)twoFingersRotate:(UIRotationGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersRotate"
                     :[NSString stringWithFormat:@"rotation:%6.1fd\nv:%7.2f",
                       sender.rotation / M_PI * 180.f, sender.velocity]];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"twoFingersPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersTwoTaps" :nil];
}

- (void)swipeRightGesture:(UISwipeGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeRightGesture" :nil];
}

- (void)swipeLeftGesture:(UISwipeGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeLeftGesture" :nil];
}

- (void)swipeDownGesture:(UISwipeGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeDownGesture" :nil];
}

- (void)swipeUpGesture:(UISwipeGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeUpGesture" :nil];
}

@end
