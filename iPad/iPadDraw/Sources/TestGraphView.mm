// TestGraphView.mm
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#import "TestGraphView.h"
#include "../../core/include/testgraph/shape.cpp"

@implementation TestGraphView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        srand((unsigned)time(NULL));
        
        RandomParam param;
        param.lineCount = 1000;
        param.arcCount = 1000;
        param.randomLineStyle = true;
        
        _shapes = new Shapes(param.getShapeCount());
        param.initShapes(_shapes);
        
        //_xform->zoomTo(_shapes->getExtent() * _xform->modelToWorld());
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

@end
