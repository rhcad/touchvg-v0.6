#import <UIKit/UIKit.h>

// 实现在点击视图以外的区域关闭该视图
@interface SCCalloutView : UIButton
{
}

+ (id)currentView;

@end

@class GiViewController;

@interface SCCalloutGraphView : SCCalloutView
{
    GiViewController    *_graphc;
}

@property (nonatomic,readonly)  GiViewController *graphc;
@property (nonatomic,assign)    id  regenobj;

- (id)initWithFrame:(CGRect)frame graphc:(GiViewController*)g;

@end
