// TestGraphView.h
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>
#import <GraphView/GiGraphView.h>

class Shapes;

@interface TestGraphView : GiGraphView {
    Shapes*     _shapes;
}

@end
