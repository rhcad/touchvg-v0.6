// GraphViewController.mm
// Created by Zhang Yungui on 2012-3-2.

#import "GraphViewController.h"
#import "TestGraphView.h"

@implementation GraphViewController

- (void)dealloc
{
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void)loadView
{
    CGRect rect = [[UIScreen mainScreen] applicationFrame];
    rect.origin.y = 0;
    TestGraphView *view = [[TestGraphView alloc] initWithFrame:rect];
    view.backgroundColor = [UIColor whiteColor];
    self.view = view;
    [view release];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

@end
