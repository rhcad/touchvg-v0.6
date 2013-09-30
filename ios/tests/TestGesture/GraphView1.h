// GraphView1.h
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@interface GraphView1 : UIView {
    CGPoint         _lastpt;
    int             _flags;
}

@property(nonatomic, readonly) UIPanGestureRecognizer *panGestureRecognizer;

- (id)initWithFrame:(CGRect)frame withFlags:(int)t;

@end
