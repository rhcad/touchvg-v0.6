// TestGraphView.mm
// Created by Zhang Yungui on 2012-3-2.

#import "TestGraphView.h"

#ifdef TESTMODE_SIMPLEVIEW
#include <mgshapest.h>
#include <vector>
#include "../../../core/include/testgraph/RandomShape.cpp"

@implementation TestGraphView

- (void)dealloc
{
    if (_shapes) {
        _shapes->release();
        _shapes = NULL;
    }
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor colorWithRed:0.9 green:0.95 blue:0.9 alpha:1.0];//clearColor
        _shapes = new MgShapesT<std::vector<MgShape*> >;
        
        RandomParam::init();
        
        RandomParam param;
        param.lineCount = 100;
        param.arcCount = 50;
        param.curveCount = 20;
        param.randomLineStyle = true;
        
        param.initShapes(_shapes);
        
        [self xform]->zoomTo(_shapes->getExtent() * [self xform]->modelToWorld());
        [self xform]->zoomByFactor(4.0);
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
    
    _shapes->clear();
    param.initShapes(_shapes);
    [self setShapes:_shapes];
}

- (BOOL)undoMotion
{
    [self reset];
    return YES;
}

@end
#endif // TESTMODE_SIMPLEVIEW
