// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiViewController.h"
#import "GiGraphView.h"
#import "GiSelectController.h"
#import "GiCmdController.h"
#include <Graph2d/gigraph.h>

@interface GiViewController(GestureRecognizer)

- (void)addGestureRecognizers;
- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@implementation GiViewController

- (void)viewDidLoad
{
    _gview = (GiGraphView*)self.view;
    [_gview setDrawingDelegate:self];
    
    _selector = [[GiSelectController alloc]initWithView:_gview];
    _commands = [[GiCommandController alloc]initWithView:_gview];
    
    [self addGestureRecognizers];
}

- (void)dealloc
{
    [_selector release];
    [_commands release];
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
    _gview.graph->clearCachedBitmap();
}

#pragma mark - View motion

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;     // supported orientations
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)orientation duration:(NSTimeInterval)duration
{
    [_gview setAnimating:YES];
    [super willRotateToInterfaceOrientation:orientation duration:duration];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [_gview setAnimating:NO];
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
        BOOL ret = NO;
        
        if (_gview.viewMode == GiViewModeSelect)
            ret = [_selector undoMotion];
        if (_gview.viewMode == GiViewModeCommand)
            ret = [_commands undoMotion];
        
        if (!ret)
            ret = [_gview undoMotion];
    }
}

- (void)dynDraw
{
    if (_gview.viewMode == GiViewModeSelect)
        [_selector dynDraw:_gview.graph];
    if (_gview.viewMode == GiViewModeCommand)
        [_commands dynDraw:_gview.graph];
}

@end

@implementation GiViewController(GestureRecognizer)

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
    
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    [twoFingersTwoTaps setNumberOfTapsRequired:2];
    [twoFingersTwoTaps setNumberOfTouchesRequired:2];
    [self.view addGestureRecognizer:twoFingersTwoTaps];
    [twoFingersTwoTaps release];
    
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [self.view addGestureRecognizer:oneFingerOneTap];
    [oneFingerOneTap release];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    BOOL ret = NO;
    
    if (_gview.viewMode == GiViewModeSelect)
        ret = [_selector twoFingersPinch:sender];
    if (_gview.viewMode == GiViewModeCommand)
        ret = [_commands twoFingersPinch:sender];
    
    if (!ret)
        ret = [_gview twoFingersPinch:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    BOOL ret = NO;
    
    if (_gview.viewMode == GiViewModeSelect)
        ret = [_selector twoFingersPan:sender];
    if (_gview.viewMode == GiViewModeCommand)
        ret = [_commands twoFingersPan:sender];
    
    if (!ret)
        ret = [_gview twoFingersPan:sender];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    BOOL ret = NO;
    
    if (_gview.viewMode == GiViewModeSelect)
        ret = [_selector oneFingerPan:sender];
    if (_gview.viewMode == GiViewModeCommand)
        ret = [_commands oneFingerPan:sender];
    
    if (!ret)
        ret = [_gview oneFingerPan:sender];
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    BOOL ret = NO;
    
    if (_gview.viewMode == GiViewModeSelect)
        ret = [_selector oneFingerOneTap:sender];
    if (_gview.viewMode == GiViewModeCommand)
        ret = [_commands oneFingerOneTap:sender];
    
    if (!ret)
        ret = [_gview oneFingerOneTap:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    BOOL ret = NO;
    
    if (_gview.viewMode == GiViewModeSelect)
        ret = [_selector oneFingerTwoTaps:sender];
    if (_gview.viewMode == GiViewModeCommand)
        ret = [_commands oneFingerTwoTaps:sender];
    
    if (!ret)
        ret = [_gview oneFingerTwoTaps:sender];
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    BOOL ret = NO;
    
    if (_gview.viewMode == GiViewModeSelect)
        ret = [_selector twoFingersTwoTaps:sender];
    if (_gview.viewMode == GiViewModeCommand)
        ret = [_commands twoFingersTwoTaps:sender];
    
    if (!ret)
        ret = [_gview twoFingersTwoTaps:sender];
    
    if (!ret) {
        [_selector undoMotion];
        //_gview.viewMode = (GiViewMode)((_gview.viewMode + 1) % GiViewModeMax);
    }
}

@end
