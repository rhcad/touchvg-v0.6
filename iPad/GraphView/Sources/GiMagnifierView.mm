// GiMagnifierView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include "GiGraphIos.h"
#include <Graph2d/mgshapes.h>

@implementation GiMagnifierView

@synthesize graph = _graph;
@synthesize centerW;

- (id)initWithFrame:(CGRect)frame graphView:(id<GiView>)gview
{
    self = [super initWithFrame:frame];
    if (self) {
        _xform = new GiTransform();
        _graph = new GiGraphIos(*_xform);
        _xform->setViewScaleRange(1, 50);
        _gview = gview;
        _drawingDelegate = Nil;
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

- (CGPoint)centerW {
    return CGPointMake(_xform->getCenterW().x, _xform->getCenterW().y);
}

- (void)setCenterW:(CGPoint)pt {
    Point2d ptd = Point2d(pt.x, pt.y) * _xform->worldToDisplay();
    if (!CGRectContainsPoint(CGRectInset(self.bounds, 20, 20), CGPointMake(ptd.x, ptd.y))) { 
        _xform->zoom(Point2d(pt.x, pt.y), _xform->getViewScale());
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
    _drawingDelegate = d;
}

- (void)regen {
    _graph->clearCachedBitmap();
    [self setNeedsDisplay];
}

- (void)redraw {
    [self setNeedsDisplay];
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    GiGraphIos* gs = (GiGraphIos*)_graph;
    
    _xform->setWndSize(CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
    _xform->zoom(_xform->getCenterW(), [_gview getXform]->getViewScale() * 3);
    _graph->setBkColor(giFromCGColor(self.backgroundColor.CGColor));
    
    if (gs->beginPaint(context, [[self gestureRecognizers]count] > 0))
    {
        if (!gs->drawCachedBitmap(0, 0)) {
            [self draw:gs];
            gs->saveCachedBitmap();
        }
        
        GiContext ctx(0, GiColor(64, 64, 64, 172));
        gs->rawLine(&ctx, self.center.x - 20, self.center.y, self.center.x + 20, self.center.y);
        gs->rawLine(&ctx, self.center.x, self.center.y - 20, self.center.x, self.center.y + 20);
        
        if ([_drawingDelegate respondsToSelector:@selector(dynDraw:)]) {
            [_drawingDelegate performSelector:@selector(dynDraw:) withObject:self];
        }
        
        gs->endPaint();
    }
}

- (void)draw:(GiGraphics*)gs
{
    if ([_gview getShapes]) {
        [_gview getShapes]->draw(*gs);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if ([_drawingDelegate respondsToSelector:@selector(graphViewActivated:)]) {
        [_drawingDelegate performSelector:@selector(graphViewActivated:) withObject:self];
    }
    [super touchesEnded:touches withEvent:event];
}

@end
