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
    [self addGestureRecognizer:oneFingersTwoTaps];
    [oneFingersTwoTaps release];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    Point2d centerW;
    _xform->getZoomValue(centerW, _lastViewScale);
    _lastCenterX = centerW.x;
    _lastCenterY = centerW.y;
}

- (void)twoFingerPinch:(UIPinchGestureRecognizer *)recognizer
{
    CGPoint point = [recognizer locationInView:self];
    POINT at = { point.x, point.y };
    double factor = (recognizer.scale - 1) * 0.75;
    
    _xform->zoom(Point2d(_lastCenterX, _lastCenterY), _lastViewScale);
    _xform->zoomByFactor(factor, &at);
    [self setNeedsDisplay];
}

- (void)twoFingerPan:(UIPanGestureRecognizer *)recognizer
{
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)recognizer
{
    CGPoint translation = [recognizer translationInView:self];
    
    _xform->zoom(Point2d(_lastCenterX, _lastCenterY), _lastViewScale);
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
