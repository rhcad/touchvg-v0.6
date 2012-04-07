//! \file gigraph.h
//! \brief 定义图形系统类 GiGraphics
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_GRAPHSYS_H_
#define __GEOMETRY_GRAPHSYS_H_

#include "gixform.h"
#include "gicontxt.h"

class GiGraphicsImpl;

//! 图形系统类
/*! 本类用于显示各种图形，图元显示原语由派生类实现，例如采用GDI/GDI+/SVG/PDF等实现；
    显示图形所用的坐标计算和坐标系转换是在 GiTransform 中定义的。
    \ingroup GRAPH_INTERFACE
*/
class GiGraphics
{
public:
    GiGraphics(GiTransform* xform);
    GiGraphics(const GiGraphics& src);
    virtual ~GiGraphics();
    GiGraphics& operator=(const GiGraphics& src);

protected:
    void beginPaint(const RECT* clipBox);
    void endPaint();

public:
    //! 返回坐标系管理对象
    const GiTransform& xf() const;

    //! 返回是否正在绘制，即调用了 beginPaint() 还未调用 endPaint()
    bool isDrawing() const;

public:
    //! 用当前背景色清除背景
    /*! 用背景色填充全部显示区域。打印或打印预览时调用无效。
        \see getBkColor
    */
    virtual void clearWnd() = 0;

    //! 显示后备缓冲位图
    /*! 后备缓冲位图是由函数 saveCachedBitmap 保存创建的
        \param x 显示偏移X，像素
        \param y 显示偏移Y，像素
        \param secondBmp 使用是否为第二个后备缓冲位图
        \return 是否绘制成功
        \see saveCachedBitmap
    */
    virtual bool drawCachedBitmap(int x = 0, int y = 0, bool secondBmp = false) = 0;

    //! 显示后备缓冲位图
    /*! 将另一个图形系统的后备缓冲位图显示到本对象的显示设备上，
        后备缓冲位图是由函数 saveCachedBitmap 保存创建的。\n
        另一个图形系统的作用是后台显示，其本身并不显示图形到设备上，即调用了 endPaint(false)
        \param p 另一个图形系统，该对象有后备缓冲位图
        \param secondBmp 使用是否为第二个后备缓冲位图
        \return 是否绘制成功
        \see saveCachedBitmap
    */
    virtual bool drawCachedBitmap2(const GiGraphics* p, bool secondBmp = false) = 0;

    //! 保存显示内容到后备缓冲位图
    /*! 将当前绘图目标(可能是绘图缓冲)的内容保存为一个位图对象，
        该位图的大小为显示窗口大小。\n
        当显示窗口大小改变或显示放缩后，将自动清除后备缓冲位图。
        \param secondBmp 是否存到第二个后备缓冲位图
    */
    virtual void saveCachedBitmap(bool secondBmp = false) = 0;

    //! 返回是否有后备缓冲位图
    /*!
        \param secondBmp 检查的是否为第二个后备缓冲位图
        \return 是否有该缓冲位图
    */
    virtual bool hasCachedBitmap(bool secondBmp = false) const = 0;

    //! 清除后备缓冲位图
    /*! 当显示窗口大小改变或显示放缩后，调用 beginPaint() 时将自动清除后备缓冲位图。
    */
    virtual void clearCachedBitmap() = 0;

    //! 返回是否正在绘图缓冲上绘图
    /*! 在派生类的 beginPaint 函数中指定是否使用绘图缓冲
        \return 是否正在绘图缓冲上绘图
        \see beginPaint
    */
    virtual bool isBufferedDrawing() const = 0;

    //! 返回图形系统类型，由派生类决定值
    virtual int getGraphType() const = 0;

    //! 返回屏幕分辨率DPI, 常量
    virtual int getScreenDpi() const = 0;


    //! 返回剪裁框，模型坐标
    Box2d getClipModel() const;

    //! 得到剪裁框，逻辑坐标
    /*!
        \param[out] prc 填充逻辑坐标矩形
    */
    virtual void getClipBox(RECT* prc) const;

    //! 设置剪裁框，逻辑坐标
    /*! 只有正处于绘图状态时，该函数的返回值才有效。
        失败时不改变图形系统内部变量和设备描述表的剪裁框，
        因此在失败时不要进行相应绘图操作，否则处于错误的剪裁框中。

        \param[in] prc 逻辑坐标矩形
        \return 是否成功设置剪裁框，没有处于绘图状态中或计算出的剪裁框为空时失败
    */
    virtual bool setClipBox(const RECT* prc);

