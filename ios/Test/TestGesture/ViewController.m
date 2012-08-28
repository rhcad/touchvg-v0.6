// ViewController.m
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import "ViewController.h"
#import "TestPaintView.h"

typedef struct {
    UIView  *view;
    UIGestureRecognizer *recognizers[kGestureMax];
} GestureView;

static GestureView s_views[20];
static int s_viewCount = 0;

@interface ViewController ()

- (void)addGestureRecognizers:(GestureView *)target;
- (id)dispatchGesture:(SEL)aSelector :(UIGestureRecognizer *)gesture
                     :(int)type :(UIView *)pview;
- (UIGestureRecognizer *)findRecognizers:(id)aview :(int)type;
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
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGesturePinch];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchRotationGesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureRotate];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchPan2Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureTwoFingersPan];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchTwoTaps2Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureTwoFingersTwoTaps];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchPan1Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGesturePan];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchTap1Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureTap];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchTwoTaps1Gesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureTwoTaps];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchLongPressGesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureLongPress];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeRightGesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureSwipeRight];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeLeftGesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureSwipeLeft];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeUpGesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureSwipeUp];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (IBAction)switchSwipeDownGesture:(id)sender
{
    UIGestureRecognizer *recognizer = [self findRecognizers:_lockedHandler :kGestureSwipeDown];
    UISwitch *button = (UISwitch *)sender;
    if (recognizer)
        recognizer.enabled = button ? button.on : !recognizer.enabled;
}

- (void)addTestingViews
{
    float x = 0, y = 0, w = 300, h = 300, diff = 20;
    UIColor *bkColors[] = { [UIColor whiteColor], [UIColor greenColor],
        [UIColor purpleColor], [UIColor brownColor], [UIColor grayColor] };
    
    for (int i = 0; i < 15; i++) {
        TestPaintView *view1 = [[TestPaintView alloc]initWithFrame:CGRectMake(x, y, w, h)];
        [_testView addSubview:view1];
        view1.tag = i + 1;
        view1.backgroundColor = bkColors[i % 5];
        [view1 release];
        
        s_views[s_viewCount].view = view1;
        [self addGestureRecognizers:&s_views[s_viewCount++]];
        
        x += w + diff;
        if (i % 3 == 2) {
            x = 0;
            y += h + diff;
        }
    }
    
    _testView.contentSize = CGSizeMake(w * 3 + 2 * diff, y);
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

- (UIGestureRecognizer *)findRecognizers:(id)aview :(int)type
{
    for (int i = 0; i < s_viewCount; i++) {
        if (s_views[i].view == aview && type >= 0 && type < kGestureMax) {
            return s_views[i].recognizers[type];
        }
    }
    return nil;
}

- (void)addGestureRecognizers:(GestureView *)target
{
    // 双指捏合手势
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    target->recognizers[kGesturePinch] = twoFingersPinch;
    
    // 双指旋转手势
    UIRotationGestureRecognizer *twoFingersRotate =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersRotate:)];
    target->recognizers[kGestureRotate] = twoFingersRotate;
    
    // 双指拖动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    target->recognizers[kGestureTwoFingersPan] = twoFingersPan;
    twoFingersPan.enabled = NO;     // 让底层滚动视图可响应双指拖动手势
    
    // 双指双击手势
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    twoFingersTwoTaps.numberOfTapsRequired = 2;
    twoFingersTwoTaps.numberOfTouchesRequired = 2;
    target->recognizers[kGestureTwoFingersTwoTaps] = twoFingersTwoTaps;
    
    // 单指拖动手势
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    oneFingerPan.maximumNumberOfTouches = 1;    // 最多一指，避免和双指拖动混淆
    target->recognizers[kGesturePan] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    oneFingerOneTap.delaysTouchesBegan = YES;
    target->recognizers[kGestureTap] = oneFingerOneTap;
    
    // 单指双击手势
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    oneFingerTwoTaps.numberOfTapsRequired = 2;
    target->recognizers[kGestureTwoTaps] = oneFingerTwoTaps;
    
    // 下行代码允许双击优先于单击，但会引起单击延迟或手势丢失问题
    //[oneFingerOneTap requireGestureRecognizerToFail:oneFingerTwoTaps];
    
    // 单指长按手势
    UILongPressGestureRecognizer *longPressGesture =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGesture:)];
    longPressGesture.minimumPressDuration = 0.8;
    target->recognizers[kGestureLongPress] = longPressGesture;
    
    UISwipeGestureRecognizer *swipeGesture;
    
    // 单指横向轻扫手势
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeRightGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionRight;
    target->recognizers[kGestureSwipeRight] = swipeGesture;
    
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeLeftGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionLeft;
    target->recognizers[kGestureSwipeLeft] = swipeGesture;
    
    // 单指纵向轻扫手势
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeUpGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionUp;
    target->recognizers[kGestureSwipeUp] = swipeGesture;
    
    swipeGesture = [[UISwipeGestureRecognizer alloc]
                    initWithTarget:self action:@selector(swipeDownGesture:)];
    swipeGesture.direction = UISwipeGestureRecognizerDirectionDown;
    target->recognizers[kGestureSwipeDown] = swipeGesture;
    
    for (int i = 0; i < kGestureMax; i++) {
        if (target->recognizers[i]) {
            [target->view addGestureRecognizer:target->recognizers[i]];
            target->recognizers[i].delegate = self;
            [target->recognizers[i] release];
        }
    }
}

