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

@end
////////////////////////////////////////////////////////////////


// Implementation
@implementation AppDelegate

@synthesize window = _window;


- (void)dealloc {
    
    self.window = nil;
    
    [super dealloc];
}


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    _window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    self.window.backgroundColor = [UIColor darkGrayColor];
    
    GraphViewController *controller = [[GraphViewController alloc] init];
    [self.window addSubview:controller.view];
    [controller release];
    
    [self.window makeKeyAndVisible];
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    
    // TODO:
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    
    // TODO:
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    
    // TODO:
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    
    // TODO:
}


- (void)applicationWillTerminate:(UIApplication *)application {
    
    // TODO:
}

@end
