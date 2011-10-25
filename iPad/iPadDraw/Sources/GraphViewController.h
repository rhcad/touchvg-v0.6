// GraphViewController.h
// Created by Zhang Yungui on 2012-3-2.

#import <GraphView/GiViewController.h>

#if 0
#define TEST_SIMPLE_VIEW 1

@interface GraphViewController : GiViewController {
}
@end

#else

@interface GraphViewController : UIViewController {
    GiViewController    *_graph;        // 图形视图控制器
    UIButton            *_downview;     // 底部按钮栏
    UIButton            *redBtn;
	UIButton            *blueBtn;
	UIButton            *yellowbtn;
	UIButton            *colorbtn;
	UIButton            *brushbtn;
	UIButton            *erasebtn;
    UIButton            *clearbtn;
    UIButton            *backbtn;
}

- (void)clearCachedData;                // 清除缓冲数据

- (IBAction)showPenView:(id)sender;     // 显示画笔粗细，透明度选择，直线，虚线视图
- (IBAction)colorBtnPress:(id)sender;   // 调节画笔的属性；红蓝黄，直线虚线，方法
- (IBAction)lineWidthChange:(id)sender; // 调节画笔粗细
- (IBAction)alphaChange:(id)sender;     // 调节画笔透明度
- (IBAction)eraseColor:(id)sender;      // 橡皮功能
- (IBAction)colorMapPress:(id)sender;   // 调色板

- (IBAction)backToView:(id)sender;      // 退出自由绘图
- (IBAction)clearView:(id)sender;       // 清屏
- (IBAction)backtoDraw:(id)sender;      // 返回自由绘制（在画直线或者虚线后，需要该方法返回）
- (IBAction)showPaletee:(id)sender;     // 显示调色板

- (UIButton *)addButton:(NSString *)imgname action:(SEL)action frame:(CGRect)frame;
- (void)showUnlightButtons;             // 显示全部非高亮钮

@end
#endif
