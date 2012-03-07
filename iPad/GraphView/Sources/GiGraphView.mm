// GiGraphView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: GPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include <GiGraphIos.h>

@implementation GiGraphView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _xform = new GiTransform(false);
        _graph = new GiGraphIos(*_xform);
        [self afterCreated];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _xform = new GiTransform(false);
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
    _xform->setViewScaleRange(0.01, 10.0);
    _xform->zoomTo(Point2d(0,0));

    [self addGestureRecognizers];
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _graph->setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (_graph->beginPaint(context))
    {
        [self draw:_graph];
        _graph->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
}

- (void)addGestureRecognizers
{
    UIPinchGestureRecognizer *twoFingerPinch =
        [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingerPinch:)];
    [self addGestureRecognizer:twoFingerPinch];
    [twoFingerPinch release];

    UIPanGestureRecognizer *twoFingerPan =
        [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingerPan:)];
    [twoFingerPan setMinimumNumberOfTouches:2];
    [twoFingerPan setMaximumNumberOfTouches:2];
    [self addGestureRecognizer:twoFingerPan];
    [twoFingerPan release];

    UIPanGestureRecognizer *oneFingerPan =
        [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    [oneFingerPan setMaximumNumberOfTouches:1];
    [self addGestureRecognizer:oneFingerPan];
    [oneFingerPan release];

    UITapGestureRecognizer *twoFingersTwoTaps =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps)];
    [twoFingersTwoTaps setNumberOfTapsRequired:2];
    [twoFingersTwoTaps setNumberOfTouchesRequired:2];
    [self addGestureRecognizer:twoFingersTwoTaps];
    [twoFingersTwoTaps release];

    UITapGestureRecognizer *oneFingersTwoTaps =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingersTwoTaps)];
    [oneFingersTwoTaps setNumberOfTapsRequired:2];
    [self addGestureRecognizer:oneFingersTwoTaps];
    [oneFingersTwoTaps release];
}

- (void)twoFingerPinch:(UIPinchGestureRecognizer *)sender
{
    if ([sender numberOfTouches] < 2)
        return;

    if (sender.state == UIGestureRecognizerStateBegan) {
        Point2d centerW;
        _xform->getZoomValue(centerW, _lastViewScale);
        _lastCenter = CGPointMake(centerW.x, centerW.y);
        _firstPoint = [sender locationInView:self];
    }

    POINT at = { _firstPoint.x, _firstPoint.y };
    _xform->zoom(Point2d(_lastCenter.x, _lastCenter.y), _lastViewScale);
    _xform->zoomByFactor(sender.scale - 1, &at);

    CGPoint point = [sender locationInView:self];
    _xform->zoomPan(point.x - _firstPoint.x, point.y - _firstPoint.y);
    
    [self setNeedsDisplay];
}

- (void)twoFingerPan:(UIPanGestureRecognizer *)sender
{
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        Point2d centerW;
        _xform->getZoomValue(centerW, _lastViewScale);
        _lastCenter = CGPointMake(centerW.x, centerW.y);
    }

    CGPoint translation = [sender translationInView:self];
    _xform->zoom(Point2d(_lastCenter.x, _lastCenter.y), _lastViewScale);
    _xform->zoomPan(translation.x, translation.y);
    
    [self setNeedsDisplay];
}

- (void)twoFingersTwoTaps
{
}

- (void)oneFingersTwoTaps
{
}

@end
