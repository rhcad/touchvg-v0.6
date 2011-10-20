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
        _shapes = createShapes();
        
        srand((unsigned)time(NULL));
        
        RandomParam param;
        param.lineCount = 100;
        param.arcCount = 50;
        param.curveCount = 20;
        param.randomLineStyle = true;
        
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
    
    param.initShapes(_shapes);
    
    _graph->clearCachedBitmap();
    [self setNeedsDisplay];
}

- (BOOL)undoMotion
{
    [self reset];
    return YES;
}

@end
