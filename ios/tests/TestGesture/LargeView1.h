// LargeView1.h
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@class GraphView1;

@interface LargeView1 : UIScrollView<UIScrollViewDelegate> {
    GraphView1 *_subview;
}

- (id)initWithFrame:(CGRect)frame withFlags:(int)t;

@end
