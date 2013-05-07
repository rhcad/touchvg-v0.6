// TestGraphView.mm
// Created by Zhang Yungui on 2012-3-2.

#import "TestGraphView.h"
#include <mgshapedoc.h>

#ifdef TESTMODE_SIMPLEVIEW
#ifdef USE_RANDOMSHAPE
#include "RandomShape.cpp"
#endif

@implementation TestGraphView

- (void)dealloc
{
    if (_doc) {
        _doc->release();
        _doc = NULL;
    }
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor colorWithRed:0.9 green:0.95 blue:0.9 alpha:1.0];//clearColor
        _doc = MgShapeDoc::create();
        
#ifdef USE_RANDOMSHAPE
        RandomParam::init();
        
        RandomParam param;
        param.lineCount = 100;
        param.arcCount = 50;
        param.curveCount = 20;
        param.randomLineStyle = true;
        
        param.initShapes(_doc->getCurrentShapes());
        
        [self xform]->zoomTo(_doc->getExtent() * [self xform]->modelToWorld());
        [self xform]->zoomByFactor(4.0);
#endif
    }
    return self;
}

- (void)reset
{
#ifdef USE_RANDOMSHAPE
    RandomParam param;
    param.lineCount = RandomParam::RandInt(0, 200);
    param.arcCount = RandomParam::RandInt(0, 100);
    param.curveCount = RandomParam::RandInt(0, 50);
    param.randomLineStyle = true;
    
    _doc->clear();
    param.initShapes(_doc->getCurrentShapes());
    [self setDoc:_doc];
#endif
}

- (BOOL)undoMotion
{
    [self reset];
    return YES;
}

@end
#endif // TESTMODE_SIMPLEVIEW
