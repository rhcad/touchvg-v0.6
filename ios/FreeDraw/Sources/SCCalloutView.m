#import "SCCalloutView.h"
#import <GiViewController.h>

extern void giIgnoreTouchesBegan(UIView* sender, CGPoint point);

@implementation SCCalloutView

- (void)dealloc
{
    [super dealloc];
}

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event
{
	BOOL res = [super pointInside:point withEvent:event];
	if (!res) {
        [GiViewController ignoreTouchesBegan:point view:self];
		[self removeFromSuperview];
    }
	return res;
}

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
	UIView *view = [super hitTest:point withEvent:event];
	if (view == self) {
        [GiViewController ignoreTouchesBegan:point view:self];
		[self removeFromSuperview];
	}
	return view;
}

@end

@implementation SCCalloutGraphView
@synthesize graphc = _graphc;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _graphc = [[GiViewController alloc]init];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame graphc:(GiViewController*)g
{
    self = [super initWithFrame:frame];
    if (self) {
        _graphc = g;
        [_graphc retain];
    }
    return self;
}

- (void)dealloc
{
    [_graphc dynamicChangeEnded:YES];
    [_graphc release];
    [super dealloc];
}

@end
