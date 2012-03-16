// TestGraphView.mm
// Created by Zhang Yungui on 2012-3-2.

#import "TestGraphView.h"
#include "../../../core/include/testgraph/shape.cpp"

@implementation TestGraphView

- (void)dealloc
{
    if (_shapes) {
        delete _shapes;
        _shapes = NULL;
    }
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        srand((unsigned)time(NULL));
        
        RandomParam param;
        param.lineCount = 1000;
        param.arcCount = 500;
        param.curveCount = 200;
        param.randomLineStyle = true;
        
        _shapes = new Shapes(param.getShapeCount());
        param.initShapes(_shapes);
        
        _xform->zoomTo(_shapes->getExtent() * _xform->modelToWorld());
        _xform->zoomByFactor(4.0);
    }
    return self;
}

- (void)reset
{
    RandomParam param;
    param.lineCount = RandomParam::RandInt(0, 200);
    param.arcCount = RandomParam::RandInt(0, 100);
    param.curveCount = RandomParam::RandInt(0, 50);
    param.randomLineStyle = true;
    
    delete _shapes;
    _shapes = new Shapes(param.getShapeCount());
    param.initShapes(_shapes);
    
    _graph->clearCachedBitmap();
    [self setNeedsDisplay];
}

- (void)dynDraw:(GiGraphics*)gs
{
    if (!CGPointEqualToPoint(_lastPoint, _firstPoint)) {
        GiContext context(0, GiColor(128,0,0,128), kLineSolid, GiColor(128,0,0,64));
        gs->rawLine(&context, _firstPoint.x, _firstPoint.y, _lastPoint.x, _lastPoint.y);
        gs->rawEllipse(&context, _firstPoint.x - 10, _firstPoint.y - 10, 20, 20);
        gs->rawEllipse(&context, _lastPoint.x - 10, _lastPoint.y - 10, 20, 20);
    }
    [super dynDraw:gs];
}

- (BOOL)undoMotion
{
    if (self.viewMode != GiViewModeView)
        return [super undoMotion];

    [self reset];
    return YES;
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (self.viewMode != GiViewModeView)
        return [super oneFingerPan:sender];
    
    if (sender.state == UIGestureRecognizerStateBegan) {
        _firstPoint = [sender locationInView:self];
        _lastPoint = _firstPoint;
    }
    else if (sender.state == UIGestureRecognizerStateChanged) {
        _lastPoint = [sender locationInView:self];
        [self setNeedsDisplay];
    }
    else {
        _lastPoint = _firstPoint;
        [self setNeedsDisplay];
    }
    
    return YES;
}

#pragma mark - ShapeProvider

- (void*)getFirstShape:(void**)it
{
    if (_shapes->getShapeCount() > 0) {
        *it = (void*)0;
        return _shapes->getShape(0);
    }
    return NULL;
}

- (void*)getNextShape:(void**)it
{
    int index = 1 + (int)*it;
    if (index > 0 && index < _shapes->getShapeCount()) {
        *it = (void*)index;
        return _shapes->getShape(index);
    }
    return NULL;
}

- (BOX2D)getShapeExtent:(void*)shape
{
    ShapeItem* item = (ShapeItem*)shape;
    return item->getExtent();
}

- (void)drawShape:(void*)shape graphics:(GiGraphics*)gs context:(const GiContext *)ctx
{
    ShapeItem* item = (ShapeItem*)shape;
    return item->draw(gs);
}

@end
