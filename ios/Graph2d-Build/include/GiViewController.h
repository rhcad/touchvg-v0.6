//! \file GiViewController.h
//! \brief 定义图形视图控制器类 GiViewController
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

//! 图形视图控制器类
/*! 使用此类管理图形视图和触摸手势。
    \ingroup GRAPH_IOS
*/
@interface GiViewController : UIViewController<UIGestureRecognizerDelegate> {
@private
    id      _cmdctl;                    //!< 绘图命令控制器, GiCommandController
    void*   _docCreated;                //!< 创建的图形列表, MgShapeDoc*
    BOOL    _gestureRecognizerUsed;     //!< 是否使用手势识别器处理触摸消息
    int     _touchCount;                //!< 开始触摸时的手指数
    UIView  *_magViews[3];              //!< 创建的放大镜视图，最多两个
    UIView  *_activeView;               //!< 当前的图形视图，主视图或第一个放大镜视图
    BOOL    _ignoreTouches;             //!< 忽略当前触摸手势，直到触摸结束
    NSTimeInterval  _timeBegan;         //!< 开始触摸时的时刻
    UIGestureRecognizerState _gestureState;  //!< 手势状态
    void*   _images;                    //!< 图像图形列表
    
    enum { kPinchGesture = 0, kRotateGesture, kTwoFingersPan, kPanGesture, 
        kTapGesture, TwoTapsGesture, kTwoFingersTwoTaps, kLongPressGesture,
        RECOGNIZER_COUNT };
    UIGestureRecognizer* _recognizers[2][RECOGNIZER_COUNT]; //!< 手势识别器，主视图和放大镜视图的
}

@property (nonatomic,readonly)  void*   doc;            //!< 图形文档, MgShapeDoc*
@property (nonatomic,readonly)  void*   currentShapes;  //!< 图形列表,MgShapes*
@property (nonatomic,readonly)  UIView* magnifierView;  //!< 第一个放大镜视图
@property (nonatomic,readonly)  UIView* activeView;     //!< 当前的图形视图，主视图或第一个放大镜视图
@property (nonatomic,assign)  NSObject* editDelegate;   //!< 编辑代理,GiEditAction

//! 当前命令名称
@property (nonatomic)         const char*   commandName;

//! 当前选中图形是否固定边长
@property (nonatomic)         BOOL      currentShapeFixedLength;

//! 是否使用手势识别器处理触摸消息
@property (nonatomic)         BOOL      gestureRecognizerUsed;

//! 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
@property (nonatomic)         float     lineWidth;

//! 像素单位的线宽，正数
@property (nonatomic)         float     strokeWidth;

//! 线条颜色，忽略透明度分量（即总是1），clearColor 表示不画线条
@property (nonatomic,assign)  UIColor   *lineColor;

//! 线条透明度, 0-1
@property (nonatomic)         float     lineAlpha;

//! 填充颜色，忽略透明度分量（即总是1），clearColor 表示不填充，nil表示随线条颜色
@property (nonatomic,assign)  UIColor   *fillColor;

//! 填充透明度, 0-1
@property (nonatomic)         float     fillAlpha;

//! 线型, 0-Solid, 1-Dash, 2-Dot, 3-DashDot, 4-DashDotdot, 5-Null
@property (nonatomic)         int       lineStyle;


- (CGImageRef)cachedBitmap:(BOOL)invert;    //!< 得到当前缓存位图，上下翻转时由调用者释放
- (void)clearCachedData;                    //!< 清除缓冲数据，下次重新构建显示
- (void)regen;                              //!< 标记视图待重新构建显示
- (void)undoMotion;                         //!< 触发晃动或撤销操作
- (BOOL)isCommand:(const char*)cmdname;     //!< 检查当前是否为指定的命令
- (void)doContextAction:(int)action;        //!< 执行默认的上下文动作
- (UIGestureRecognizer*) getGestureRecognizer:(int)index;   //!< 得到主视图的触摸手势识别器

- (void)removeShapes;                       //!< 清除所有图形
- (NSUInteger)getShapeCount;                //!< 得到图形个数
- (BOOL)loadShapes:(void*)mgstorage;        //!< 从 MgStorage 对象加载图形列表
- (BOOL)saveShapes:(void*)mgstorage;        //!< 保存图形列表到 MgStorage 对象

