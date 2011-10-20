// GiGraphView.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

struct MgShapes;
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
    MgShapes*       _shapes;
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

@property (nonatomic,readonly) MgShapes*    shapes;
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

