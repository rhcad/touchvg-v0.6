// ViewController.m
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import "ViewController.h"
#import "TestPaintView.h"

@interface ViewController ()

- (void)addGestureRecognizers:(UIView *)target;
- (BOOL)dispatchGesture:(SEL)action :(UIGestureRecognizer *)gesture
                       :(int)type :(UIView *)pview;
- (int)getRecognizerType:(UIGestureRecognizer *)gesture;
- (void)addTestingViews;

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersRotate:(UIRotationGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTap:(UIGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UIGestureRecognizer *)sender;

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UIGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UIGestureRecognizer *)sender;
- (void)longPressGesture:(UIGestureRecognizer *)sender;

- (void)swipeLeftGesture:(UIGestureRecognizer *)sender;
- (void)swipeRightGesture:(UIGestureRecognizer *)sender;
- (void)swipeUpGesture:(UIGestureRecognizer *)sender;
- (void)swipeDownGesture:(UIGestureRecognizer *)sender;

@end

@implementation ViewController

- (int)getRecognizerType:(UIGestureRecognizer *)gesture
{
    for (int i = 0; i < kGestureMax; i++) {
        if (gesture == _recognizers[i])
            return i;
    }
    return -1;
}

- (void)dealloc {
    [_testView release];
    [_gestureLabel release];
    [_buttonsView release];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self addTestingViews];
	
    _gestureLabel.text = @"Ready to check gestures.";
}

