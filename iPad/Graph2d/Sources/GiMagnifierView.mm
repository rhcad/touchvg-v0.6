// GiMagnifierView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiGraphView.h"
#include <iosgraph.h>
#include <mgshapes.h>

@implementation GiMagnifierView

@synthesize pointW = _pointW;
@synthesize scale = _scale;
@synthesize lockRedraw = _lockRedraw;

- (id)initWithFrame:(CGRect)frame graphView:(id<GiView>)gview
{
    self = [super initWithFrame:frame];
    if (self) {
        _graph = new GiGraphIos();
        _graph->xf.setViewScaleRange(1e-5, 50);
        _graph->gs.setMaxPenWidth(4);
        _gview = gview;
        _drawingDelegate = Nil;
        _shapeAdded = NULL;
        _cachedDraw = YES;
        _scale = 3;
        _lockRedraw = YES;
        _zooming = NO;
        
        self.contentMode = UIViewContentModeRedraw;
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
    [super dealloc];
}

- (BOOL)isActiveView
{
    return ([_drawingDelegate respondsToSelector:@selector(activeView)]
            && self == [_drawingDelegate performSelector:@selector(activeView)]);
}

- (void)setPointW:(CGPoint)ptw {
    _pointW = ptw;
    if (!_lockRedraw || [self isActiveView])
        [self setPointWandRedraw:ptw];
}

- (void)setPointWandRedraw:(CGPoint)ptw
{
    _pointW = ptw;
    Point2d ptd = Point2d(ptw.x, ptw.y) * _graph->xf.worldToDisplay();
    BOOL inside = CGRectContainsPoint(CGRectInset(self.bounds, 10, 10), CGPointMake(ptd.x, ptd.y));
    
    if (!inside) {
        _graph->xf.zoom(Point2d(ptw.x, ptw.y), _graph->xf.getViewScale());
    }
    _cachedDraw = YES;
    [self setNeedsDisplay];
}

- (void)zoomPan:(CGPoint)translation
{
    if (_graph->xf.zoomPan(translation.x, translation.y)) {
        _pointW = CGPointMake(_graph->xf.getCenterW().x, _graph->xf.getCenterW().y);
        _cachedDraw = YES;
        [self setNeedsDisplay];
    }
}

- (MgShapes*)shapes {
    return [_gview shapes];
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

- (void)setShapes:(MgShapes*)data {
}

- (void)setAnimating:(BOOL)animated {
    if (animated)
        _zooming |= 0x2;
    else
        _zooming &= ~0x2;
}

- (void)setDrawingDelegate:(id)d {
    _drawingDelegate = (UIResponder*)d;
}

- (void)regen {
    _graph->gs.clearCachedBitmap();
    _cachedDraw = YES;
    [self setNeedsDisplay];
}

- (void)redraw:(bool)fast {
    _cachedDraw = !fast;
    [self setNeedsDisplay];
}

- (BOOL)isZooming {
    return _zooming || [_gview isZooming];
}

- (void)drawRect:(CGRect)rect
{
    GiTransform& xf = _graph->xf;
    
    xf.setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    
    if (_scale < 1 && !self.hidden && ![_gview isZooming]) {    // 缩略视图，动态放缩时不regen
        CGSize gsize = [_gview ownerView].bounds.size;
        Box2d rcw(Box2d(0, 0, gsize.width, gsize.height) * [_gview xform]->displayToWorld());
        Box2d rcd(rcw * xf.worldToDisplay());       // 实际图形视图在本视图中的位置
        
        if (rcd.width() < self.bounds.size.width && rcd.height() < self.bounds.size.height) {
            if (!CGRectContainsRect(self.bounds,    // 出界则平移显示
                                    CGRectMake(rcd.xmin, rcd.ymin, rcd.width(), rcd.height()))) {
                xf.zoomPan(CGRectGetMidX(self.bounds) - rcd.center().x,
                           CGRectGetMidY(self.bounds) - rcd.center().y);
            }
        }
    }
    if (![_gview isZooming]) {                      // 同步显示比例，动态放缩时除外
        xf.zoom(xf.getCenterW(), [_gview xform]->getViewScale() * _scale);
    }
    
    GiCanvasIos &cv = _graph->canvas;
    GiGraphics &gs = _graph->gs;

    if (cv.beginPaint(UIGraphicsGetCurrentContext(), [self isZooming])) // 在当前画布上准备绘图
    {
        if (!cv.drawCachedBitmap()) {               // 显示上次保存的缓冲图
            [self draw:&gs];                        // 不行则重新显示所有图形
            if (![self isZooming])                  // 动态放缩时不保存显示内容
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
    _cachedDraw = NO;
    [self setNeedsDisplay];
}

- (BOOL)draw:(GiGraphics*)gs
{
    int count = 0;
    if ([_gview shapes]) {
        count = [_gview shapes]->draw(*gs);
    }
    return count > 0;
}

- (void)dynDraw:(GiGraphics*)gs
{
    BOOL inactive = ![self isActiveView];
    BOOL locked = _lockRedraw && inactive;
    
    if (!locked && [_drawingDelegate respondsToSelector:@selector(dynDraw:)]) {
        [_drawingDelegate performSelector:@selector(dynDraw:) withObject:self];
    }
    
    bool antiAlias = gs->isAntiAliasMode();
    gs->setAntiAliasMode(false);
    
    if (_scale < 1) {
        GiContext ctx(0, GiColor(64, 64, 64, 172), kLineDot);
        UIView *v = [_gview ownerView];
        Box2d rect(Box2d(0, 0, v.bounds.size.width, v.bounds.size.height) * [_gview xform]->displayToWorld());
        gs->drawRect(&ctx, rect, false);
    }
    else if (inactive) {
        GiContext ctx(0, GiColor(64, 64, 64, 172));
        Point2d ptd(Point2d(_pointW.x, _pointW.y) * _graph->xf.worldToDisplay());
        gs->rawLine(&ctx, ptd.x - 20, ptd.y, ptd.x + 20, ptd.y);
        gs->rawLine(&ctx, ptd.x, ptd.y - 20, ptd.x, ptd.y + 20);
    }
    
    gs->setAntiAliasMode(antiAlias);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [_drawingDelegate touchesBegan:touches withEvent:event];    // for GiCommandController
    [super touchesBegan:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [_drawingDelegate touchesCancelled:touches withEvent:event];    // for GiCommandController
    [super touchesCancelled:touches withEvent:event];
}

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (sender.view == self) {
        if (sender.state == UIGestureRecognizerStateBegan) {
            _zooming = YES;
        }
        else if (sender.state == UIGestureRecognizerStateChanged) {
            [self zoomPan:[sender translationInView:sender.view]];
            [sender setTranslation:CGPointZero inView:sender.view];
        }
        else {
            _zooming = NO;
            [self redraw:NO];
        }
    }
    
    return sender.view == self;
}

- (BOOL)automoveSuperview:(CGPoint)point fromView:(UIView*)view
{
    CGPoint ptzoom = [self.superview convertPoint:point fromView:view];
    CGRect mainBounds = [view convertRect:view.bounds toView:self.superview.superview];
    BOOL moved = NO;
    
    if (CGRectContainsRect(mainBounds, self.superview.frame)    // 放大镜视图在实际绘图视图内
        && CGRectContainsPoint(CGRectInset(self.superview.bounds, -20, -20), ptzoom)) // 进入放大镜视图
    {
        CGPoint cen;        // 本视图的上级视图的中心点，为view的视图坐标系
        
        if (point.x < view.bounds.size.width / 2) {             // 移到view的右侧
            cen.x = view.bounds.size.width - self.superview.frame.size.width / 2 - 10;
        }
        else {
            cen.x = self.superview.frame.size.width / 2 + 10;   // 移到view的左侧
        }
        if (point.y < view.bounds.size.height / 2) {            // 移到view的下侧
            cen.y = view.bounds.size.height - self.superview.frame.size.height / 2 - 10;
        }
        else {
            cen.y = self.superview.frame.size.height / 2 + 10;  // 移到view的上侧
        }
        
        moved = YES;
        self.superview.center = [self.superview.superview convertPoint:cen fromView:view];
    }
    
    return moved;
}

@end
