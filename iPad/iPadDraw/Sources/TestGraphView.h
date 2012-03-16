// TestGraphView.h
// Created by Zhang Yungui on 2012-3-2.

#import <GraphView/GiGraphView.h>

class Shapes;

@interface TestGraphView : GiGraphView {
    Shapes*     _shapes;
}

- (void)reset;

@end
