// TestGraphView.mm
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#import "TestGraphView.h"
#include "../../../core/include/testgraph/shape.cpp"

@implementation TestGraphView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        srand((unsigned)time(NULL));
        
        RandomParam param;
        param.lineCount = RandomParam::RandInt(0, 100);
        param.arcCount = RandomParam::RandInt(0, 50);
        param.curveCount = RandomParam::RandInt(0, 20);
        param.randomLineStyle = true;
        
        _shapes = new Shapes(param.getShapeCount());
        param.initShapes(_shapes);
        
        _xform->zoomTo(_shapes->getExtent() * _xform->modelToWorld());
        _xform->zoomByFactor(2.0);
    }
    return self;
}

- (void)dealloc
{
    if (_shapes) {
        delete _shapes;
        _shapes = NULL;
    }
    [super dealloc];
}

- (void)draw:(GiGraphics*)gs
{
    _shapes->draw(gs);
}

- (void)oneFingersTwoTaps
{
    RandomParam param;
    param.lineCount = RandomParam::RandInt(0, 100);
        param.arcCount = RandomParam::RandInt(0, 50);
        param.curveCount = RandomParam::RandInt(0, 20);
    param.randomLineStyle = true;
    
    delete _shapes;
    _shapes = new Shapes(param.getShapeCount());
    param.initShapes(_shapes);
    [self setNeedsDisplay];
}

@end
