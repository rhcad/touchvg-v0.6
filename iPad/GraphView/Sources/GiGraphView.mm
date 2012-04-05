// GiGraphView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include "GiGraphIos.h"
#include <Graph2d/mgshapes.h>

@interface GiGraphView(Zooming)

- (BOOL)dynZooming:(UIPinchGestureRecognizer *)sender;
- (BOOL)dynPanning:(UIPanGestureRecognizer *)sender;
- (BOOL)switchZoomed:(UIGestureRecognizer *)sender;

@end

@implementation GiGraphView

@synthesize shapes = _shapes;
@synthesize xform = _xform;
@synthesize graph = _graph;
@synthesize enableZoom = _enableZoom;
@synthesize zooming = _zooming;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _shapes = NULL; // need to set by the subclass
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
        _shapes = NULL; // need to set by the subclass
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
    _zooming = NO;
    _doubleZoomed = NO;
    _enableZoom = YES;
    
    CGFloat scrscale = [[UIScreen mainScreen] scale];
    GiGraphIos::setScreenDpi(mgRound(160 * scrscale));

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
            if ([_drawingDelegate respondsToSelector:@selector(dynDraw:)]) {
                [_drawingDelegate performSelector:@selector(dynDraw:) withObject:self];
            }
        }
        
        gs->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
    if (_shapes)
        _shapes->draw(*gs);
}

- (MgShapes*)getShapes
{
    return _shapes;
}

- (GiTransform*)getXform
{
    return _xform;
}

- (GiGraphics*)getGraph
{
    return _graph;
}

- (UIView*)getOwnerView {
    return self;
}

- (void)setShapes:(MgShapes*)data
{
    _shapes = data;
    [self regen];
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

- (void)regen
{
    _graph->clearCachedBitmap();
    [self setNeedsDisplay];
}

- (void)redraw
{
    [self setNeedsDisplay];
}

- (BOOL)isZooming {
    return !!_zooming;
}

- (void)dynDraw:(GiGraphics*)gs
{
}

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    return _enableZoom && [self dynZooming:sender];
}

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    return _enableZoom && [self dynPanning:sender];
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    return _enableZoom && [self dynPanning:sender];
}

- (BOOL)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    return _enableZoom && [self switchZoomed:sender];
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
    
    if ([_drawingDelegate respondsToSelector:@selector(afterZoomed:)]) {
        [_drawingDelegate performSelector:@selector(afterZoomed:) withObject:self];
    }

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
    
    if ([_drawingDelegate respondsToSelector:@selector(afterZoomed:)]) {
        [_drawingDelegate performSelector:@selector(afterZoomed:) withObject:self];
    }
    
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
    }
    else                // zoomin at the point
    {
        Point2d centerW;
        _xform->getZoomValue(centerW, _scaleBeforeDbl);
        _centerBeforeDbl = CGPointMake(centerW.x, centerW.y);
        
        if (_xform->zoomByFactor(2, &at)) {
            _xform->zoomTo(Point2d(point.x, point.y) * _xform->displayToWorld());
            _doubleZoomed = YES;
        }
    }
    
    if ([_drawingDelegate respondsToSelector:@selector(afterZoomed:)]) {
        [_drawingDelegate performSelector:@selector(afterZoomed:) withObject:self];
    }
    
    return YES;
}

@end
