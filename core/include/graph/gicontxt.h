//! \file gicontxt.h
//! \brief 定义绘图环境类 GiContext
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_DRAWCONTEXT_H_
#define __GEOMETRY_DRAWCONTEXT_H_

#include "gicolor.h"

//! 线型
enum kLineStyle {
    kLineSolid = 0,     //!< ----------
    kLineDash,          //!< － － － －
    kLineDot,           //!< ..........
    kLineDashDot,       //!< _._._._._
    kLineDashDotdot,    //!< _.._.._.._
    kLineNull           //!< Not draw.
};

//! 设置属性的位掩码类型
enum kContextBits {
    kContextLineRGB   = 0x01,
    kContextLineAlpha = 0x02,
    kContextLineColor = 0x03,
    kContextLineWidth = 0x04,
    kContextLineStyle = 0x08,
    kContextFillRGB   = 0x10,
    kContextFillAlpha = 0x20,
    kContextFillColor = 0x30,
    kContextCopyAll   = 0xFF,
};

//! 绘图参数上下文类
/*! 用于在图形系统的绘图函数中传入绘图参数
    \ingroup GRAPH_INTERFACE
    \see kLineStyle, GiColor
*/
class GiContext
{
public:
    //! 默认构造函数
    /*! 绘图参数为1像素宽的黑实线、不填充
    */
    GiContext() : m_type(0), m_lineStyle(kLineSolid), m_lineWidth(0)
        , m_lineColor(GiColor::Black()), m_fillColor(GiColor::Invalid())
    {
    }
    
    //! 指定线条参数来构造
    /*! 填充参数为不填充
        \param width 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
        \param color 线条颜色， GiColor::Invalid() 表示不画线条
        \param style 线型, kLineStyle, 取值为 kLineSolid 等
        \param fillcr 填充颜色， GiColor::Invalid() 表示不填充
    */
    GiContext(float width, GiColor color = GiColor::Black(), 
              int style = kLineSolid, const GiColor& fillcr = GiColor::Invalid())
        : m_type(0), m_lineStyle(style), m_lineWidth(width)
        , m_lineColor(color), m_fillColor(fillcr)
    {
    }
    
    //! 拷贝构造函数
    GiContext(const GiContext& src)
    {
        m_lineStyle = src.m_lineStyle;
        m_lineWidth = src.m_lineWidth;
        m_lineColor = src.m_lineColor;
        m_fillColor = src.m_fillColor;
    }

    //! 赋值函数, kContextBits 按位设置
    GiContext& copy(const GiContext& src, int mask = -1)
    {
        if (this != &src)
        {
            if (mask & 0x01) {
                GiColor c = src.m_lineColor;
                m_lineColor.set(c.r, c.g, c.b);
            }
            if (mask & 0x02) {
                m_lineColor.a = src.m_lineColor.a;
            }
            if (mask & 0x04) {
                m_lineWidth = src.m_lineWidth;
            }
            if (mask & 0x08) {
                m_lineStyle = src.m_lineStyle;
            }
            if (mask & 0x10) {
                GiColor c = src.m_fillColor;
                m_fillColor.set(c.r, c.g, c.b);
            }
            if (mask & 0x20) {
                m_fillColor.a = src.m_fillColor.a;
            }
        }
        return *this;
    }

    //! 比较相等操作符函数
    bool equals(const GiContext& src) const
    {
        return m_lineStyle == src.m_lineStyle
            && m_lineWidth == src.m_lineWidth
            && m_lineColor == src.m_lineColor
            && m_fillColor == src.m_fillColor;
    }
    
#ifndef SWIG
    //! 赋值操作符函数
    GiContext& operator=(const GiContext& src)
    {
        return copy(src);
    }
    
    //! 比较相等操作符函数
    bool operator==(const GiContext& src) const
    {
        return equals(src);
    }
    
