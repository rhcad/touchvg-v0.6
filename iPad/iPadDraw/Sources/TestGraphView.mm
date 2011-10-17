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
        param.lineCount = 100;
        param.arcCount = 50;
        param.curveCount = 20;
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

- (BOOL)undoMotion
{
    if (self.viewMode != GiViewModeView)
        return [super undoMotion];

    [self reset];
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
    return item->draw(gs, ctx);
}

- (double)hitTest:(void*)shape limits:(const BOX2D*)box
{
    ShapeItem* item = (ShapeItem*)shape;
    double dist = 1e30;
    item->hitTest(*box, dist);
    return dist;
}

@end
