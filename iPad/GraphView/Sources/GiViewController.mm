// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiViewController.h"
#include <Graph2d/mgshapest.h>
#include <list>
#import "GiCmdController.h"
#import "GiGraphView.h"

@interface GiViewController(GestureRecognizer)

- (id<GiView>)gview;
- (id<GiMotionHandler>)motionView:(SEL)aSelector;
- (id<GiMotionHandler>)getCommand:(SEL)aSelector;

- (void)addGestureRecognizers;
- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@implementation GiViewController

@synthesize gestureRecognizerUsed = _gestureRecognizerUsed;
@synthesize lineWidth;
@synthesize lineColor;
@synthesize fillColor;
@synthesize lineAlpha;
@synthesize fillAlpha;
@synthesize lineStyle;
@synthesize commandName;

- (id)init
{
    self = [super init];
    if (self) {
        _command = [[GiCommandController alloc]init];
        _shapesCreated = NULL;
        for (int i = 0; i < RECOGNIZER_COUNT; i++)
            _recognizers[i] = Nil;
        _gestureRecognizerUsed = YES;
    }
    return self;
}

- (void)viewDidLoad
{
    [[self gview] setDrawingDelegate:self];
    [self addGestureRecognizers];
}

- (void)dealloc
{
    [_command release];
    
    if (_shapesCreated) {
        ((MgShapes*)_shapesCreated)->release();
        _shapesCreated = NULL;
    }
    for (int i = 0; i < RECOGNIZER_COUNT; i++) {
        [_recognizers[i] release];
        _recognizers[i] = Nil;
    }
    [super dealloc];
}

- (UIView*)createGraphView:(CGRect)frame backgroundColor:(UIColor*)bkColor
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:frame];
    
    self.view = aview;
    aview.backgroundColor = bkColor;
    [aview setDrawingDelegate:self];
    
    aview.shapes = new MgShapesT<std::list<MgShape*> >;
    _shapesCreated = aview.shapes;
    
    if (!_recognizers[0])
        [self addGestureRecognizers];
    
    [aview release];
    return self.view;
}

- (UIView*)createSubGraphView:(UIView*)parentView
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:parentView.bounds];
    
    self.view = aview;
    aview.backgroundColor = [UIColor clearColor];
    aview.enableZoom = NO;
    [aview setDrawingDelegate:self];
    
    [parentView addSubview:aview];
    
    aview.shapes = new MgShapesT<std::list<MgShape*> >;
    _shapesCreated = aview.shapes;
    
    if (!_recognizers[0])
        [self addGestureRecognizers];
    
    [aview release];
    return self.view;
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
    [[self gview] getGraph]->clearCachedBitmap();
}

- (id)setCommand:(id)cmd
{
    assert(!cmd || [cmd conformsToProtocol:@protocol(GiMotionHandler)]);
    id oldcmd = _command;
    
    [[self getCommand:@selector(cancel)] cancel];
    _command = cmd;
    
    return oldcmd;
}


- (const char*)commandName {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.commandName;
}

- (void)setCommandName:(const char*)name {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.commandName = name;
}

- (int)lineWidth {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.context->getLineWidth();
}

- (void)setLineWidth:(int)w {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setLineWidth(w);
}

- (UIColor*)lineColor {
    GiCommandController* cmd = (GiCommandController*)_command;
    GiColor c = cmd.context->getLineColor();
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:1];
}

- (void)setLineColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_command;
    GiColor color(giFromCGColor(c.CGColor));
    if (color.a > 0.01f && cmd.context->getLineColor().a > 0)
        color.a = cmd.context->getLineColor().a;
    cmd.context->setLineColor(color);
}

- (UIColor*)fillColor {
    GiCommandController* cmd = (GiCommandController*)_command;
    GiColor c = cmd.context->getFillColor();
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:1];
}

- (void)setFillColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_command;
    GiColor color(giFromCGColor(c.CGColor));
    if (color.a > 0.01f && cmd.context->getFillColor().a > 0)
        color.a = cmd.context->getFillColor().a;
    cmd.context->setFillColor(color);
}

- (float)lineAlpha {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.context->getLineColor().a / 255.0f;
}

- (void)setLineAlpha:(float)a {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setLineAlpha(mgRound(a * 255));
}

- (float)fillAlpha {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.context->getFillColor().a / 255.0f;
}

- (void)setFillAlpha:(float)a {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setFillAlpha(mgRound(a * 255));
}

- (int)lineStyle {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.context->getLineStyle();
}

- (void)setLineStyle:(int)style {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setLineStyle((kLineStyle)style);
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

- (void)dynDraw
{
    GiGraphics* gs = [[self gview] getGraph];
    if (gs->isDrawing()) {
        [[self getCommand:@selector(dynDraw:)] dynDraw: gs];
    }
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
    return [_command respondsToSelector:aSelector] ?
        (id<GiMotionHandler>)_command : Nil;
}

- (void)addGestureRecognizers
{
    int n = 0;
    
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    _recognizers[n++] = twoFingersPinch;
    
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    [twoFingersPan setMaximumNumberOfTouches:2];
    _recognizers[n++] = twoFingersPan;
    
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    [oneFingerTwoTaps setNumberOfTapsRequired:2];
    _recognizers[n++] = oneFingerTwoTaps;
    
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [oneFingerOneTap requireGestureRecognizerToFail:oneFingerTwoTaps];
    _recognizers[n++] = oneFingerOneTap;
    
    _touchCount = 0;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++)
            [self.view addGestureRecognizer:_recognizers[i]];
    }
}

- (void)setGestureRecognizerUsed:(BOOL)used
{
    if (_gestureRecognizerUsed != used) {
        if (_gestureRecognizerUsed) {
            for (int i = 0; i < RECOGNIZER_COUNT; i++)
                [self.view removeGestureRecognizer:_recognizers[i]];
        }
        _gestureRecognizerUsed = used;
        if (_gestureRecognizerUsed) {
            for (int i = 0; i < RECOGNIZER_COUNT; i++)
                [self.view addGestureRecognizer:_recognizers[i]];
        }
    }
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    if (![[self getCommand:@selector(twoFingersPinch:)] twoFingersPinch:sender])
        [[self motionView:@selector(twoFingersPinch:)] twoFingersPinch:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        _touchCount = [sender numberOfTouches];
    }
    if (1 == _touchCount) {
        [self oneFingerPan:sender];
    }
    else if (![[self getCommand:@selector(twoFingersPan:)] twoFingersPan:sender]) {
        [[self motionView:@selector(twoFingersPan:)] twoFingersPan:sender];
    }
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (![[self getCommand:@selector(oneFingerPan:)] oneFingerPan:sender])
        [[self motionView:@selector(oneFingerPan:)] oneFingerPan:sender];
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    if (![[self getCommand:@selector(oneFingerOneTap:)] oneFingerOneTap:sender])
        [[self motionView:@selector(oneFingerOneTap:)] oneFingerOneTap:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    if (![[self getCommand:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender])
        [[self motionView:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender];
}

@end