    //! 设置剪裁框，世界坐标
    /*! 新剪裁框将会和原始剪裁框相叠加求交，所以函数可能返回失败。\n
        失败时不改变图形系统内部变量和设备描述表的剪裁框，
        因此在失败时不要进行相应绘图操作，否则处于错误的剪裁框中。\n
        只有正处于绘图状态时，该函数的返回值才有效。

        \param[in] rectWorld 世界坐标矩形
        \return 是否成功设置剪裁框，没有处于绘图状态中或计算出的剪裁框为空时失败
    */
    virtual bool setClipWorld(const Box2d& rectWorld);
    

    //! 颜色模式定义
    enum kColorMode {
        kColorReal,     //!< 真彩
        kColorGray,     //!< 灰度，256级
        kColorMono      //!< 单色，背景色和反色
    };
    //! 返回颜色模式
    /*!
        \return 颜色模式，见 kColorMode 定义
        \see kColorMode
    */
    int getColorMode() const;

    //! 设置颜色模式
    /*!
        \param mode 颜色模式，见 kColorMode 定义
        \see kColorMode
    */
    void setColorMode(int mode);
    

    //! 返回背景色
    /*!
        \return 背景色，RGB色，如果是打印或打印预览，则为白色
    */
    virtual GiColor getBkColor() const = 0;

    //! 设置背景色
    /*! 如果正处于绘图状态中，该函数将同时设置绘图设备的背景色
        \param color 背景色，RGB色
        \return 原来的背景色
    */
    virtual GiColor setBkColor(const GiColor& color) = 0;

    //! 返回和模拟设备的调色板相近的颜色
    /*! 打印或打印预览时本函数才有效果，此时模拟设备为打印机，否则为当前显示设备。
        \param color 普通的RGB颜色
        \return 符合模拟设备的RGB颜色
    */
    virtual GiColor getNearestColor(const GiColor& color) const = 0;

    //! 计算画笔颜色
    /*! 根据颜色模式和设备属性调整显示颜色。\n
        如果传入颜色和背景色相同，本函数不会自动反色，因此可能看不见图形
        \param color 普通的RGB颜色
        \return 实际绘图的RGB颜色
    */
    GiColor calcPenColor(const GiColor& color) const;

    //! 计算画笔宽度
    /*! 打印时会自动调整以像素为单位的线宽，使得不致太细
        \param lineWidth 线宽。正数单位为0.01毫米，负数单位为像素，0表示1像素
        \param useViewScale 是否使用显示比例来计算
        \return 画笔宽度，非负数，像素
    */
    UInt16 calcPenWidth(Int16 lineWidth, bool useViewScale = true) const;

    //! 设置最大画笔宽度，像素
    /*! 只需要按屏幕显示来设置，打印时会自动调整
        \param pixels 最大画笔宽度，像素
    */
    void setMaxPenWidth(UInt8 pixels);

    //! 返回是否为反走样模式
    bool isAntiAliasMode() const;

    //! 设置是否为反走样模式
    virtual void setAntiAliasMode(bool antiAlias);
    
    //! 返回当前绘图参数
    virtual const GiContext* getCurrentContext() const = 0;
    

    //! 绘制直线段，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param startPt 起点
        \param endPt 终点
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawLine(const GiContext* ctx, 
        const Point2d& startPt, const Point2d& endPt, bool modelUnit = true);

    //! 绘制折线，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为2
        \param points 顶点数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawLines(const GiContext* ctx, 
        int count, const Point2d* points, bool modelUnit = true);

