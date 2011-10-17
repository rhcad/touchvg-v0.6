// GiGraphView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include "GiGraphIos.h"

@implementation GiGraphView(ShapeProvider)

- (void*)getFirstShape:(void**)it { return NULL; }
- (void*)getNextShape:(void**)it { return NULL; }
- (BOX2D)getShapeExtent:(void*)shape { return Box2d(); }
- (void)drawShape:(void*)shape graphics:(GiGraphics*)gs context:(const GiContext *)ctx {}
- (double)hitTest:(void*)shape limits:(const BOX2D*)box { return DBL_MAX; }

@end

@interface GiGraphView(Zooming)

- (BOOL)dynZooming:(UIPinchGestureRecognizer *)sender;
- (BOOL)dynPanning:(UIPanGestureRecognizer *)sender;
- (BOOL)switchZoomed:(UIGestureRecognizer *)sender;

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
    _drawingDelegate = Nil;
    _viewMode = GiViewModeSelect;
    _zooming = NO;
    _doubleZoomed = NO;

    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _xform->setViewScaleRange(0.01, 20.0);
    _xform->zoomTo(Point2d(0,0));
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
            if ([_drawingDelegate respondsToSelector:@selector(dynDraw)]) {
                [_drawingDelegate performSelector:@selector(dynDraw)];
            }
        }
        
        gs->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
    Box2d clip(gs->getClipModel());
    Box2d extent;
    void *it, *shape;
    
    for (shape = [self getFirstShape:&it]; shape; shape = [self getNextShape:&it]) {
        Box2d box = [self getShapeExtent:shape];
        extent.unionWith(box);
        if (clip.isIntersect(box)) {
            [self drawShape:shape graphics:gs context:NULL];
        }
    }
    
    GiContext context(0, GiColor(128, 128, 128, 150), kLineDot);
    gs->drawRect(&context, extent);
}

- (void)dynDraw:(GiGraphics*)gs
{
}

- (void)setDrawingDelegate:(id)d
{
    _drawingDelegate = d;
}

- (void)setAnimating:(BOOL)animated
{
    if (animated)
        _zooming |= 0x2;
    else
        _zooming &= ~0x2;
}

- (void)setViewMode:(GiViewMode)mode
{
    if (mode >= GiViewModeView && mode < GiViewModeMax) {
        _viewMode = mode;
        _lastPoint = _firstPoint;
    }
}

- (BOOL)undoMotion
{
    return NO;
}

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    return [self dynZooming:sender];
}

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    return [self dynPanning:sender];
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    return NO;
}

- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    return [self switchZoomed:sender];
}

- (BOOL)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    return NO;
}

@end

@implementation GiGraphView(Zooming)

- (BOOL)dynZooming:(UIPinchGestureRecognizer *)sender
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
    return YES;
}

- (BOOL)dynPanning:(UIPanGestureRecognizer *)sender
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
    return YES;
}

- (BOOL)switchZoomed:(UIGestureRecognizer *)sender
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
    
    return YES;
}

@end
