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
    
    for (int i = 0; i < 12; i++)
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

- (void)addGestureRecognizers
{
    int n = 0;
    
    // 双指捏合手势
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    _recognizers[n++] = twoFingersPinch;
    
    // 双指旋转手势
    UIRotationGestureRecognizer *twoFingersRotate =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersRotate:)];
    _recognizers[n++] = twoFingersRotate;
    
    // 双指滑动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    _recognizers[n++] = twoFingersPan;
    
    // 双指双击手势
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    twoFingersTwoTaps.numberOfTapsRequired = 2;
    twoFingersTwoTaps.numberOfTouchesRequired = 2;
    _recognizers[n++] = twoFingersTwoTaps;
    
    // 单指滑动手势
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    oneFingerPan.maximumNumberOfTouches = 1;
    oneFingerPan.delaysTouchesBegan = YES;
    _recognizers[n++] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [oneFingerOneTap requireGestureRecognizerToFail:oneFingerPan];  // 不是滑动才算点击
    _recognizers[n++] = oneFingerOneTap;
    
    // 单指双击手势
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    oneFingerTwoTaps.numberOfTapsRequired = 2;
    _recognizers[n++] = oneFingerTwoTaps;
    
    // 单指长按手势
    UILongPressGestureRecognizer *longPressGesture =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGesture:)];
    longPressGesture.minimumPressDuration = 0.8;
    _recognizers[n++] = longPressGesture;
    
    // 单指轻扫手势
    UISwipeGestureRecognizer *swipeGestureH =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeGestureH:)];
    swipeGestureH.direction = (UISwipeGestureRecognizerDirectionRight
                              | UISwipeGestureRecognizerDirectionLeft);
    _recognizers[n++] = swipeGestureH;
    
    // 单指轻扫手势
    UISwipeGestureRecognizer *swipeGestureV =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeGestureV:)];
    swipeGestureV.direction = (UISwipeGestureRecognizerDirectionUp
                              | UISwipeGestureRecognizerDirectionDown);
    [swipeGestureH requireGestureRecognizerToFail:swipeGestureV];
    _recognizers[n++] = swipeGestureV;
    
    for (int i = 0; _recognizers[i]; i++) {
        _recognizers[i].delegate = self;
        [self.view addGestureRecognizer:_recognizers[i]];
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

- (void)swipeGestureH:(UISwipeGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeGestureH" :nil];
}

- (void)swipeGestureV:(UISwipeGestureRecognizer *)sender
{
    [self showGesture:sender :@"swipeGestureV" :nil];
}

@end