    //! 绘制多条贝塞尔曲线，模型坐标或世界坐标
    /*! 第一条曲线从第一个点绘制到第四个点，以第二个点和第三个点为控制点。
        此序列中的每一条后续曲线都需要三个点：
        前一个曲线的终点作为起点，序列中的另两个点为控制点，第三个点为终点。

        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为4，必须为3的倍数加1
        \param points 控制点和端点的数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawBeziers(const GiContext* ctx, 
        int count, const Point2d* points, bool modelUnit = true);

    //! 绘制椭圆弧，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param center 椭圆心
        \param rx 椭圆半长轴的长度
        \param ry 椭圆半短轴的长度，为0时取为rx
        \param startAngle 起始角度，弧度，相对于指定坐标系+X轴，逆时针为正
        \param sweepAngle 转角，弧度，相对于起始角度，逆时针为正，顺时针为负，
            大于2PI或小于-2PI时将取2PI或-2PI
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawArc(const GiContext* ctx, 
        const Point2d& center, double rx, double ry, 
        double startAngle, double sweepAngle, bool modelUnit = true);


    //! 绘制并填充多边形，模型坐标或世界坐标
    /*! 通过绘制从最后一个顶点到第一个顶点的线段而自动闭合
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为2，不需要首末两点重合
        \param points 顶点数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawPolygon(const GiContext* ctx, 
        int count, const Point2d* points, bool modelUnit = true);

    //! 绘制并填充椭圆，模型坐标或世界坐标
    /*! 如果半长轴和半短轴的长度相同，则相当于圆
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param center 圆心
        \param rx 椭圆半长轴的长度
        \param ry 椭圆半短轴的长度，为0时取为rx
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawEllipse(const GiContext* ctx, 
        const Point2d& center, double rx, double ry = 0.0, 
        bool modelUnit = true);
    
    //! 绘制并填充椭圆，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param rect 椭圆外框
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
     */
    bool drawEllipse(const GiContext* ctx, const Box2d& rect, bool modelUnit = true);

    //! 绘制并填充椭圆弧饼图，模型坐标或世界坐标
    /*! 绘制一个由椭圆弧和两个半径相交闭合而成的饼状楔形图。
        如果半长轴和半短轴的长度相同，则相当于圆弧。
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param center 椭圆心
        \param rx 椭圆半长轴的长度
        \param ry 椭圆半短轴的长度，为0时取为rx
        \param startAngle 起始角度，弧度，相对于指定坐标系+X轴，逆时针为正
        \param sweepAngle 转角，弧度，相对于起始角度，逆时针为正，顺时针为负，
            大于2PI或小于-2PI时将取2PI或-2PI
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawPie(const GiContext* ctx, 
        const Point2d& center, double rx, double ry, 
        double startAngle, double sweepAngle, bool modelUnit = true);

    //! 绘制并填充直角矩形，模型坐标或世界坐标
    /*! 
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param rect 直角矩形
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawRect(const GiContext* ctx, const Box2d& rect, bool modelUnit = true);

    //! 绘制并填充圆角矩形，模型坐标或世界坐标
    /*! 
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param rect 矩形外框
        \param rx X方向的圆角半径，正数时有效，否则忽略该参数
        \param ry Y方向的圆角半径，为非正数时取为rx
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawRoundRect(const GiContext* ctx, 
        const Box2d& rect, double rx, double ry = 0.0, bool modelUnit = true);


    //! 绘制三次参数样条曲线，模型坐标或世界坐标
    /*! 切矢量可以通过 mgCubicSplines 函数计算得到，一般先计算出切矢量供每次显示用
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 型值点的点数，至少为2
        \param knots 型值点坐标数组，元素个数为count
        \param knotVectors 型值点的切矢量数组，元素个数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see mgCubicSplines
        \see drawClosedSplines
    */
    bool drawSplines(const GiContext* ctx, 
        int count, const Point2d* knots, const Vector2d* knotVectors, 
        bool modelUnit = true);

    //! 绘制并填充三次参数样条曲线，自动闭合
    /*! 首末型值点不必重合，本函数通过绘制从最后一个型值点到第一个型值点的曲线段而自动闭合。\n
        切矢量可以通过 mgCubicSplines 函数计算得到，一般先计算出切矢量供每次显示用
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param count 型值点的点数，至少为2
        \param knots 型值点坐标数组，元素个数为count
        \param knotVectors 型值点的切矢量数组，元素个数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see mgCubicSplines
        \see drawSplines
    */
    bool drawClosedSplines(const GiContext* ctx, 
        int count, const Point2d* knots, const Vector2d* knotVectors, 
        bool modelUnit = true);

    //! 绘制三次B样条曲线，模型坐标或世界坐标
    /*! 
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 控制点的点数，至少为4
        \param controlPoints 控制点坐标数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see drawClosedBSplines
    */
    bool drawBSplines(const GiContext* ctx, 
        int count, const Point2d* controlPoints, bool modelUnit = true);

