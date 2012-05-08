//  AppDelegate.m
//  iPadDraw
//  Created by Zhang Yungui on 2012-3-2.
//

#import "AppDelegate.h"
#import "GraphViewController.h"

// Inner property and methods
@interface AppDelegate ()

#ifndef __IPHONE_5_0
@property (nonatomic, retain) UIWindow *window;
#endif
@property (nonatomic, retain) GraphViewController *controller;

@end
////////////////////////////////////////////////////////////////


// Implementation
@implementation AppDelegate

@synthesize window = _window;
@synthesize controller = _controller;

- (void)dealloc {
    [_controller release];
    [_window release];
    
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    _window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    _controller = [[GraphViewController alloc] init];
    [_window addSubview:_controller.view];
    [_window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [_controller clearCachedData];
    //[_controller.view removeFromSuperview];
    //[_controller release];
    //_controller = Nil;
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    //_controller = [[GraphViewController alloc] init];
    //[_window addSubview:_controller.view];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    NSLog(@"applicationWillResignActive");
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    NSLog(@"applicationDidBecomeActive");
}

- (void)applicationWillTerminate:(UIApplication *)application {
    NSLog(@"applicationWillTerminate");
}

@end
