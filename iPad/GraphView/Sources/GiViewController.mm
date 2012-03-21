// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiViewController.h"
#import "GiSelectController.h"
#include <Graph2d/gigraph.h>

@interface GiViewController(GestureRecognizer)

- (id<GiView, GiMotionHandler>)gview;
- (id<GiMotionHandler>)getCommand;
- (void)addGestureRecognizers;
- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@implementation GiViewController

- (void)viewDidLoad
{
    _command = Nil;
    [[self gview] setDrawingDelegate:self];
    _selector = [[GiSelectController alloc]initWithView:[self gview]];
    
    [self addGestureRecognizers];
}

- (void)dealloc
{
    [_selector release];
    [super dealloc];
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
    id old = _command;
    
    _command = cmd;
    
    return old;
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
    if (motion == UIEventSubtypeMotionShake)
    {
        id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
        if (![cmd undoMotion:self.view])
            [[self gview] undoMotion:self.view];
    }
}

- (void)dynDraw
{
    id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
    [cmd dynDraw: [[self gview] getGraph]];
}

@end

@implementation GiViewController(GestureRecognizer)

- (id<GiView, GiMotionHandler>)gview
{
    assert([self.view conformsToProtocol:@protocol(GiView)]);
    assert([self.view conformsToProtocol:@protocol(GiMotionHandler)]);
    return (id<GiView, GiMotionHandler>)self.view;
}

- (id<GiMotionHandler>)getCommand
{
    return (id<GiMotionHandler>)_command;
}

- (void)addGestureRecognizers
{
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    [self.view addGestureRecognizer:twoFingersPinch];
    [twoFingersPinch release];
    
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    [twoFingersPan setMinimumNumberOfTouches:2];
    [twoFingersPan setMaximumNumberOfTouches:2];
    [self.view addGestureRecognizer:twoFingersPan];
    [twoFingersPan release];
    
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    [oneFingerPan setMaximumNumberOfTouches:1];
    [self.view addGestureRecognizer:oneFingerPan];
    [oneFingerPan release];
    
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    [oneFingerTwoTaps setNumberOfTapsRequired:2];
    [self.view addGestureRecognizer:oneFingerTwoTaps];
    [oneFingerTwoTaps release];
    
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [self.view addGestureRecognizer:oneFingerOneTap];
    [oneFingerOneTap release];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
    if (![cmd twoFingersPinch:sender])
        [[self gview] twoFingersPinch:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
    if (![cmd twoFingersPan:sender])
        [[self gview] twoFingersPan:sender];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
    if (![cmd oneFingerPan:sender])
        [[self gview] oneFingerPan:sender];
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
    if (![cmd oneFingerOneTap:sender])
        [[self gview] oneFingerOneTap:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    id<GiMotionHandler> cmd = [self getCommand] ? [self getCommand] : _selector;
    if (![cmd oneFingerTwoTaps:sender])
        [[self gview] oneFingerTwoTaps:sender];
}

@end
