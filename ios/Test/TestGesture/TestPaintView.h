// TestPaintView.h
// Created by Zhang Yungui on 2012-8-27.
//

#import "GestureHandler.h"

@interface TestPaintView : UIView<UIGestureHandler> {
    CGPoint     _points[200];
    int         _count;
}
@end
