// GiGraphView.mm
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include <GiGraphIos.h>

@implementation GiGraphView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _xform = new GiTransform();
        _graph = new GiGraphIos(*_xform);
        [self afterCreated];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _xform = new GiTransform();
        _graph = new GiGraphIos(*_xform);
        [self afterCreated];
    }
    return self;
}

- (void)dealloc
{
    if (_graph) {
        delete _graph;
        _graph = NULL;
    }
    if (_xform) {
        delete _xform;
        _xform = NULL;
    }
    [super dealloc];
}

- (void)afterCreated
{
    self.multipleTouchEnabled = YES;
    
    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    _graph->setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (_graph->beginPaint(context)) {
        [self draw:_graph];
        _graph->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
    gs->drawEllipse(NULL, Point2d(0,0), 10, 10);
}

@end
