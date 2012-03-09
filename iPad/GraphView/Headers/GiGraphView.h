// GiGraphView.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: GPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

class GiTransform;
class GiGraphIos;
class GiGraphics;

@interface GiGraphView : UIView {
    GiTransform*    _xform;
    GiGraphIos*     _graph;
    
    BOOL            _fastDraw;
    double          _lastViewScale;
    CGPoint         _lastCenter;
    CGPoint         _firstPoint;
    CGPoint         _lastPoint;
    
    BOOL            _doubleZoomed;
    double          _scaleBefore;
    CGPoint         _centerBefore;
}

- (void)draw:(GiGraphics*)gs;

- (void)afterCreated;
- (void)addGestureRecognizers;

- (void)twoFingerPinch:(UIPinchGestureRecognizer *)recognizer;
- (void)twoFingerPan:(UIPanGestureRecognizer *)recognizer;
- (void)oneFingerPan:(UIPanGestureRecognizer *)recognizer;
- (void)oneFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)twoFingersTwoTaps;

@end
