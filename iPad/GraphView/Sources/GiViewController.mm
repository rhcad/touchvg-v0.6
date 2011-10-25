// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchdraw

#import "GiViewController.h"
#include <mgshapest.h>
#include <list>
#import "GiCmdController.h"
#import "GiGraphView.h"

@interface GiViewController(GestureRecognizer)

- (id<GiView>)gview;
- (id<GiMotionHandler>)motionView:(SEL)aSelector;
- (id<GiMotionHandler>)getCommand:(SEL)aSelector;
- (void)updateMagnifierCenter:(UIGestureRecognizer *)sender;

- (void)addGestureRecognizers:(int)t view:(UIView*)view;
- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@implementation GiViewController

@synthesize gestureRecognizerUsed = _gestureRecognizerUsed;
@synthesize magnifierView;
@synthesize activeView = _activeView;
@synthesize lineWidth;
@synthesize lineColor;
@synthesize fillColor;
@synthesize lineAlpha;
@synthesize fillAlpha;
@synthesize lineStyle;
@synthesize commandName;
@synthesize shapes;

- (id)init
{
    self = [super init];
    if (self) {
        for (int iv = 0; iv < 3; iv++)
            _magnifierView[iv] = Nil;
        _cmdctl = [[GiCommandController alloc]initWithViews:_magnifierView];
        _shapesCreated = NULL;
        for (int t = 0; t < 2; t++) {
            for (int i = 0; i < RECOGNIZER_COUNT; i++)
                _recognizers[t][i] = Nil;
        }
        _gestureRecognizerUsed = YES;
    }
    return self;
}

- (void)viewDidLoad
{
    _activeView = self.view;
    [[self gview] setDrawingDelegate:self];
    [self addGestureRecognizers:0 view:self.view];
    
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd touchesBegan:CGPointZero view:_activeView];
    cmd.lineWidth = 50;
    cmd.lineColor = GiColor(0, 0, 0, 128);
}

- (void)dealloc
{
    [_cmdctl release];
    
    if (_shapesCreated) {
        ((MgShapes*)_shapesCreated)->release();
        _shapesCreated = NULL;
    }
    for (int t = 0; t < 2; t++) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [_recognizers[t][i] release];
            _recognizers[t][i] = Nil;
        }
    }
    [super dealloc];
}

- (UIView*)createGraphView:(CGRect)frame backgroundColor:(UIColor*)bkColor shapes:(void*)sp
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:frame];
    
    self.view = aview;
    aview.backgroundColor = bkColor;
    [aview setDrawingDelegate:self];
    
    if (sp) {
        aview.shapes = (MgShapes*)sp;
    }
    else
    {
        aview.shapes = new MgShapesT<std::list<MgShape*> >;
        _shapesCreated = aview.shapes;
    }
    
    [self viewDidLoad];
    
    [aview release];
    return self.view;
}

- (UIView*)createSubGraphView:(UIView*)parentView frame:(CGRect)frame shapes:(void*)sp
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:frame];
    
    self.view = aview;
    aview.backgroundColor = [UIColor clearColor];
    aview.enableZoom = NO;
    
    [aview setDrawingDelegate:self];
    [parentView addSubview:aview];
    
    if (sp) {
        aview.shapes = (MgShapes*)sp;
    }
    else
    {
        aview.shapes = new MgShapesT<std::list<MgShape*> >;
        _shapesCreated = aview.shapes;
    }
    
    [self addGestureRecognizers:0 view:aview];
    
    [aview release];
    return self.view;
}

- (UIView*)createMagnifierView:(UIView*)parentView frame:(CGRect)frame scale:(CGFloat)scale
{
    GiMagnifierView *aview = [[GiMagnifierView alloc] initWithFrame:frame graphView:[self gview]];
    _magnifierView[_magnifierView[0] ? 1 : 0] = aview;
    aview.backgroundColor = [UIColor clearColor];
    aview.scale = scale;
    
    [aview setDrawingDelegate:self];
    [parentView addSubview:aview];
    [self addGestureRecognizers:1 view:aview];
    
    [aview release];
    return aview;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
    [self clearCachedData];
}

- (void)clearCachedData
{
    [[self gview] graph]->clearCachedBitmap();
}

- (UIView*)magnifierView {
    if (_magnifierView[1] && [_magnifierView[1].gestureRecognizers count] > 0)
        return _magnifierView[1];
    return _magnifierView[0];
}

