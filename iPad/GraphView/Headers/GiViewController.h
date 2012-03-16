// GiViewController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <UIKit/UIKit.h>

@class GiGraphView;
@class GiSelectController;
@class GiCommandController;

@interface GiViewController : UIViewController {
    GiGraphView*            _gview;
    GiSelectController*     _selector;
    GiCommandController*    _commands;
}

- (void)clearCachedData;
- (void)dynDraw:(void*)gs;

@end
