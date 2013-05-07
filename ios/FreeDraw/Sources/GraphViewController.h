// GraphViewController.h
// Created by Zhang Yungui on 2012-3-2.

#import "TestConfig.h"

#ifdef TESTMODE_SIMPLEVIEW
#import <GiViewController.h>

@interface GraphViewController : GiViewController {
}
@end

#else // TESTMODE
#import <GiViewController.h>
@class GiViewControllerEx;

@interface GraphViewController : UIViewController {
    GiViewControllerEx  *_graphc;       // 图形视图控制器
    UIView              *_magViews[2];  // 放大镜视图
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
- (void)regen;                          // 标记视图待重新构建显示

- (IBAction)showPenView:(id)sender;     // 显示画笔粗细，透明度选择，直线，虚线视图
- (IBAction)colorBtnPress:(id)sender;   // 调节画笔的属性；红蓝黄，直线虚线，方法
- (IBAction)lineWidthChange:(id)sender; // 调节画笔粗细
- (IBAction)alphaChange:(id)sender;     // 调节画笔透明度
- (IBAction)eraseColor:(id)sender;      // 橡皮功能

- (IBAction)backToView:(id)sender;      // 退出自由绘图
- (IBAction)clearView:(id)sender;       // 清屏
- (IBAction)hideMagnifier:(id)sender;   // 切换放大镜视图的可见性
- (IBAction)hideOverview:(id)sender;    // 切换缩小显示的视图的可见性
- (IBAction)showPaletee:(id)sender;     // 显示调色板

- (IBAction)lockMagnifier:(id)sender;   // 切换是否禁止放大镜动态显示
- (IBAction)resizeMagnifier:(id)sender; // 切换放大镜视图大小
- (IBAction)fireUndo:(id)sender;        // 回退一步
- (IBAction)addTestShapes:(id)sender;   // 添加随机测试图形
- (IBAction)selectCommand:(id)sender;   // 选择绘图命令
- (IBAction)commandSelected:(id)sender; // 绘图命令已选择
- (IBAction)writeInBox:(id)sender;      // 在放大区域内写字
- (IBAction)addImageShape:(id)sender;   // 插入一个图片

- (UIButton *)addButton:(NSString *)imgname action:(SEL)action bar:(UIView*)bar
                      x:(CGFloat*)x size:(CGFloat)size diffx:(CGFloat)diffx;
- (void)showUnlightButtons;             // 显示全部非高亮钮

@end
#endif // TESTMODE
