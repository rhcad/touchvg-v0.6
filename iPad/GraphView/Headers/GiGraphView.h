// GiGraphView.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

class GiTransform;
class GiGraphics;

typedef enum {
    GiViewModeView,
    GiViewModeSelect,
    GiViewModeCommand,
    GiViewModeMax
} GiViewMode;

@protocol GiMotionHandler

- (void)dynDraw:(GiGraphics*)gs;
- (BOOL)undoMotion;
- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (BOOL)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@interface GiGraphView : UIView<GiMotionHandler> {
    GiTransform*    _xform;
    GiGraphics*     _graph;
    GiViewMode      _viewMode;
    id              _drawingDelegate;
    
    CGPoint         _firstPoint;
    CGPoint         _lastPoint;
    
    BOOL            _zooming;
    double          _lastViewScale;
    CGPoint         _lastCenterW;
    
    BOOL            _doubleZoomed;
    double          _scaleBeforeDbl;
    CGPoint         _centerBeforeDbl;
}

@property (nonatomic,readonly) GiTransform* xform;
@property (nonatomic,readonly) GiGraphics*  graph;
@property (nonatomic)          GiViewMode   viewMode;
@property (nonatomic,readonly) BOOL         zooming;

- (void)setAnimating:(BOOL)animated;
- (void)setViewMode:(GiViewMode)mode;
- (void)setDrawingDelegate:(id)d;

- (void)afterCreated;
- (void)draw:(GiGraphics*)gs;

@end

#include <Graph2d/mgtype.h>
class GiContext;

@interface GiGraphView(ShapeProvider)

- (void*)getFirstShape:(void**)it;
- (void*)getNextShape:(void**)it;
- (BOX2D)getShapeExtent:(void*)shape;
- (void)drawShape:(void*)shape graphics:(GiGraphics*)gs context:(const GiContext *)ctx;

@end
