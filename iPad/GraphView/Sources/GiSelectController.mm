// GiSelectController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiSelectController.h"
#include <Graph2d/gigraph.h>

@interface GiSelectController(Internal)

- (BOOL)hitTest:(void**)shapeFound point:(CGPoint)point;
- (void)addToSelection:(void*)shape;

@end

@implementation GiSelectController

- (id)initWithView:(GiGraphView*)view
{
    self = [super init];
    if (self) {
        _view = view;
        _count = 0;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)dynDraw:(GiGraphics*)gs
{
    GiContext context(-4, GiColor(0, 0, 255, 55));
    
    for (int i = 0; i < _count; i++) {
        [_view drawShape:_section[i] graphics:gs context:&context];
    }
}

- (BOOL)undoMotion
{
    if (_count > 0) {
        _count = 0;
        [_view setNeedsDisplay];
    }
    
    return YES;
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
    void *shape = NULL;
    
    if ([self hitTest:&shape point:[sender locationInView:sender.view]]) {
        _count = 0;
        [self addToSelection:shape];
    }
    else if (_count > 0) {
        _count = 0;
        [sender.view setNeedsDisplay];
    }
    
    return YES;
}

@end

@implementation GiSelectController(Internal)

- (void)addToSelection:(void*)shape
{
    if (_count < 100) {
        _section[_count] = shape;
        _count++;
        [_view setNeedsDisplay];
    }
}

- (BOOL)hitTest:(void**)shapeFound point:(CGPoint)point
{
    Box2d limits(Box2d(Point2d(point.x, point.y), 50, 50) * _view.xform->displayToModel());
    double minDist = limits.width();
    void *it, *shape;
    
    *shapeFound = NULL;
    for (shape = [_view getFirstShape:&it]; shape; shape = [_view getNextShape:&it]) {
        Box2d box = [_view getShapeExtent:shape];
        if (limits.isIntersect(box)) {
            double dist = [_view hitTest:shape limits:&limits];
            if (minDist > dist) {
                minDist = dist;
                *shapeFound = shape;
            }
        }
    }
    
    return *shapeFound != NULL;
}

@end
