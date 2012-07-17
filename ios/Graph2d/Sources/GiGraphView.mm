// GiGraphView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiGraphView.h"
#include <iosgraph.h>
#include <mgshapes.h>

@interface GiGraphView(Zooming)

- (void)saveZoomScale:(CGPoint)point;
- (BOOL)dynZoom:(UIGestureRecognizer *)sender point:(CGPoint)point scale:(float)scale;
- (BOOL)dynZooming:(UIPinchGestureRecognizer *)sender;
- (BOOL)dynPanning:(UIPanGestureRecognizer *)sender;
- (BOOL)switchZoomed:(UIGestureRecognizer *)sender;
- (void)shapesLocked:(MgShapes*)sp locked:(BOOL)locked;

@end

GiColor giFromUIColor(UIColor *color)
{
    return color ? giFromCGColor(color.CGColor) : GiColor::Invalid();
}

static void onShapesLocked(MgShapes* sp, void* obj, bool locked)
{
    GiGraphView* view = (GiGraphView*)obj;
    [view shapesLocked:sp locked:locked];
}

@implementation GiGraphView

@synthesize enableZoom = _enableZoom;
@synthesize zooming = _zooming;
@synthesize shapeAdded = _shapeAdded;
@synthesize bufferEnabled;

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
    MgShapesLock::unregisterObserver(onShapesLocked, self);
    
    if (_playShapes) {
        _playShapes->release();
        _playShapes = NULL;
    }
    if (_bkImg) {
        [_bkImg release];
        _bkImg = nil;
    }
    if (_graph) {
        delete _graph;
        _graph = NULL;
    }
    [super dealloc];
}

- (void)afterCreated
{
	CGFloat scrw = CGRectGetWidth([UIScreen mainScreen].bounds);
    GiCanvasIos::setScreenDpi(scrw > 700 ? 132 : 163, [UIScreen mainScreen].scale);
    
    if (!_graph) {
        _graph = new GiGraphIos();
    }

    _graph->xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _graph->xf.setViewScaleRange(0.01, 20.0);
    _graph->xf.zoomTo(Point2d(0,0));
    
    self.contentMode = UIViewContentModeRedraw;
    self.multipleTouchEnabled = YES;
    self.opaque = NO;
    self.clearsContextBeforeDrawing = NO;
    
    _drawingDelegate = Nil;
    _shapeAdded = NULL;
    _playShapes = NULL;
    _buffered = 0x11;
    _scaleReaded = NO;
    _zooming = NO;
    _doubleZoomed = NO;
    _enableZoom = YES;
    
    MgShapesLock::registerObserver(onShapesLocked, self);
}

- (CGImageRef)cachedBitmap:(BOOL)invert
{
    return _graph->canvas.cachedBitmap(!!invert);
}

- (void)drawRect:(CGRect)rect
{
    GiCanvasIos &cv = _graph->canvas;
    GiGraphics &gs = _graph->gs;
    bool buffered = (_buffered & 0x10) && ((_buffered & 1) || !cv.hasCachedBitmap());
    bool nextDraw = false;
    MgShape* tmpAdded = _shapeAdded;
    
    _graph->xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    if (_shapes && !_scaleReaded) {
        _scaleReaded = YES;
        _graph->xf.setModelTransform(_shapes->modelTransform());
        _graph->xf.zoom(_shapes->getViewCenterW(), _shapes->getViewScale());
    }
    
    if (cv.beginPaint(UIGraphicsGetCurrentContext(), // 在当前画布上准备绘图
                      !!_zooming, buffered))          // iPad3上不用缓冲更快
    {
        if (!cv.drawCachedBitmap()) {               // 显示上次保存的缓冲图
            if ([self draw:&gs]) {                  // 不行则重新显示所有图形
                if (!_zooming)                      // 动态放缩时不保存显示内容
                    cv.saveCachedBitmap();          // 保存显示缓冲图，下次就不重新显示图形
                tmpAdded = NULL;
            }
            else {
                nextDraw = true;
            }
        }
        else if (_shapeAdded) {                     // 在缓冲图上显示新的图形
            _shapeAdded->draw(gs);
            cv.saveCachedBitmap();                  // 更新缓冲图
            tmpAdded = NULL;
        }
        
        nextDraw = ![self dynDraw:&gs] || nextDraw; // 显示动态临时图形
        
        cv.endPaint();                              // 显示完成后贴到视图画布上
    }
    
    if (_shapeAdded != tmpAdded) {
        MgDynShapeLock lockdyn;
        _shapeAdded = tmpAdded;
    }
    if (nextDraw) {
        [self setNeedsDisplay];
    }
}

