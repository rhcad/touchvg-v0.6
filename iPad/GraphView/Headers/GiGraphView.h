// GiGraphView.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

class GiTransform;
class GiGraphics;

@interface GiGraphView : UIView {
    GiTransform*    _xform;
    GiGraphics*     _graph;
    
    CGPoint         _firstPoint;
    CGPoint         _lastPoint;
    
    BOOL            _zooming;
    double          _lastViewScale;
    CGPoint         _lastCenterW;
    
    BOOL            _doubleZoomed;
    double          _scaleBeforeDbl;
    CGPoint         _centerBeforeDbl;
}

- (void)draw:(GiGraphics*)gs;
- (void)dynDraw:(GiGraphics*)gs;

- (void)afterCreated;
- (void)addGestureRecognizers;

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;

@end
