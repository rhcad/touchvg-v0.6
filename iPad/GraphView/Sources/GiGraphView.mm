// GiGraphView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include "GiGraphIos.h"

@interface GiGraphView(GestureRecognizer)

- (void)addGestureRecognizers;
- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@interface GiGraphView(Zooming)

- (void)dynZooming:(UIPinchGestureRecognizer *)sender;
- (void)dynPanning:(UIPanGestureRecognizer *)sender;
- (void)switchZoomed:(UIGestureRecognizer *)sender;

@end

@implementation GiGraphView

@synthesize xform = _xform;
@synthesize graph = _graph;
@synthesize viewMode = _viewMode;
@synthesize zooming = _zooming;

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
    self.contentMode = UIViewContentModeRedraw;
    _viewMode = GiViewModeView;
    _zooming = NO;
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
    
    if (gs->beginPaint(context, true, !!_zooming))
    {
        if (_zooming) {
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

- (void)setAnimating:(BOOL)animated
{
    if (animated)
        _zooming |= 0x2;
    else
        _zooming &= ~0x2;
}

- (void)shakeMotion
{
}

- (void)setViewMode:(GiViewMode)mode
{
    if (mode >= GiViewModeView && mode < GiViewModeMax) {
        _viewMode = mode;
        _lastPoint = _firstPoint;
    }
}

@end

@implementation GiGraphView(GestureRecognizer)

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
    
    UITapGestureRecognizer *oneFingerOneTap =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [self addGestureRecognizer:oneFingerOneTap];
    [oneFingerOneTap release];
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    [self dynZooming:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    [self dynPanning:sender];
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    [self switchZoomed:sender];
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    self.viewMode = (GiViewMode)((_viewMode + 1) % GiViewModeMax);
}

@end

@implementation GiGraphView(Zooming)

- (void)dynZooming:(UIPinchGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        Point2d centerW;
        _xform->getZoomValue(centerW, _lastViewScale);
        _lastCenterW = CGPointMake(centerW.x, centerW.y);
        
        _firstPoint = [sender locationInView:self];
        _lastPoint = _firstPoint;
        _zooming = YES;
        _doubleZoomed = NO;
    }
    else
    {
        _zooming = (sender.state == UIGestureRecognizerStateChanged);
    }
    
    POINT at = { _firstPoint.x, _firstPoint.y };
    _xform->zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);
    _xform->zoomByFactor(sender.scale - 1, &at);
    
    CGPoint point = ([sender numberOfTouches] < 2) ? _lastPoint : [sender locationInView:self];
    _xform->zoomPan(point.x - _firstPoint.x, point.y - _firstPoint.y);
    _lastPoint = point;
    
    [self setNeedsDisplay];
}

- (void)dynPanning:(UIPanGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        Point2d centerW;
        _xform->getZoomValue(centerW, _lastViewScale);
        _lastCenterW = CGPointMake(centerW.x, centerW.y);
        _zooming = YES;
    }
    else
    {
        _zooming = (sender.state == UIGestureRecognizerStateChanged);
    }
    
    CGPoint translation = [sender translationInView:self];
    _xform->zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);
    _xform->zoomPan(translation.x, translation.y);
    
    [self setNeedsDisplay];
}

- (void)switchZoomed:(UIGestureRecognizer *)sender
{
    CGPoint point = [sender locationInView:self];
    POINT at = { point.x, point.y };
    
    if (_doubleZoomed)  // restore zoom scale
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

@end
