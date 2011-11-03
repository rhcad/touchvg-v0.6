// GiMagnifierView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include "GiGraphIos.h"
#include <Graph2d/mgshapes.h>

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
        _graph = new GiGraphIos(*_xform);
        _xform->setViewScaleRange(1, 50);
        _graph->setMaxPenWidth(10);
        _gview = gview;
        _drawingDelegate = Nil;
        _scale = 3;
        _lockRedraw = YES;
        
        self.multipleTouchEnabled = YES;
        self.contentMode = UIViewContentModeRedraw;
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

- (BOOL)isActiveView
{
    return ([_drawingDelegate respondsToSelector:@selector(activeView)]
            && self == [_drawingDelegate performSelector:@selector(activeView)]);
}

- (void)setPointW:(CGPoint)pt {
    _pointW = pt;
    if (!_lockRedraw || [self isActiveView])
        [self setPointWandRedraw:pt];
}

- (void)setPointWandRedraw:(CGPoint)pt
{
    _pointW = pt;
    Point2d ptd = Point2d(pt.x, pt.y) * _xform->worldToDisplay();
    BOOL inside = CGRectContainsPoint(CGRectInset(self.bounds, 20, 20), CGPointMake(ptd.x, ptd.y));
    
    if (!inside) {
        _xform->zoom(Point2d(pt.x, pt.y), _xform->getViewScale());
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

- (void)setShapes:(MgShapes*)data {
}

- (void)setAnimating:(BOOL)animated {
}

- (void)setDrawingDelegate:(id)d {
    _drawingDelegate = (UIResponder*)d;
}

- (void)regen {
    _xform->zoom(Point2d(_pointW.x, _pointW.y), [_gview getXform]->getViewScale() * _scale);
    _graph->clearCachedBitmap();
    [self setNeedsDisplay];
    [_gview regen];
}

- (void)redraw {
    [self setNeedsDisplay];
    [_gview redraw];
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    GiGraphIos* gs = (GiGraphIos*)_graph;
    
    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _xform->zoom(_xform->getCenterW(), [_gview getXform]->getViewScale() * _scale);
    _graph->setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (gs->beginPaint(context))
    {
        if (!gs->drawCachedBitmap(0, 0)) {
            [self draw:gs];
            gs->saveCachedBitmap();
        }
        [self dynDraw:gs];
        gs->endPaint();
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
    
    if (inactive) {
        GiContext ctx(0, GiColor(64, 64, 64, 172));
        Point2d ptd(Point2d(_pointW.x, _pointW.y) * _xform->worldToDisplay());
        gs->rawLine(&ctx, ptd.x - 20, ptd.y, ptd.x + 20, ptd.y);
        gs->rawLine(&ctx, ptd.x, ptd.y - 20, ptd.x, ptd.y + 20);
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [_drawingDelegate touchesBegan:touches withEvent:event];
    [super touchesBegan:touches withEvent:event];
}

@end
