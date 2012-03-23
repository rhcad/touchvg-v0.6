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


- (const char*)getCommandName {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.commandName;
}

- (void)setCommandName:(const char*)name {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.commandName = name;
}

- (int)getLineWidth {
    GiCommandController* cmd = (GiCommandController*)_command;
    return cmd.context->getLineWidth();
}

- (void)setLineWidth:(int)w {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setLineWidth(w);
}

- (UIColor*)getLineColor {
    GiCommandController* cmd = (GiCommandController*)_command;
    GiColor c = cmd.context->getLineColor();
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:c.a];
}

- (void)setLineColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setLineColor(giFromCGColor(c.CGColor));
}

- (UIColor*)getFillColor {
    GiCommandController* cmd = (GiCommandController*)_command;
    GiColor c = cmd.context->getFillColor();
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:c.a];
}

- (void)setFillColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_command;
    cmd.context->setFillColor(giFromCGColor(c.CGColor));
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
    [twoFingersPan setMinimumNumberOfTouches:2];
    [twoFingersPan setMaximumNumberOfTouches:2];
    _recognizers[n++] = twoFingersPan;
    
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    [oneFingerPan setMaximumNumberOfTouches:1];
    _recognizers[n++] = oneFingerPan;
    
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    [oneFingerTwoTaps setNumberOfTapsRequired:2];
    _recognizers[n++] = oneFingerTwoTaps;
    
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [oneFingerOneTap requireGestureRecognizerToFail:oneFingerTwoTaps];
    _recognizers[n++] = oneFingerOneTap;
    
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
    if (![[self getCommand:@selector(twoFingersPan:)] twoFingersPan:sender])
        [[self motionView:@selector(twoFingersPan:)] twoFingersPan:sender];
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
