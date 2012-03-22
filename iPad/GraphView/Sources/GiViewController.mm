// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiViewController.h"
#import "GiCmdController.h"
#import "GiGraphView.h"
#include <Graph2d/mgshapest.h>
#include <list>

@interface GiViewController(GestureRecognizer)

- (id<GiView>)gview;
- (id<GiMotionHandler>)motionView;
- (id<GiMotionHandler>)getCommand;

- (void)addGestureRecognizers;
- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@implementation GiViewController

@synthesize gestureRecognizerUsed = _gestureRecognizerUsed;

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
    
    [[self getCommand] cancel];
    _command = cmd;
    
    return oldcmd;
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
    if (![[self getCommand] undoMotion])
        [[self motionView] undoMotion];
}

- (void)dynDraw
{
    GiGraphics* gs = [[self gview] getGraph];
    if (gs->isDrawing()) {
        [[self getCommand] dynDraw: gs];
    }
}

@end

@implementation GiViewController(GestureRecognizer)

- (id<GiView>)gview
{
    assert([self.view conformsToProtocol:@protocol(GiView)]);
    return (id<GiView>)self.view;
}

- (id<GiMotionHandler>)motionView
{
    if ([self.view conformsToProtocol:@protocol(GiMotionHandler)])
        return (id<GiMotionHandler>)self.view;
    return Nil;
}

- (id<GiMotionHandler>)getCommand
{
    return (id<GiMotionHandler>)_command;
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
    if (![[self getCommand] twoFingersPinch:sender])
        [[self motionView] twoFingersPinch:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (![[self getCommand] twoFingersPan:sender])
        [[self motionView] twoFingersPan:sender];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (![[self getCommand] oneFingerPan:sender])
        [[self motionView] oneFingerPan:sender];
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    if (![[self getCommand] oneFingerOneTap:sender])
        [[self motionView] oneFingerOneTap:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    if (![[self getCommand] oneFingerTwoTaps:sender])
        [[self motionView] oneFingerTwoTaps:sender];
}

@end
