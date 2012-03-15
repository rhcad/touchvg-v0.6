// GiGraphView.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

class GiTransform;
class GiGraphics;

typedef enum {
    GiViewModeView,
    GiViewModeSelect,
    GiViewModeCreateShape,
    GiViewModeMax
} GiViewMode;

@interface GiGraphView : UIView {
    GiTransform*    _xform;
    GiGraphics*     _graph;
    GiViewMode      _viewMode;
    
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

- (void)draw:(GiGraphics*)gs;
- (void)dynDraw:(GiGraphics*)gs;

- (void)setAnimating:(BOOL)animated;
- (void)shakeMotion;

- (void)afterCreated;
- (void)setViewMode:(GiViewMode)mode;

@end
