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
	CGFloat scrw = CGRectGetWidth([UIScreen mainScreen].bounds);
    GiCanvasIos::setScreenDpi(scrw > 1000 ? 132 : 163); // iPad or iPhone
    
    if (!_graph) {
        _graph = new GiGraphIos();
    }

    _graph->xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _graph->xf.setViewScaleRange(0.01, 20.0);
    _graph->xf.zoomTo(Point2d(0,0));
    
    self.contentMode = UIViewContentModeRedraw;
    self.multipleTouchEnabled = YES;
    _drawingDelegate = Nil;
    _shapeAdded = NULL;
    _zooming = NO;
    _doubleZoomed = NO;
    _enableZoom = YES;
}

- (void)drawRect:(CGRect)rect
{
    GiCanvasIos &cv = _graph->canvas;
    GiGraphics &gs = _graph->gs;
    
    _graph->xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    cv.setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (cv.beginPaint(UIGraphicsGetCurrentContext(), !!_zooming))   // 在当前画布上准备绘图
    {
        if (!cv.drawCachedBitmap()) {               // 显示上次保存的缓冲图
            [self draw:&gs];                        // 不行则重新显示所有图形
            if (!_zooming)                          // 动态放缩时不保存显示内容
                cv.saveCachedBitmap();              // 保存显示缓冲图，下次就不重新显示图形
        }
        else if (_shapeAdded) {                     // 在缓冲图上显示新的图形
            _shapeAdded->draw(gs);
            cv.saveCachedBitmap();                  // 更新缓冲图
        }
        
        [self dynDraw:&gs];                         // 显示动态临时图形
        
        cv.endPaint();                              // 显示完成后贴到视图画布上
        _shapeAdded = NULL;
    }
}

- (void)shapeAdded:(MgShape*)shape
{
    _shapeAdded = shape;
    [self setNeedsDisplay];
}

- (void)draw:(GiGraphics*)gs
{
    if (_shapes) {
        _shapes->draw(*gs);
    }
}

- (void)dynDraw:(GiGraphics*)gs
{
    if ([_drawingDelegate respondsToSelector:@selector(dynDraw:)]) {
        [_drawingDelegate performSelector:@selector(dynDraw:) withObject:self];
    }
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