- (id)dispatchGesture:(SEL)aSelector :(UIGestureRecognizer *)gesture
                     :(int)type :(UIView *)pview
{
    id ret = nil;
    BOOL click = (type == kGestureTwoFingersTwoTaps
                  || type == kGestureTap
                  || type == kGestureTwoTaps
                  || type >= kGestureSwipeRight);
    
    if (!_lockedHandler
        || gesture.state == UIGestureRecognizerStateBegan
        || (gesture.state == UIGestureRecognizerStateEnded && click)) {
        _lockedHandler = nil;
        
        for (UIView *aview in pview.subviews) {
            if (!CGRectContainsPoint(aview.frame, [gesture locationInView:aview.superview]))
                continue;
            ret = [self dispatchGesture:aSelector :gesture :type :aview];
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
        //_lockedHandler = nil;
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
    
    [self dispatchGesture:@selector(longPressGesture:)
                         :sender :kGestureLongPress :self.view];
}

- (void)oneFingerOneTap:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"oneFingerOneTap" :nil];
    
    [self dispatchGesture:@selector(oneFingerOneTap:)
                         :sender :kGestureTap :self.view];
}

- (void)oneFingerTwoTaps:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"oneFingerTwoTaps" :nil];
    
    [self dispatchGesture:@selector(oneFingerTwoTaps:)
                         :sender :kGestureTwoTaps :self.view];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"oneFingerPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
    
    [self dispatchGesture:@selector(oneFingerPan:)
                         :sender :kGesturePan :self.view];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersPinch"
                     :[NSString stringWithFormat:@"scale:%6.2f\nv:%7.2f",
                       sender.scale, sender.velocity]];
    
    [self dispatchGesture:@selector(twoFingersPinch:)
                         :sender :kGesturePinch :self.view];
}

- (void)twoFingersRotate:(UIRotationGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersRotate"
                     :[NSString stringWithFormat:@"rotation:%6.1fd\nv:%7.2f",
                       sender.rotation / M_PI * 180.f, sender.velocity]];
    
    [self dispatchGesture:@selector(twoFingersRotate:)
                         :sender :kGestureRotate :self.view];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:sender.view];
    CGPoint velocity = [sender velocityInView:sender.view];
    
    [self showGesture:sender :@"twoFingersPan"
                     :[NSString stringWithFormat:@"dx:%6.1f\ndy:%6.1f\nvx:%7.2f\nvy:%7.2f",
                       translation.x, translation.y, velocity.x, velocity.y]];
    
    [self dispatchGesture:@selector(twoFingersPan:) :sender
                         :kGestureTwoFingersPan :self.view];
}

- (void)twoFingersTwoTaps:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"twoFingersTwoTaps" :nil];
    
    [self dispatchGesture:@selector(twoFingersTwoTaps:)
                         :sender :kGestureTwoFingersTwoTaps :self.view];
}

- (void)swipeRightGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeRightGesture" :nil];
    
    if (![self dispatchGesture:@selector(swipeRightGesture:)
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
    [self showGesture:sender :@"swipeLeftGesture" :nil];
    
    if (![self dispatchGesture:@selector(swipeLeftGesture:) 
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
    [self showGesture:sender :@"swipeDownGesture" :nil];
    
    [self dispatchGesture:@selector(swipeDownGesture:) :sender :kGestureSwipeDown :self.view];
}

- (void)swipeUpGesture:(UIGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeUpGesture" :nil];
    
    [self dispatchGesture:@selector(swipeUpGesture:) :sender :kGestureSwipeUp :self.view];
}

@end
