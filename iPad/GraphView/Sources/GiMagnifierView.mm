// GiMagnifierView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchdraw

#import "GiGraphView.h"
#include <GiCanvasIos.h>
#include <gigraph.h>
#include <mgshapes.h>

@implementation GiMagnifierView

@synthesize graph = _graph;
@synthesize pointW = _pointW;
@synthesize scale = _scale;
@synthesize lockRedraw = _lockRedraw;

- (id)initWithFrame:(CGRect)frame graphView:(id<GiView>)gview
{
    self = [super initWithFrame:frame];
    if (self) {
        _xform = new GiTransform();
        _graph = new GiGraphics(_xform);
        _canvas = new GiCanvasIos(_graph);
        _xform->setViewScaleRange(1e-5, 50);
        _graph->setMaxPenWidth(4);
        _gview = gview;
        _drawingDelegate = Nil;
        _scale = 3;
        _lockRedraw = YES;
        
        self.contentMode = UIViewContentModeRedraw;
    }
    return self;
}

- (void)dealloc
{
    if (_canvas) {
        delete _canvas;
        _canvas = NULL;
    }
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
    Point2d ptd = Point2d(ptw.x, ptw.y) * _xform->worldToDisplay();
    BOOL inside = CGRectContainsPoint(CGRectInset(self.bounds, 10, 10), CGPointMake(ptd.x, ptd.y));
    
    if (!inside) {
        _xform->zoom(Point2d(ptw.x, ptw.y), _xform->getViewScale());
    }
    [self setNeedsDisplay];
}

- (void)zoomPan:(CGPoint)translation
{
    if (_xform->zoomPan(translation.x, translation.y)) {
        _pointW = CGPointMake(_xform->getCenterW().x, _xform->getCenterW().y);
        [self setNeedsDisplay];
    }
}

- (MgShapes*)getShapes {
    return [_gview getShapes];
}

- (GiTransform*)getXform {
    return _xform;
}

- (GiGraphics*)getGraph {
    return _graph;
}

- (UIView*)getOwnerView {
    return self;
}

- (void)setShapes:(MgShapes*)data {
}

- (void)setAnimating:(BOOL)animated {
}

- (void)setDrawingDelegate:(id)d {
    _drawingDelegate = (UIResponder*)d;
}

- (void)regen {
    _graph->clearCachedBitmap();
    [self setNeedsDisplay];
    [_gview regen];
}

- (void)redraw {
    [self setNeedsDisplay];
    [_gview redraw];
}

- (BOOL)isZooming {
    return [_gview isZooming];
}

- (void)drawRect:(CGRect)rect
{
    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    
    if (_scale < 1 && ![_gview isZooming]) {            // 缩略视图，动态放缩时不regen
        CGSize gsize = [_gview getOwnerView].bounds.size;
        Box2d rectw(Box2d(0, 0, gsize.width, gsize.height) * [_gview getXform]->displayToWorld());
        Box2d rectd(rectw * _xform->worldToDisplay());  // 实际图形视图在本视图中的位置
        
        if (rectd.width() < self.bounds.size.width && rectd.height() < self.bounds.size.height) {
            if (!CGRectContainsRect(self.bounds,        // 出界则平移显示
                                    CGRectMake(rectd.xmin, rectd.ymin, rectd.width(), rectd.height()))) {
                _xform->zoomPan(CGRectGetMidX(self.bounds) - rectd.center().x,
                                CGRectGetMidY(self.bounds) - rectd.center().y);
            }
        }
    }
    if (![_gview isZooming]) {                          // 同步显示比例，动态放缩时除外
        _xform->zoom(_xform->getCenterW(), [_gview getXform]->getViewScale() * _scale);
    }
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    _graph->setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    if (_canvas->beginPaint(context, true, [self isZooming]))
    {
        if (!_graph->drawCachedBitmap(0, 0)) {
            [self draw:_graph];
            _graph->saveCachedBitmap();
        }
        [self dynDraw:_graph];
        _canvas->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
    if ([_gview getShapes]) {
        [_gview getShapes]->draw(*gs);
    }
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
        UIView *v = [_gview getOwnerView];
        Box2d rect(Box2d(0, 0, v.bounds.size.width, v.bounds.size.height) * [_gview getXform]->displayToWorld());
        gs->drawRect(&ctx, rect, false);
    }
    else if (inactive) {
        GiContext ctx(0, GiColor(64, 64, 64, 172));
        Point2d ptd(Point2d(_pointW.x, _pointW.y) * _xform->worldToDisplay());
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

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (sender.view == self) {
        if (sender.state == UIGestureRecognizerStateChanged) {
            [self zoomPan:[sender translationInView:sender.view]];
            [sender setTranslation:CGPointZero inView:sender.view];
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
