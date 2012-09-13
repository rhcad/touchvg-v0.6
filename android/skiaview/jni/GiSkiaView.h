//! \file GiSkiaView.h
//! \brief 定义支持Android平台的图形视图类 GiSkiaView
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIAVIEW_H_
#define __TOUCHVG_SKIAVIEW_H_

#include "GiCanvasBase.h"

class MgStorageBase;
struct MgShapes;
class MgViewProxy;
class GiContext;

typedef enum {                //!< 手势类型
    kGestureUnknown,          //!< 未知的手势
    kSinglePan = 1,           //!< 单指滑动
    kSingleTap,               //!< 单指单击
    kDoubleTap,               //!< 单指双击
    kLongPress,               //!< 长按
    kZoomRotatePan,           //!< 双指移动
    kTwoFingersDblClick,      //!< 双指双击
} GiGestureType;

typedef enum { //!< 手势状态
    kGestureCancel = 0,       //!< 取消
    kGestureBegan,            //!< 开始
    kGestureMoved,            //!< 改变
    kGestureEnded,            //!< 结束
} GiGestureState;

//! 支持Android平台的图形视图类
/*! \ingroup GRAPH_SKIA
 */
class GiSkiaView
{
public:
    GiSkiaView(GiCanvasBase* canvas);
    virtual ~GiSkiaView();
    
    //! 保存图形列表
    bool saveShapes(MgStorageBase* s);
    
    //! 加载图形列表，s为NULL则清空
    bool loadShapes(MgStorageBase* s);

    //! 得到图形个数
	int getShapeCount();
    
    //! 得到图形内容改变计数，可用于觉得是否需要调用 saveShapes
	int getChangeCount();

    //! 添加测试图形
    int addTestingShapes();
    
    //! 返回视图宽度，像素
    int getWidth() const;
    
    //! 返回视图高度，像素
    int getHeight() const;
    
    //! 设置视图宽高，像素
    void onSize(int width, int height);
    
    //! 显示正式图形
    bool onDraw(GiCanvasBase& canvas);
    
    //! 显示临时动态图形
    bool onDynDraw(GiCanvasBase& canvas);
    
    //! 返回当前命令名称
    const char* getCommandName() const;
    
    //! 启动指定名称的命令
    bool setCommandName(const char* name);
    
    //! 传递触摸手势消息
    /**
     * \param gestureType 手势类型, GiGestureType
     * \param gestureState 手势状态, GiGestureState, gestureType为 kSinglePan 或 kZoomRotatePan 时有效
     * \param fingerCount 触点个数
     * \param x1 第一个触点的X坐标，fingerCount小于1时忽略
     * \param y1 第一个触点的Y坐标，fingerCount小于1时忽略
     * \param x2 第二个触点的X坐标，fingerCount小于2时忽略
     * \param y2 第二个触点的Y坐标，fingerCount小于2时忽略
     * \return 内部是否响应了此手势
     */
    bool onGesture(GiGestureType gestureType, GiGestureState gestureState, int fingerCount,
                   float x1, float y1, float x2, float y2);
    
    //! 返回当前绘图属性
    /**
     * \param forChange 是否用于改动绘图属性
     * \return 当前绘图属性，如果选中了图形则为所选图形的属性
     */
    GiContext& getCurrentContext(bool forChange);
    
    //! 绘图属性改变后提交更新
    /** 在 getCurrentContext(true) 后调用本函数。
     * \param ctx 绘图属性
     * \param mask 需要应用哪些属性，-1表示全部属性，0则不修改，按位组合值见 GiContext 的 GiContextBits
     * \param apply 0表示还要继续动态修改属性，1表示结束动态修改并提交，-1表示结束动态修改并放弃改动
     */
    void applyContext(const GiContext& ctx, int mask, int apply);
    
    //! 设置允许的放缩类型: 0-禁止, 1-平移, 2-缩放, 4-局部放大和还原, 7-全部
    void setZoomFeature(int mask);
    
private:
    bool dynZoom(const Point2d& pt1, const Point2d& pt2, int gestureState);
    bool switchZoom(const Point2d& pt);
    
private:
    MgViewProxy*        _view;
    int                 _zoomMask;
    Point2d             _lastCenterW;
    float               _lastViewScale;
    Point2d             _firstPt;
    float               _firstDist;
};

#endif // __TOUCHVG_SKIAVIEW_H_