- (void*)shapes {
    return [[self gview] shapes];
}

- (const char*)commandName {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.commandName;
}

- (void)setCommandName:(const char*)name {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    cmd.commandName = name;
}

- (int)lineWidth {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.lineWidth;
}

- (void)setLineWidth:(int)w {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd setLineWidth:w];
}

- (UIColor*)lineColor {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor c = cmd.lineColor;
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:1];
}

- (void)setLineColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(giFromCGColor(c.CGColor));
    if (color.a > 0.01f && cmd.lineColor.a > 0)
        color.a = cmd.lineColor.a;
    [cmd setLineColor:color];
}

- (UIColor*)fillColor {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor c = cmd.fillColor;
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:1];
}

- (void)setFillColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(giFromCGColor(c.CGColor));
    if (color.a > 0.01f && cmd.fillColor.a > 0)
        color.a = cmd.fillColor.a;
    [cmd setFillColor:color];
}

- (float)lineAlpha {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.lineColor.a / 255.0f;
}

- (void)setLineAlpha:(float)a {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(cmd.lineColor);
    color.a = mgRound(a * 255);
    [cmd setLineColor:color];
}

- (float)fillAlpha {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.fillColor.a / 255.0f;
}

- (void)setFillAlpha:(float)a {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(cmd.fillColor);
    color.a = mgRound(a * 255);
    [cmd setFillColor:color];
}

- (int)lineStyle {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.lineStyle;
}

- (void)setLineStyle:(int)style {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd setLineStyle:style];
}

#pragma mark - View motion

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;     // supported orientations
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)orientation duration:(NSTimeInterval)duration
{
    [[self gview] setAnimating:YES];
    [super willRotateToInterfaceOrientation:orientation duration:duration];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [[self gview] setAnimating:NO];
    [super didRotateFromInterfaceOrientation:fromInterfaceOrientation];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    [self becomeFirstResponder];
}

- (void)viewDidDisappear:(BOOL)animated {
    [self resignFirstResponder];
    [super viewDidDisappear:animated];
}

- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
    if (motion == UIEventSubtypeMotionShake) {
        [self undoMotion];
    }
}

- (void)undoMotion
{
    if (![[self getCommand:@selector(undoMotion)] undoMotion])
        [[self motionView:@selector(undoMotion)] undoMotion];
}

- (void)dynDraw:(id)sender
{
    GiGraphics* gs = NULL;
    
    if ([sender conformsToProtocol:@protocol(GiView)]) {
        id<GiView> aview = (id<GiView>)sender;
        gs = [aview graph];
    }
    else if (sender == _magnifierView[0]) {
        GiMagnifierView *aview = (GiMagnifierView *)_magnifierView[0];
        gs = [aview graph];
    }
    else if (sender == _magnifierView[1]) {
        GiMagnifierView *aview = (GiMagnifierView *)_magnifierView[1];
        gs = [aview graph];
    }
    
    if (gs && gs->isDrawing()) {
        [[self getCommand:@selector(dynDraw:)] dynDraw: gs];
    }
}

- (void)afterZoomed:(id)sender
{
    [self.view setNeedsDisplay];
    [_magnifierView[0] setNeedsDisplay];
    [_magnifierView[1] setNeedsDisplay];
}

static CGPoint _ignorepoint = CGPointMake(-1000, -1000);

+ (void)ignoreTouchesBegan:(CGPoint)point view:(UIView*)sender {
    _ignorepoint = [sender convertPoint:point toView:[sender window]];
}

@end

@implementation GiViewController(GestureRecognizer)

- (id<GiView>)gview
{
    assert([self.view conformsToProtocol:@protocol(GiView)]);
    return (id<GiView>)self.view;
}

- (id<GiMotionHandler>)motionView:(SEL)aSelector
{
    if ([self.view conformsToProtocol:@protocol(GiMotionHandler)]
        && [self.view respondsToSelector:aSelector]) {
        return (id<GiMotionHandler>)self.view;
    }
    return Nil;
}

- (id<GiMotionHandler>)getCommand:(SEL)aSelector
{
    return [_cmdctl respondsToSelector:aSelector] ?
        (id<GiMotionHandler>)_cmdctl : Nil;
}

