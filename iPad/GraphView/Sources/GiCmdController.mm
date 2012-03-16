// GiCmdController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiCmdController.h"
#import "GiGraphView.h"

@implementation GiCommandController

- (id)initWithView:(GiGraphView*)view
{
    self = [super init];
    if (self) {
        _view = view;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)dynDraw:(GiGraphics*)gs
{
}

- (BOOL)undoMotion
{
    return NO;
}

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    return NO;
}

@end