//- (BOOL)record:(void*)mgstorage;            //!< 录制图形到 MgStorage 对象
//- (BOOL)playback:(void*)mgstorage;          //!< 从 MgStorage 对象回放图形，传NULL则清除
//- (NSUInteger)getChangeCount;               //!< 得到正式图形内容改变次数，可用于判断是否需要调用 record
//- (NSUInteger)getRedrawCount;               //!< 得到重绘次数，可用于判断是否需要调用 getDynamicShapes
//- (BOOL)getDynamicShapes:(void*)mgstorage;  //!< 保存临时动态图形到 MgStorage 对象
//- (BOOL)setDynamicShapes:(void*)mgstorage;  //!< 从 MgStorage 对象加载和播放动态图形，传NULL则清除

//! 创建了一个包含图像的矩形图形对象
/*! \param image 图像对象，为nil时根据filename自动加载
    \param filename 图像文件全名，图像自动加载时需要该值，可为nil
    \param name 没有路径的文件标识名称，最多63个字符，为nil时自动取filename的文件名
    \return 是否创建了一个包含图像的矩形图形对象
*/
- (BOOL)addImageShape:(UIImage*)image filename:(NSString*)filename name:(NSString*)name;

//! 得到图片形状的真实文件全名，默认返回Documents目录与name的组合路径文件名，可重载定位到其他目录
- (NSString*)getImageShapePath:(NSString*)name;

//! 图片形状对应的图像不再使用时的通知，可以用于删除相应图片文件
- (void)imageShapeDeleted:(NSString*)name;

//! 创建缩略图，由调用者释放
- (UIImage *)createThumbnail:(CGSize)size shapes:(void*)mgstorage invert:(BOOL)invert;

- (id)dynDraw:(id)sender;                   //!< 供图形视图动态显示时调用
- (void)afterZoomed:(id)sender;             //!< 供图形视图在动态放缩后通知
- (void)afterShapeChanged;                  //!< 图形列表改变后的通知
- (void)gestureStateChanged:(UIGestureRecognizer*)sender;  //!< 手势开始或结束

//! 创建图形视图(GiGraphView)和图形列表，不需要图形视图的派生类时使用
/*! 新视图作为本对象的自身视图（替代原视图），并已加入到parentView中。
    \param parentView 已有视图，将创建其子视图
    \param frame 视图位置大小，可取为 parentView.bounds
    \param bkColor 绘图背景色, 为空表示取上级视图的背景色或白色，新视图的背景色为透明色
    \return 创建的子图形视图(GiGraphView)
 */
- (UIView*)createGraphView:(UIView*)parentView frame:(CGRect)frame
           backgroundColor:(UIColor*)bkColor;

//! 在已有视图中创建子图形视图(GiGraphView)
/*! 创建的视图内默认不能放缩显示，背景色为透明色。新视图作为本对象的自身视图（替代原视图），并已加入到parentView中。
    \param parentView 已有视图，将创建其子视图，可以为滚动视图或特殊内容视图
    \param frame 视图位置大小，可取为 parentView.bounds
    \param doc 已有的共享图形列表，如果为NULL则自动创建图形列表
    \param bkColor 绘图背景色, 为空表示取上级视图的背景色或白色，新视图的背景色为透明色
    \return 创建的子图形视图(GiGraphView)
 */
- (UIView*)createSubGraphView:(UIView*)parentView frame:(CGRect)frame
                       doc:(void*)doc backgroundColor:(UIColor*)bkColor;

//! 在给定视图内创建放大镜视图
/*! 最多两个放大镜视图，第一个放大镜视图（放大显示）上可以使用手势绘图。已将新视图加入到parentView中。
    \param parentView 已有视图，将创建其子视图
    \param frame 视图位置大小，可取为 parentView.bounds 或更小区域
    \param scale 放大镜视图相对于图形视图(GiGraphView)的显示放大倍数，大于1时放大，小于1的正数缩小显示
    \return 创建的放大镜视图(GiMagnifierView)，或Nil表示不能创建更多的视图。
 */
- (UIView*)createMagnifierView:(UIView*)parentView frame:(CGRect)frame scale:(CGFloat)scale;

//! 让图形视图显示到指定的模型坐标区域，可用于写字视图
- (void)zoomTo:(CGRect)modelRect;

//! 点击导致来源视图消失时忽略本次触摸动作，直到触摸结束
+ (void)ignoreTouchesBegan:(CGPoint)point view:(UIView*)sender;

//! 退出动态修改模式（修改线宽和颜色等），应用修改结果或放弃修改
- (BOOL)dynamicChangeEnded:(BOOL)apply;

//! 得到当前图形的各种度量尺寸
//- (int)getDimensions:(float*)vars :(char*)types count:(int)count;

//! 返回选择包络框，显示坐标
- (CGRect)getBoundingViewBox;

@end
