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
    _fastDraw = NO;
    _doubleZoomed = NO;

    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _xform->setViewScaleRange(0.01, 20.0);
    _xform->zoomTo(Point2d(0,0));

    [self addGestureRecognizers];
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    GiGraphIos* gs = (GiGraphIos*)_graph;
    
    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _graph->setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (gs->beginPaint(context, true, _fastDraw))
    {
        if (_fastDraw) {
            [self draw:gs];
        }
        else {
            if (!gs->drawCachedBitmap(0, 0)) {
                [self draw:gs];
                gs->saveCachedBitmap();
            }
            [self dynDraw:gs];
        }
        
        gs->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
}

- (void)dynDraw:(GiGraphics*)gs
{
}

- (void)addGestureRecognizers
{
    UIPinchGestureRecognizer *twoFingersPinch =
        [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    [self addGestureRecognizer:twoFingersPinch];
    [twoFingersPinch release];

    UIPanGestureRecognizer *twoFingersPan =
        [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    [twoFingersPan setMinimumNumberOfTouches:2];
    [twoFingersPan setMaximumNumberOfTouches:2];
    [self addGestureRecognizer:twoFingersPan];
    [twoFingersPan release];

    UIPanGestureRecognizer *oneFingerPan =
        [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    [oneFingerPan setMaximumNumberOfTouches:1];
    [self addGestureRecognizer:oneFingerPan];
    [oneFingerPan release];

    UITapGestureRecognizer *oneFingerTwoTaps =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    [oneFingerTwoTaps setNumberOfTapsRequired:2];
    [self addGestureRecognizer:oneFingerTwoTaps];
    [oneFingerTwoTaps release];
    
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    [twoFingersTwoTaps setNumberOfTapsRequired:2];
    [twoFingersTwoTaps setNumberOfTouchesRequired:2];
    [self addGestureRecognizer:twoFingersTwoTaps];
    [twoFingersTwoTaps release];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        Point2d centerW;
        _xform->getZoomValue(centerW, _lastViewScale);
        _lastCenterW = CGPointMake(centerW.x, centerW.y);
        _firstPoint = [sender locationInView:self];
        _lastPoint = _firstPoint;
        _fastDraw = YES;
        _doubleZoomed = NO;
    }
    else
    {
        _fastDraw = (sender.state == UIGestureRecognizerStateChanged);
    }
    
    POINT at = { _firstPoint.x, _firstPoint.y };
    _xform->zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);
    _xform->zoomByFactor(sender.scale - 1, &at);
    
    CGPoint point = ([sender numberOfTouches] < 2) ? _lastPoint : [sender locationInView:self];
    _xform->zoomPan(point.x - _firstPoint.x, point.y - _firstPoint.y);
    _lastPoint = point;
    
    [self setNeedsDisplay];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        Point2d centerW;
        _xform->getZoomValue(centerW, _lastViewScale);
        _lastCenterW = CGPointMake(centerW.x, centerW.y);
        _fastDraw = YES;
    }
    else
    {
        _fastDraw = (sender.state == UIGestureRecognizerStateChanged);
    }
    
    CGPoint translation = [sender translationInView:self];
    _xform->zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);
    _xform->zoomPan(translation.x, translation.y);
    
    [self setNeedsDisplay];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    [self twoFingersPan:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    CGPoint point = [sender locationInView:self];
    POINT at = { point.x, point.y };
    
    if (_doubleZoomed)  // restory zoom scale
    {
        _doubleZoomed = NO;
        _xform->zoom(Point2d(_centerBeforeDbl.x, _centerBeforeDbl.y), _scaleBeforeDbl);
        [self setNeedsDisplay];
    }
    else                // zoomin at the point
    {
        Point2d centerW;
        _xform->getZoomValue(centerW, _scaleBeforeDbl);
        _centerBeforeDbl = CGPointMake(centerW.x, centerW.y);
        
        if (_xform->zoomByFactor(2, &at)) {
            _xform->zoomTo(Point2d(point.x, point.y) * _xform->displayToWorld());
            _doubleZoomed = YES;
            [self setNeedsDisplay];
        }
    }
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
}

@end
