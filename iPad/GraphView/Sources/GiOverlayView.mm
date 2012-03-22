// GiOverlayView.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"
#include "GiGraphIos.h"
#include <Graph2d/mgshapes.h>

@implementation GiOverlayView

@synthesize shapes = _shapes;
@synthesize xform = _xform;
@synthesize graph = _graph;

- (id)initWithView:(UIView*)view
{
    self = [super init];
    if (self) {
        _view = view;
        _shapes = NULL; // need to set by the controller or subclass
        _xform = new GiTransform();
        _graph = new GiGraphIos(*_xform);
        _drawingDelegate = Nil;
        
        _xform->setWndSize(CGRectGetWidth(view.bounds), CGRectGetHeight(view.bounds));
        _xform->setViewScaleRange(0.01, 20.0);
        _xform->zoomTo(Point2d(0,0));
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

- (void)drawOnView:(CGContextRef)context
{
    GiGraphIos* gs = (GiGraphIos*)_graph;
    
    _xform->setWndSize(CGRectGetWidth(_view.bounds), CGRectGetHeight(_view.bounds));
    _graph->setBkColor(giFromCGColor(_view.backgroundColor.CGColor));
    
    if (gs->beginPaint(context, true))
    {
        if (!gs->drawCachedBitmap(0, 0)) {
            [self draw:gs];
            gs->saveCachedBitmap();
        }
        if ([_drawingDelegate respondsToSelector:@selector(dynDraw)]) {
            [_drawingDelegate performSelector:@selector(dynDraw)];
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

- (void)setShapes:(MgShapes*)data
{
    _shapes = data;
    _graph->clearCachedBitmap();
    [_view setNeedsDisplay];
}

- (void)setDrawingDelegate:(id)d
{
    _drawingDelegate = d;
}

- (void)setAnimating:(BOOL)animated
{
}

- (void)redraw
{
    [_view setNeedsDisplay];
}

@end
