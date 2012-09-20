//
//  main.m
//  TestGesture
//
//  Created by Zhang Yungui on 12-8-23.
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    [pool release];
    
    return retVal;
}
