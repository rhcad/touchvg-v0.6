//
//  TestGraphIosAppDelegate.h
//  TestGraphIos
//
//  Created by Zhang Yungui on 2/27/12.
//  Copyright 2012 github.com/rhcad. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TestGraphIosViewController;

@interface TestGraphIosAppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet TestGraphIosViewController *viewController;

@end
