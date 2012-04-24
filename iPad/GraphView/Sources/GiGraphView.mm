// GiGraphView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchdraw

#import "GiGraphView.h"
#include <graphios.h>
#include <mgshapes.h>

@interface GiGraphView(Zooming)

- (BOOL)dynZooming:(UIPinchGestureRecognizer *)sender;
- (BOOL)dynPanning:(UIPanGestureRecognizer *)sender;
- (BOOL)switchZoomed:(UIGestureRecognizer *)sender;

@end

@implementation GiGraphView

@synthesize enableZoom = _enableZoom;
@synthesize zooming = _zooming;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _shapes = NULL; // need to set by the subclass
        _graph = NULL;
        [self afterCreated];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _shapes = NULL; // need to set by the subclass
        _graph = NULL;
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
    [super dealloc];
}

- (void)afterCreated
{
	CGFloat scrscale = [[UIScreen mainScreen] scale];
    GiCanvasIos::setScreenDpi(160 * scrscale);
    
    if (!_graph) {
        _graph = new GiGraphIos();
    }

    _graph->xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _graph->xf.setViewScaleRange(0.01, 20.0);
    _graph->xf.zoomTo(Point2d(0,0));
    
    self.contentMode = UIViewContentModeRedraw;
    _drawingDelegate = Nil;
    _zooming = NO;
    _doubleZoomed = NO;
    _enableZoom = YES;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    GiGraphics& gs = _graph->gs;
    
    _graph->xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    gs.setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (_graph->canvas.beginPaint(context, true, !!_zooming))
    {
        if (_zooming) {
            [self draw:&gs];
        }
        else {
            if (!gs.drawCachedBitmap(0, 0)) {
                [self draw:&gs];
                gs.saveCachedBitmap();
            }
            [self dynDraw:&gs];
            if ([_drawingDelegate respondsToSelector:@selector(dynDraw:)]) {
                [_drawingDelegate performSelector:@selector(dynDraw:) withObject:self];
            }
        }
        
        _graph->canvas.endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
    if (_shapes)
        _shapes->draw(*gs);
}

- (MgShapes*)shapes
{
    return _shapes;
}

- (GiTransform*)xform
{
    return &_graph->xf;
}

- (GiGraphics*)graph
{
    return &_graph->gs;
}

- (UIView*)ownerView {
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
    _graph->gs.clearCachedBitmap();
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
        _graph->xf.getZoomValue(centerW, _lastViewScale);
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
    
    Point2d at (_firstPoint.x, _firstPoint.y );
    _graph->xf.zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);
    _graph->xf.zoomByFactor(sender.scale - 1, &at);
    
    CGPoint point = ([sender numberOfTouches] < 2) ? _lastPoint : [sender locationInView:self];
    _graph->xf.zoomPan(point.x - _firstPoint.x, point.y - _firstPoint.y);
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
        _graph->xf.getZoomValue(centerW, _lastViewScale);
        _lastCenterW = CGPointMake(centerW.x, centerW.y);
        _zooming = YES;
    }
    else
    {
        _zooming = (sender.state == UIGestureRecognizerStateChanged);
    }
    
    CGPoint translation = [sender translationInView:self];
    _graph->xf.zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);
    _graph->xf.zoomPan(translation.x, translation.y);
    
    if ([_drawingDelegate respondsToSelector:@selector(afterZoomed:)]) {
        [_drawingDelegate performSelector:@selector(afterZoomed:) withObject:self];
    }
    
    return YES;
}

- (BOOL)switchZoomed:(UIGestureRecognizer *)sender
{
    CGPoint point = [sender locationInView:self];
    Point2d at (_firstPoint.x, _firstPoint.y );
    
    if (_doubleZoomed)  // restore zoom scale
    {
        _doubleZoomed = NO;
        _graph->xf.zoom(Point2d(_centerBeforeDbl.x, _centerBeforeDbl.y), _scaleBeforeDbl);
    }
    else                // zoomin at the point
    {
        Point2d centerW;
        _graph->xf.getZoomValue(centerW, _scaleBeforeDbl);
        _centerBeforeDbl = CGPointMake(centerW.x, centerW.y);
        
        if (_graph->xf.zoomByFactor(2, &at)) {
            _graph->xf.zoomTo(Point2d(point.x, point.y) * _graph->xf.displayToWorld());
            _doubleZoomed = YES;
        }
    }
    
    if ([_drawingDelegate respondsToSelector:@selector(afterZoomed:)]) {
        [_drawingDelegate performSelector:@selector(afterZoomed:) withObject:self];
    }
    
    return YES;
}

@end
