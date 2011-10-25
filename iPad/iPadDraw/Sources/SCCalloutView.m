#import "SCCalloutView.h"

@implementation SCCalloutView

- (void)dealloc
{
    [super dealloc];
}

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event
{
	BOOL res = [super pointInside:point withEvent:event];
	if (!res)
		[self removeFromSuperview];
	return res;
}

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
	UIView *view = [super hitTest:point withEvent:event];
	if (view == self)
	{
		[self removeFromSuperview];
	}
	return view;
}

@end