- (void)addGestureRecognizers:(int)t view:(UIView*)view
{
    if (_recognizers[t][0])
        return;
    
    int n = 0;
    
    // 双指捏合手势
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    _recognizers[t][n++] = twoFingersPinch;
    
    // 双指滑动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    twoFingersPan.delaysTouchesEnded = NO;
    [twoFingersPan requireGestureRecognizerToFail:twoFingersPinch];
    _recognizers[t][n++] = twoFingersPan;
    
    // 单指滑动手势
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    oneFingerPan.maximumNumberOfTouches = 2;
    oneFingerPan.delaysTouchesEnded = NO;
    _recognizers[t][n++] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    oneFingerOneTap.delaysTouchesEnded = NO;
    [oneFingerOneTap requireGestureRecognizerToFail:oneFingerPan];  // 不是滑动才算点击
    _recognizers[t][n++] = oneFingerOneTap;
    
    // 单指双击手势
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    oneFingerTwoTaps.numberOfTapsRequired = 2;
    _recognizers[t][n++] = oneFingerTwoTaps;
    
    // 双指双击手势
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    twoFingersTwoTaps.numberOfTapsRequired = 2;
    twoFingersTwoTaps.numberOfTouchesRequired = 2;
    _recognizers[t][n++] = twoFingersTwoTaps;
    
    _touchCount = 0;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++)
            [view addGestureRecognizer:_recognizers[t][i]];
    }
}

- (void)setGestureRecognizerUsed:(BOOL)used
{
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [self.view removeGestureRecognizer:_recognizers[0][i]];
            [_magnifierView[0] removeGestureRecognizer:_recognizers[1][i]];
        }
    }
    _gestureRecognizerUsed = used;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [self.view addGestureRecognizer:_recognizers[0][i]];
            [_magnifierView[0] addGestureRecognizer:_recognizers[1][i]];
        }
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:touch.view];
    CGPoint ignorept = [touch.view convertPoint:_ignorepoint fromView:[touch.view window]];
    
    _ignorepoint = CGPointMake(-1000, -1000);
    _ignoreTouches = CGPointEqualToPoint(point, ignorept);
    
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd touchesBegan:point view:touch.view];
    
    _activeView = touch.view;
    if (touch.view == self.view) {
        [super touchesBegan:touches withEvent:event];
    }
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(twoFingersPinch:)] twoFingersPinch:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(twoFingersPinch:)] twoFingersPinch:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (sender.state == UIGestureRecognizerStateBegan) {
        _touchCount = [sender numberOfTouches];
    }
    if (1 == _touchCount) {
        [self oneFingerPan:sender];
    }
    else if (sender.view == _magnifierView[0]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magnifierView[0];
        [zview twoFingersPan:sender];
    }
    else if (sender.view == _magnifierView[1]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magnifierView[1];
        [zview twoFingersPan:sender];
    }
    else if (_touchCount > 1) {
        if (![[self getCommand:@selector(twoFingersPan:)] twoFingersPan:sender]) {
            [[self motionView:@selector(twoFingersPan:)] twoFingersPan:sender];
        }
        [self updateMagnifierCenter:sender];
    }
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (sender.state == UIGestureRecognizerStateBegan) {
        _touchCount = [sender numberOfTouches];
    }
    if (2 == _touchCount) {
        [self twoFingersPan:sender];
    }
    else if (1 == _touchCount) {
        if (![[self getCommand:@selector(oneFingerPan:)] oneFingerPan:sender]
            && sender.view == self.view) {
            [[self motionView:@selector(oneFingerPan:)] oneFingerPan:sender];
        }
        [self updateMagnifierCenter:sender];
    }
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(oneFingerOneTap:)] oneFingerOneTap:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerOneTap:)] oneFingerOneTap:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(twoFingersTwoTaps:)] twoFingersTwoTaps:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(twoFingersTwoTaps:)] twoFingersTwoTaps:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)updateMagnifierCenter:(UIGestureRecognizer *)sender
{
    if (!_magnifierView[0] || sender.view != self.view)
        return;
    
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiMagnifierView *zview = (GiMagnifierView *)_magnifierView[0];
    
    if ([sender numberOfTouches] > 0) {
        [zview automoveSuperview:[sender locationInView:sender.view] fromView:self.view];
    }
    
    if (sender.state == UIGestureRecognizerStateEnded) {
        [zview setPointWandRedraw:[cmd getPointModel]];
    }
    else {
        [zview setPointW:[cmd getPointModel]];
    }
}

@end