- (void)shapeAdded:(MgShape*)shape
{
    if (_shapeAdded || !shape) {
        _buffered |= 1;
        [self regen];
    }
    else {
        _shapeAdded = shape;
        _buffered &= ~1;
        [self setNeedsDisplay];
    }
}

- (BOOL)draw:(GiGraphics*)gs
{
    BOOL ret = YES;
    
    if (_playShapes) {
        MgShapesLock locker(_playShapes, MgShapesLock::ReadOnly, 50);
        ret = locker.locked();
        if (ret) {
            _playShapes->draw(*gs);
        }
    }
    else if (_shapes) {
        _shapes->draw(*gs);
    }
    
    return ret;
}

- (BOOL)dynDraw:(GiGraphics*)gs
{
    BOOL ret = YES;
    if ([_drawingDelegate respondsToSelector:@selector(dynDraw:)]) {
        ret = !![_drawingDelegate performSelector:@selector(dynDraw:) withObject:self];
    }
    return ret;
}

- (MgShapes*)getPlayShapes:(BOOL)clear
{
    if (clear) {
        MgShapesLock locker(_playShapes, MgShapesLock::Edit, 1000);
        if (locker.shapes) {
            locker.shapes->release();
            locker.shapes = NULL;
        }
        _playShapes = NULL;
    }
    else if (!_playShapes) {
        MgShapesLock locker(_shapes, MgShapesLock::ReadOnly);
        _playShapes = (MgShapes*)_shapes->clone();
    }
    
    return _playShapes;
}

- (MgShapes*)shapes {
    return _shapes;
}

- (GiTransform*)xform {
    return &_graph->xf;
}

- (GiGraphics*)graph {
    return &_graph->gs;
}

- (UIView*)ownerView {
    return self;
}

- (void)setShapes:(MgShapes*)data
{
    [self shapesLocked:_shapes locked:YES];
    _shapes = data;
    [self regen];
}

- (void)setDrawingDelegate:(id)d {
    _drawingDelegate = d;
}

- (void)setAnimating:(BOOL)animated {
    if (animated)
        _zooming |= 0x2;
    else
        _zooming &= ~0x2;
}

- (void)regen {
    _graph->gs.clearCachedBitmap();
    _buffered |= 1;
    [self setNeedsDisplay];
}

- (void)redraw:(bool)fast
{
    _buffered = fast ? (_buffered & ~1) : (_buffered | 1);
    [self setNeedsDisplay];
}

- (BOOL)bufferEnabled {
    return (_buffered & 0x10) != 0;
}

- (void)setBufferEnabled:(BOOL)enabled {
    _buffered = enabled ? (_buffered | 0x10) : (_buffered & ~0x10);
}