    //! 绘制并填充三次B样条曲线，自动闭合
    /*! 首末控制点不必重合，本函数通过补充绘制曲线段而自动闭合
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param count 控制点的点数，至少为3
        \param controlPoints 控制点坐标数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see drawBSplines
    */
    bool drawClosedBSplines(const GiContext* ctx, 
        int count, const Point2d* controlPoints, bool modelUnit = true);

    //! 绘制路径，模型坐标或世界坐标
    /*! 
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param count 点数，points和types的元素个数
        \param points 端点和控制点的数组
        \param types points中每一点的含义，见MSDN中PolyDraw的说明，值为PT_MOVETO等
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see GiPath::getPoints
    */
    bool drawPath(const GiContext* ctx, int count, 
        const Point2d* points, const UInt8* types, bool modelUnit = true);


    //! 绘制直线段的原语函数，像素坐标，不剪裁
    virtual bool rawLine(const GiContext* ctx, 
        int x1, int y1, int x2, int y2) = 0;

    //! 绘制折线的原语函数，像素坐标，不剪裁
    virtual bool rawPolyline(const GiContext* ctx, 
        const POINT* lppt, int count) = 0;

    //! 绘制多条贝塞尔曲线的原语函数，像素坐标，不剪裁
    virtual bool rawPolyBezier(const GiContext* ctx, 
        const POINT* lppt, int count) = 0;

    //! 绘制多边形的原语函数，像素坐标，不剪裁
    virtual bool rawPolygon(const GiContext* ctx, 
        const POINT* lppt, int count) = 0;

    //! 绘制矩形的原语函数，像素坐标，不剪裁
    virtual bool rawRect(const GiContext* ctx, 
        int x, int y, int w, int h) = 0;

    //! 绘制椭圆的原语函数，像素坐标，不剪裁
    virtual bool rawEllipse(const GiContext* ctx, 
        int x, int y, int w, int h) = 0;

    //! 绘制多样线的原语函数，像素坐标，不剪裁
    virtual bool rawPolyDraw(const GiContext* ctx, 
        int count, const POINT* lppt, const UInt8* types) = 0;


    //! 开始一个路径的原语函数
    virtual bool rawBeginPath() = 0;

    //! 结束并显示一个路径的原语函数
    virtual bool rawEndPath(const GiContext* ctx, bool fill) = 0;

    //! 在当前路径中移动到新的位置的原语函数
    virtual bool rawMoveTo(int x, int y) = 0;

    //! 在当前路径中添加画线指令到新的位置的原语函数
    virtual bool rawLineTo(int x, int y) = 0;

    //! 在当前路径中添加画贝塞尔曲线指令的原语函数
    virtual bool rawPolyBezierTo(const POINT* lppt, int count) = 0;

    //! 在当前路径中添加闭合指令的原语函数
    virtual bool rawCloseFigure() = 0;

protected:
    GiGraphics();
    GiGraphicsImpl*  m_impl;
};

//! 保存和恢复图形系统的剪裁框的辅助类
/*! 利用该类在堆栈上定义局部变量，该变量出作用区后自动析构从而恢复剪裁框。
    利用该类可以避免因中途退出或异常时没有执行恢复剪裁框的语句。
    \ingroup GRAPH_INTERFACE
*/
class GiSaveClipBox
{
public:
    //! 构造函数，将新的剪裁框压栈
    /*! 必须调用 succeed 函数检查是否成功，失败时不要继续绘图
        \param gs 要保存剪裁框的图形系统
        \param rectWorld 新剪裁框的世界坐标矩形
        \see succeed
    */
    GiSaveClipBox(GiGraphics* gs, const Box2d& rectWorld)
    {
        m_gs = gs;
        gs->getClipBox(&m_clipRect);
        m_clip = gs->setClipWorld(rectWorld);
    }

    //! 析构函数，恢复上一个剪裁框
    ~GiSaveClipBox()
    {
        if (m_clip)
            m_gs->setClipBox(&m_clipRect);
    }

    //! 返回是否成功设置了新剪裁框
    /*! 如果返回false，则不要继续在该世界坐标剪裁框下的绘图操作，
        因为失败时表明无法绘制任何图形，但是设置剪裁框时并不改变剪裁框，
        如果还继续绘图，则可能绘制出图形，但已经不在正确的剪裁框中。
        \return 是否成功设置了新剪裁框
    */
    bool succeed() const
    {
        return m_clip;
    }

private:
    GiGraphics* m_gs;
    RECT        m_clipRect;
    bool        m_clip;
};

#endif // __GEOMETRY_GRAPHSYS_H_
