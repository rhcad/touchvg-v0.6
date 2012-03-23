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
    
#if 0
    [self createGraphView:rect backgroundColor:[UIColor whiteColor]];
#else
    TestGraphView *view = [[TestGraphView alloc] initWithFrame:rect];
    self.view = view;
    [view release];
#endif
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.commandName = "select";
}

- (void)undoMotion
{
    static int n = 0;
    const char* names[] = { "select", "splines", "lines", "rect" };
    self.commandName = names[++n % 4];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

@end
