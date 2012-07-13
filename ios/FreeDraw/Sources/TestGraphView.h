// TestGraphView.h
// Created by Zhang Yungui on 2012-3-2.

#import "TestConfig.h"

#ifdef TESTMODE_SIMPLEVIEW
#import <GiGraphView.h>

@interface TestGraphView : GiGraphView {
}

- (void)reset;

@end
#endif // TESTMODE_SIMPLEVIEW
