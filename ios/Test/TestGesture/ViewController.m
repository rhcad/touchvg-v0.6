// ViewController.m
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import "ViewController.h"
#import "TestPaintView.h"

@interface ViewController ()

- (void)addGestureRecognizers;
- (id)dispatchGesture:(SEL)aSelector :(UIGestureRecognizer *)gesture :(UIView *)pview;
- (void)addTestingViews;

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersRotate:(UIRotationGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
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
    [self addGestureRecognizers:_contentView];
	
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

- (void)addTestingViews
{
    float x = 0, y = 0, w = 300, h = 300, diff = 20;
    UIColor *bkColors[] = { [UIColor whiteColor], [UIColor greenColor],
        [UIColor purpleColor], [UIColor brownColor], [UIColor grayColor] };
    
    _contentView = [[UIView alloc] initWithFrame:_testView.bounds];
    _contentView.clipsToBounds = YES;
    [_testView addSubview:_contentView];
    [_contentView release];
    
    for (int i = 0; i < 15; i++) {
        TestPaintView *view1 = [[TestPaintView alloc]initWithFrame:CGRectMake(x, y, w, h)];
        [_contentView addSubview:view1];
        view1.tag = i + 1;
        view1.backgroundColor = bkColors[i % 5];
        [view1 release];
        
        x += w + diff;
        if (i % 3 == 2) {
            x = 0;
            y += h + diff;
        }
    }
    
    _contentView.frame = CGRectMake(0, 0, w * 3 + 2 * diff, y);
    _testView.contentSize = _contentView.frame.size;
    _testView.contentInset = UIEdgeInsetsMake(diff, diff, diff, diff);
    _testView.minimumZoomScale = 0.5f;
    _testView.maximumZoomScale = 3.0f;
    _testView.delegate = self;
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
}

- (void)scrollViewDidZoom:(UIScrollView *)scrollView
{
}

- (void)addGestureRecognizers:(UIView *)targetView
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
    _recognizers[kGesturePan] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
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
            [targetView addGestureRecognizer:_recognizers[i]];
            _recognizers[i].delegate = self;
            [_recognizers[i] release];
        }
    }
}

- (id)dispatchGesture:(SEL)aSelector :(UIGestureRecognizer *)gesture :(UIView *)pview
{
    id ret = nil;
    
    if (!_lockedHandler && (gesture.state == UIGestureRecognizerStateBegan
                            || gesture.state == UIGestureRecognizerStateEnded)) {
        _lockedHandler = nil;
        
        for (UIView *aview in pview.subviews) {
            if (!CGRectContainsPoint(aview.frame, [gesture locationInView:aview.superview]))
                continue;
            ret = [self dispatchGesture:aSelector :gesture :aview];
            if (ret) {
                break;
            }
            if ([aview respondsToSelector:aSelector]) {
                ret = [aview performSelector:aSelector withObject:gesture];
                if (ret) {
                    _lockedHandler = aview;
                    break;
                }
            }
            UIResponder* controller = [aview nextResponder];
            if ([controller isKindOfClass:[UIViewController class]]
                && [controller respondsToSelector:aSelector]) {
                ret = [controller performSelector:aSelector withObject:gesture];
                if (ret) {
                    _lockedHandler = controller;
                    break;
                }
            }
        }
    }
    else if ([_lockedHandler respondsToSelector:aSelector]) {
        ret = [_lockedHandler performSelector:aSelector withObject:gesture];
    }
    if (gesture.state == UIGestureRecognizerStateEnded
        || gesture.state == UIGestureRecognizerStateCancelled) {
        _lockedHandler = nil;
    }
    
    return ret;
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
    [self showGesture:sender :@"longPressGesture" :nil];
    
    [self dispatchGesture:@selector(longPressGesture:) :sender :self.view];
}

- (void)oneFingerOneTap:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"oneFingerOneTap" :nil];
    
    [self dispatchGesture:@selector(oneFingerOneTap:) :sender :self.view];
}

- (void)oneFingerTwoTaps:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"oneFingerTwoTaps" :nil];
    
    [self dispatchGesture:@selector(oneFingerTwoTaps:) :sender :self.view];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"oneFingerPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
    
    [self dispatchGesture:@selector(oneFingerPan:) :sender :self.view];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersPinch"
                     :[NSString stringWithFormat:@"scale:%6.2f\nv:%7.2f",
                       sender.scale, sender.velocity]];
    
    [self dispatchGesture:@selector(twoFingersPinch:) :sender :self.view];
}

- (void)twoFingersRotate:(UIRotationGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersRotate"
                     :[NSString stringWithFormat:@"rotation:%6.1fd\nv:%7.2f",
                       sender.rotation / M_PI * 180.f, sender.velocity]];
    
    [self dispatchGesture:@selector(twoFingersRotate:) :sender :self.view];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"twoFingersPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
    
    [self dispatchGesture:@selector(twoFingersPan:) :sender :self.view];
}

- (void)twoFingersTwoTaps:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersTwoTaps" :nil];
    
    [self dispatchGesture:@selector(twoFingersTwoTaps:) :sender :self.view];
}

- (void)swipeRightGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeRightGesture" :nil];
    
    if (![self dispatchGesture:@selector(swipeRightGesture:) :sender :self.view]) {
        
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
    [self showGesture:sender :@"swipeLeftGesture" :nil];
    
    if (![self dispatchGesture:@selector(swipeLeftGesture:) :sender :self.view]) {
        
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
    [self showGesture:sender :@"swipeDownGesture" :nil];
    
    [self dispatchGesture:@selector(swipeDownGesture:) :sender :self.view];
}

- (void)swipeUpGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeUpGesture" :nil];
    
    [self dispatchGesture:@selector(swipeUpGesture:) :sender :self.view];
}

@end