- (void)viewDidUnload
{
    [_testView release];
    _testView = nil;
    [_gestureLabel release];
    _gestureLabel = nil;
    [_buttonsView release];
    _buttonsView = nil;
            
    [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES; // 允许转屏
}

- (IBAction)switchPinchGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGesturePinch];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchRotationGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureRotate];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchPan2Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureTwoPan];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchTap2Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureTwoTap];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchDblTaps2Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureTwoDblTaps];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchPan1Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGesturePan];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchTap1Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureTap];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchDblTaps1Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureDblTaps];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchLongPressGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureLongPress];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeRightGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureSwipeRight];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeLeftGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureSwipeLeft];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeUpGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureSwipeUp];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeDownGesture:(id)sender
{
    UIGestureRecognizer *recognizer = _recognizers[kGestureSwipeDown];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (void)addTestingViews
{
    float x = 0, y = 0, w = 300, h = 300, diff = 20;
    UIColor *bkColors[] = { [UIColor whiteColor], [UIColor greenColor],
        [UIColor purpleColor], [UIColor brownColor], [UIColor grayColor] };
    
    // 在滚动视图上覆盖透明视图，并在该透明视图上识别手势
    UIView *contentView = [[UIView alloc]initWithFrame:
                           CGRectMake(0, 0, (w + diff) * 3 - diff, (h + diff) * 5 - diff)];
    [_testView addSubview:contentView];
    [contentView release];
    [self addGestureRecognizers:contentView];
    [_testView.panGestureRecognizer requireGestureRecognizerToFail:_recognizers[kGesturePan]];
    
    TestDragView *viewD = [[TestDragView alloc]initWithFrame:CGRectMake(x, y, (w + diff) * 3 - diff, h * 2 + diff)];
    [contentView addSubview:viewD];
    viewD.backgroundColor = [UIColor clearColor];
    [viewD release];
        
    for (int i = 0; i < 12; i++) {
        TestPaintView *view1 = [[TestPaintView alloc]initWithFrame:CGRectMake(x, y, w, h)];
        [contentView addSubview:view1];
        view1.tag = i + 1;
        view1.backgroundColor = bkColors[i % 5];
        view1.alpha = 0.4f;
        [view1 release];
        
        x += w + diff;
        if (i % 3 == 2) {
            x = 0;
            y += h + diff;
        }
    }
    
    TestDragView *view2 = [[TestDragView alloc]initWithFrame:CGRectMake(x, y, (w + diff) * 3 - diff, h)];
    [contentView addSubview:view2];
    view2.backgroundColor = [UIColor darkGrayColor];
    [view2 release];
    
    _testView.contentSize = contentView.frame.size;
    _testView.contentInset = UIEdgeInsetsMake(diff, diff, diff, diff);
    _testView.minimumZoomScale = 0.3f;
    _testView.maximumZoomScale = 3.0f;
    _testView.delegate = self;
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
    return [scrollView.subviews objectAtIndex:0];
}

- (void)addGestureRecognizers:(UIView *)target
{
    // 双指捏合手势
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    _recognizers[kGesturePinch] = twoFingersPinch;
    
    // 双指旋转手势
    UIRotationGestureRecognizer *twoFingersRotate =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersRotate:)];
    _recognizers[kGestureRotate] = twoFingersRotate;
    
    // 双指拖动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    _recognizers[kGestureTwoPan] = twoFingersPan;
    twoFingersPan.enabled = NO;         // 让底层滚动视图可响应双指拖动手势
    
    // 双指点击手势
    UITapGestureRecognizer *twoFingersTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTap:)];
    twoFingersTap.numberOfTouchesRequired = 2;
    _recognizers[kGestureTwoTap] = twoFingersTap;
    
    // 双指双击手势
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    twoFingersTwoTaps.numberOfTapsRequired = 2;
    twoFingersTwoTaps.numberOfTouchesRequired = 2;
    _recognizers[kGestureTwoDblTaps] = twoFingersTwoTaps;
    
    // 单指拖动手势
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    oneFingerPan.maximumNumberOfTouches = 1;    // 最多一指，避免双指拖动识别为该手势
    _recognizers[kGesturePan] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    _recognizers[kGestureTap] = oneFingerOneTap;
    
    // 单指双击手势
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    oneFingerTwoTaps.numberOfTapsRequired = 2;
    _recognizers[kGestureDblTaps] = oneFingerTwoTaps;
    
    // 下行代码允许双击优先于单击，但会引起单击延迟或手势丢失问题
    //[oneFingerOneTap requireGestureRecognizerToFail:oneFingerTwoTaps];
    
    // 单指长按手势
    UILongPressGestureRecognizer *longPressGesture =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGesture:)];
    longPressGesture.minimumPressDuration = 0.8;
    _recognizers[kGestureLongPress] = longPressGesture;
    
    UISwipeGestureRecognizer *swipeGesture;
    
    // 单指横向轻扫手势
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeRightGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionRight;
    _recognizers[kGestureSwipeRight] = swipeGesture;
    
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeLeftGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionLeft;
    _recognizers[kGestureSwipeLeft] = swipeGesture;
    
    // 单指纵向轻扫手势
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeUpGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionUp;
    _recognizers[kGestureSwipeUp] = swipeGesture;
    
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeDownGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionDown;
    _recognizers[kGestureSwipeDown] = swipeGesture;
    
    for (int i = 0; i < kGestureMax; i++) {
        if (_recognizers[i]) {
            [target addGestureRecognizer:_recognizers[i]];
            _recognizers[i].delegate = self;
            [_recognizers[i] release];
        }
    }
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    int type = [self getRecognizerType:gestureRecognizer];
    SEL action = nil;
    
    switch (type) {
        case kGesturePinch:
            action = @selector(gesturePinch:);
            break;
        case kGestureRotate:
            action = @selector(gestureRotate:);
            break;
        case kGestureTwoPan:
            action = @selector(gestureTwoPan:);
            break;
        case kGestureTwoTap:
            action = @selector(gestureTwoTap:);
            break;
        case kGestureTwoDblTaps:
            action = @selector(gestureTwoDblTaps:);
            break;
        case kGesturePan:
            action = @selector(gesturePan:);
            break;
        case kGestureTap:
            action = @selector(gestureTap:);
            break;
        case kGestureDblTaps:
            action = @selector(gestureDblTaps:);
            break;
        case kGestureLongPress:
            action = @selector(gestureLongPress:);
            break;
        case kGestureSwipeRight:
            action = @selector(gestureSwipeRight:);
            break;
        case kGestureSwipeLeft:
            action = @selector(gestureSwipeLeft:);
            break;
        case kGestureSwipeUp:
            action = @selector(gestureSwipeUp:);
            break;
        case kGestureSwipeDown:
            action = @selector(gestureSwipeDown:);
            break;
        default:
            break;
    }
    
    return (action && !_lockedHandler
            && [self dispatchGesture:action :gestureRecognizer :type :self.view]);
}

- (BOOL)gestureShouldBegin:(SEL)action :(UIGestureRecognizer *)gesture
                          :(int)type :(id)target
{
    BOOL ret = NO;
    
    if ([target respondsToSelector:action]) {
        ret = (![target respondsToSelector:@selector(gestureShouldBegin::)]
               || [target performSelector:@selector(gestureShouldBegin::)
                               withObject:gesture withObject:(id)action]);
        if (ret) {
            _lockedHandler = target;
        }
    }
    
    return ret;
}

- (BOOL)dispatchGesture:(SEL)action :(UIGestureRecognizer *)gesture
                       :(int)type :(UIView *)pview
{
    BOOL handled = NO;
    
    if (!_lockedHandler) {
        for (UIView *aview in pview.subviews) {
            if (!CGRectContainsPoint(aview.frame, [gesture locationInView:aview.superview]))
                continue;
            UIResponder* controller = [aview nextResponder];
            handled = ([self dispatchGesture:action :gesture :type :aview]
                       || [self gestureShouldBegin:action :gesture :type :aview]
                       || ([controller isKindOfClass:[UIViewController class]]
                           && [self gestureShouldBegin:action :gesture :type :controller]));
            if (handled || _lockedHandler)
                break;
        }
    }
    else if ([_lockedHandler respondsToSelector:action]) {
        handled = !![_lockedHandler performSelector:action withObject:gesture];
    }
    if (gesture.state >= UIGestureRecognizerStateEnded) {
        _lockedHandler = nil;
    }
    
    return handled;
}