- (BOOL)isZooming {
    return !!_zooming;
}

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender {
    return _enableZoom && [self dynZooming:sender];
}

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender {
    return _enableZoom && [self dynPanning:sender];
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender {
    return _enableZoom && [self dynPanning:sender];
}

- (BOOL)twoFingersTwoTaps:(UITapGestureRecognizer *)sender {
    return _enableZoom && [self switchZoomed:sender];
}

- (void)setZoomCallback:(id<GiZoomCallback>)obj {
    _zoomCallback = obj;
}

- (void)setBackgroundImage:(UIImage*)image {
    if (image) {
        [image retain];
    }
    if (_bkImg) {
        [_bkImg release];
    }
    _bkImg = image;
}

- (void)setModelTransform:(CGAffineTransform)xf
{
    Matrix2d mat(xf.a, xf.b, xf.c, xf.d, xf.tx, xf.ty);
    
    _graph->xf.setModelTransform(mat);
    if (_shapes) {
        _shapes->modelTransform() = mat;
        [self regen];
    }
}

- (void)zoomModel:(CGRect)pageRect to:(CGRect)pxRect save:(BOOL)save
{
    Box2d fromrc(pageRect.origin.x, pageRect.origin.y, 
               pageRect.origin.x + pageRect.size.width, pageRect.origin.y + pageRect.size.height);
    RECT2D torc = { pxRect.origin.x, pxRect.origin.y, 
        pxRect.origin.x + pxRect.size.width, pxRect.origin.y + pxRect.size.height };
    
    fromrc *= _graph->xf.modelToWorld();
    _graph->xf.zoomTo(fromrc, &torc);
    if (save) {
        _shapes->setZoomState(_graph->xf.getViewScale(), _graph->xf.getCenterW());
        _scaleReaded = YES;
    }
}

- (void)zoomPan:(CGPoint)offset
{
    _graph->xf.zoomPan(-offset.x, -offset.y);
    [self regen];
}

- (float)getViewScale {
    return _graph->xf.getViewScale();
}

- (CGRect)getModelRect {
    Box2d rect(0, 0, _graph->xf.getWidth(), _graph->xf.getHeight());
    rect *= _graph->xf.displayToModel();
    return CGRectMake(rect.xmin, rect.ymin, rect.width(), rect.height());
}

@end

@implementation GiGraphView(Zooming)

- (void)saveZoomScale:(CGPoint)point
{
    Point2d centerW;
    _graph->xf.getZoomValue(centerW, _lastViewScale);
    _lastCenterW = CGPointMake(centerW.x, centerW.y);
    _firstPoint = point;
}

- (BOOL)dynZoom:(UIGestureRecognizer *)sender point:(CGPoint)point scale:(float)scale
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        [self saveZoomScale:point];
        _zooming = YES;
    }
    else {
        _zooming = (sender.state == UIGestureRecognizerStateChanged);
    }
    
    if (_zooming) {
        Point2d at (_firstPoint.x, _firstPoint.y );
        _graph->xf.zoom(Point2d(_lastCenterW.x, _lastCenterW.y), _lastViewScale);   // 先恢复
        _graph->xf.zoomByFactor(scale - 1, &at);                        // 以起始点为中心放缩显示
        _graph->xf.zoomPan(point.x - _firstPoint.x, point.y - _firstPoint.y);   // 平移到当前点
    }
    
    if ([_drawingDelegate respondsToSelector:@selector(afterZoomed:)]) {
        [_drawingDelegate performSelector:@selector(afterZoomed:) withObject:self];
    }
    
    return YES;
}

- (BOOL)dynZooming:(UIPinchGestureRecognizer *)sender
{
    _doubleZoomed = NO;
    return [self dynZoom:sender point:[sender locationInView:self] scale:sender.scale];
}

- (BOOL)dynPanning:(UIPanGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {        // 开始时没有放缩
        _lastDistPan = 0;                                       // 待记录双指距离
        _lastScalePan = 1;
    }
    
    CGPoint point = [sender locationInView:self];               // 默认是单指位置
    
    if ([sender numberOfTouches] > 1) {
        CGPoint p1 = [sender locationOfTouch:0 inView:sender.view];
        CGPoint p2 = [sender locationOfTouch:1 inView:sender.view];
        float dist = sqrtf((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
        
        point = CGPointMake((p2.x + p1.x) / 2, (p2.y + p1.y) / 2);  // 双指中点
        if (_lastDistPan < 0.01f) {                             // 还没放缩
            [self saveZoomScale:point];                         // 记下开始时的放缩比例
            _lastDistPan = dist;                                // 双指起始距离
            _lastScalePan = 1;
        }
        else {
            _lastScalePan = dist / _lastDistPan;                // 放缩显示
        }
    }
    else if (sender.state == UIGestureRecognizerStateChanged
             && _lastDistPan > 0.01f) {                         // 由放缩显示转为平移显示
        [self saveZoomScale:point];                             // 重新记下开始时的位置
        _lastDistPan = 0;                                       // 没有放缩，待记录
        _lastScalePan = 1;
        
        return YES;                                             // 双指变单指时不放缩平移
    }
    
    return [self dynZoom:sender point:point scale:_lastScalePan];
}

- (BOOL)switchZoomed:(UIGestureRecognizer *)sender
{
    CGPoint point = [sender locationInView:self];
    Point2d at (_firstPoint.x, _firstPoint.y );
    
    if (_doubleZoomed) {    // restore zoom scale
        _doubleZoomed = NO;
        _graph->xf.zoom(Point2d(_centerBeforeDbl.x, _centerBeforeDbl.y), _scaleBeforeDbl);
    }
    else {                  // zoomin at the point
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

- (void)shapesLocked:(MgShapes*)sp locked:(BOOL)locked
{
    if (!locked && sp == _shapes) {
        if ([_drawingDelegate respondsToSelector:@selector(afterShapeChanged)]) {
            [_drawingDelegate performSelector:@selector(afterShapeChanged)];
        }
    }
}

@end
