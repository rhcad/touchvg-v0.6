// GiGraphView.h
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

class GiTransform;
class GiGraphIos;

@interface GiGraphView : UIView {
    GiTransform*    _xform;
    GiGraphIos*     _graph;
}

@end