    //! 比较不相等操作符函数
    bool operator!=(const GiContext& src) const
    {
        return !equals(src);
    }
#endif // SWIG
    
    //! 返回线型, kLineStyle
    int getLineStyle() const
    {
        return m_lineColor.isInvalid() ? kLineNull : m_lineStyle;
    }
    
    //! 设置线型, kLineStyle
    void setLineStyle(int style)
    {
        m_lineStyle = style;
    }
    
    //! 返回线宽
    /*! 正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
    */
    float getLineWidth() const
    {
        return m_lineWidth;
    }
    
    //! 设置线宽
    /*!
        \param width 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
    */
    void setLineWidth(float width)
    {
        m_lineWidth = width;
    }
    
    //! 返回是否为空线，即不画线
    bool isNullLine() const
    {
        return m_lineStyle == kLineNull || m_lineColor.isInvalid();
    }
    
    //! 设置为空线，即不画线
    /*! 如果要恢复成普通画线状态，可调setLineStyle(kLineSolid)
    */
    void setNullLine()
    {
        m_lineStyle = kLineNull;
    }
    
    //! 返回线条颜色
    GiColor getLineColor() const
    {
        return m_lineColor;
    }
    
    //! 设置线条颜色， GiColor::Invalid() 表示不画线条
    void setLineColor(const GiColor& color)
    {
        m_lineColor = color;
    }
    
    //! 设置线条颜色
    void setLineColor(unsigned char r, unsigned char g, unsigned char b)
    {
        m_lineColor.set(r, g, b);
    }

    //! 返回线条ARGB颜色
    int getLineARGB() const
    {
        return m_lineColor.getARGB();
    }
    
    //! 设置线条ARGB颜色，0表示不画线条
    void setLineARGB(int argb)
    {
        m_lineColor.setARGB(argb);
    }
    
    //! 返回线条透明度
    char getLineAlpha() const
    {
        return m_lineColor.a;
    }
    
    //! 设置线条透明度，0到255，0表示全透明，255表示不透明
    void setLineAlpha(char alpha)
    {
        m_lineColor.a = alpha;
    }
    
    //! 返回是否填充
    bool hasFillColor() const
    {
        return !m_fillColor.isInvalid();
    }
    
    //! 设置为不填充
    void setNoFillColor()
    {
        m_fillColor = GiColor::Invalid();
    }
    
    //! 返回填充颜色
    GiColor getFillColor() const
    {
        return m_fillColor;
    }
    
    //! 设置填充颜色， GiColor::Invalid() 表示不填充
    void setFillColor(const GiColor& color)
    {
        m_fillColor = color;
    }
    
    //! 设置填充颜色
    void setFillColor(unsigned char r, unsigned char g, unsigned char b)
    {
        m_fillColor.set(r, g, b);
    }

    //! 返回线条ARGB颜色
    int getFillARGB() const
    {
        return m_fillColor.getARGB();
    }
    
    //! 设置线条ARGB颜色，0表示不填充
    void setFillARGB(int argb)
    {
        m_fillColor.setARGB(argb);
    }
    
    //! 返回填充透明度
    char getFillAlpha() const
    {
        return m_fillColor.a;
    }
    
    //! 设置填充透明度，0到255，0表示全透明，255表示不透明
    void setFillAlpha(char alpha)
    {
        m_fillColor.a = alpha;
    }
    
    //! 返回绘图环境类型，供派生类用
    /*! 本类始终返回1，派生类可设置 m_type 为其他值
    */
    int getType() const
    {
        return m_type;
    }
    
protected:
    int         m_type;            //!< 派生类可指定其他值来表示不同类型
private:
    int         m_lineStyle;       //!< 线型, kLineStyle
    float       m_lineWidth;       //!< 线宽, >0: 0.01mm, =0: 1px, <0:px
    GiColor     m_lineColor;
    GiColor     m_fillColor;
};

#endif // __GEOMETRY_DRAWCONTEXT_H_