- (void)showGesture:(UIGestureRecognizer *)sender :(NSString *)name :(NSString *)info
{
    CGPoint pt = [sender locationInView:sender.view];
    NSString *str = [NSString stringWithFormat:@"%@\n%@\nstate:%d\n(%5.1f, %5.1f)",
                     name, info ? info : @"", sender.state, pt.x, pt.y];
    if ([sender numberOfTouches] > 1) {
        CGPoint pt2 = [sender locationOfTouch:1 inView:sender.view];
        pt = [sender locationOfTouch:0 inView:sender.view];
        str = [str stringByAppendingFormat:@":(%5.1f, %5.1f)(%5.1f, %5.1f)", 
               pt.x, pt.y, pt2.x, pt2.y];
    }
    _gestureLabel.text = str;
}

- (void)longPressGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureLongPress" :nil];
    
    [self dispatchGesture:@selector(gestureLongPress:)
                         :sender :kGestureLongPress :self.view];
}

- (void)oneFingerOneTap:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureTap" :nil];
    
    [self dispatchGesture:@selector(gestureTap:)
                         :sender :kGestureTap :self.view];
}

- (void)oneFingerTwoTaps:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureDblTaps" :nil];
    
    [self dispatchGesture:@selector(gestureDblTaps:)
                         :sender :kGestureDblTaps :self.view];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"gesturePan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
    
    [self dispatchGesture:@selector(gesturePan:)
                         :sender :kGesturePan :self.view];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    [self showGesture:sender :@"gesturePinch"
                     :[NSString stringWithFormat:@"scale:%6.2f\nv:%7.2f",
                       sender.scale, sender.velocity]];
    
    [self dispatchGesture:@selector(gesturePinch:)
                         :sender :kGesturePinch :self.view];
}

- (void)twoFingersRotate:(UIRotationGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureRotate"
                     :[NSString stringWithFormat:@"rotation:%6.1fd\nv:%7.2f",
                       sender.rotation / M_PI * 180.f, sender.velocity]];
    
    [self dispatchGesture:@selector(gestureRotate:)
                         :sender :kGestureRotate :self.view];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"gestureTwoPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
    
    [self dispatchGesture:@selector(gestureTwoPan:) :sender
                         :kGestureTwoPan :self.view];
}

- (void)twoFingersTap:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureTwoTap" :nil];
    
    [self dispatchGesture:@selector(gestureTwoTap:)
                         :sender :kGestureTwoTap :self.view];
}

- (void)twoFingersTwoTaps:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureTwoDblTaps" :nil];
    
    [self dispatchGesture:@selector(gestureTwoDblTaps:)
                         :sender :kGestureTwoDblTaps :self.view];
}

- (void)swipeRightGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureSwipeRight" :nil];
    
    if (![self dispatchGesture:@selector(gestureSwipeRight:)
                              :sender :kGestureSwipeRight :self.view]) {
        if (CGRectContainsPoint(_gestureLabel.frame, [sender locationInView:sender.view])
            && _buttonsView.hidden) {
            _buttonsView.hidden = NO;
            
            if (fabs(_gestureLabel.frame.origin.y - _buttonsView.frame.origin.y) < 5) {
                CGRect rect = _gestureLabel.frame;
                rect.origin.x = _buttonsView.frame.size.width;
                rect.size.width -= rect.origin.x;
                _gestureLabel.frame = rect;
            }
        }
    }
}

- (void)swipeLeftGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureSwipeLeft" :nil];
    
    if (![self dispatchGesture:@selector(gestureSwipeLeft:)
                              :sender :kGestureSwipeLeft :self.view]) {
        if (CGRectContainsPoint(_gestureLabel.frame, [sender locationInView:sender.view])
            && !_buttonsView.hidden) {
            _buttonsView.hidden = YES;
            
            if (fabs(_gestureLabel.frame.origin.y - _buttonsView.frame.origin.y) < 5) {
                CGRect rect = _gestureLabel.frame;
                rect.size.width += rect.origin.x;
                rect.origin.x = 0;
                _gestureLabel.frame = rect;
            }
        }
    }
}

- (void)swipeDownGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureSwipeDown" :nil];
    
    [self dispatchGesture:@selector(gestureSwipeDown:)
                         :sender :kGestureSwipeDown :self.view];
}

- (void)swipeUpGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"gestureSwipeUp" :nil];
    
    [self dispatchGesture:@selector(gestureSwipeUp:)
                         :sender :kGestureSwipeUp :self.view];
}

@end
