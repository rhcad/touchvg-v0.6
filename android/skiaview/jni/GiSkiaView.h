//! \file GiSkiaView.h
//! \brief 定义支持Android平台的图形视图类 GiSkiaView
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIAVIEW_H_
#define __TOUCHVG_SKIAVIEW_H_

#include "GiCanvasBase.h"

class GiCmdController;
class MgStorageBase;
struct MgShapes;
class MgViewProxy;

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

    //! 返回是否需要重绘视图
	bool isNeedRedraw() const;

    //! 传递触摸手势消息
    /**
     * \param gestureType 手势类型，1-单指滑动，2-单指单击，3-单指双击，4-长按，5-双指滑动，6-双指放缩，7-双指双击
     * \param gestureState 手势状态，1-开始，2-改变，3-结束，0-取消
     * \param fingerCount 触点个数
     * \param x1 第一个触点的X坐标，fingerCount小于1时忽略
     * \param y1 第一个触点的Y坐标，fingerCount小于1时忽略
     * \param x2 第二个触点的X坐标，fingerCount小于2时忽略
     * \param y2 第二个触点的Y坐标，fingerCount小于2时忽略
     * \return 内部是否响应了此手势
     * \see isNeedRedraw
     */
    bool onGesture(int gestureType, int gestureState, int fingerCount,
    		float x1, float y1, float x2, float y2);

private:
    MgViewProxy*		_view;
    GiCmdController*    _cmdc;
};

#endif // __TOUCHVG_SKIAVIEW_H_
