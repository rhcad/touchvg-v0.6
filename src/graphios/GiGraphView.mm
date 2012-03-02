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
        self.multipleTouchEnabled = YES;
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _xform = new GiTransform();
        _graph = new GiGraphIos(*_xform);
        self.multipleTouchEnabled = YES;
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

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    if (_graph->beginPaint(context)) {
        _graph->endPaint();
    }
}

@end
