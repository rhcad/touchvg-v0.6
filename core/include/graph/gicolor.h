//! \file gicolor.h
//! \brief 定义RGB颜色类: GiColor
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_GICOLOR_H_
#define __GEOMETRY_GICOLOR_H_

#include <mgtype.h>

//! RGB颜色
/*!
    \ingroup GRAPH_INTERFACE
    \see GiContext
*/
struct GiColor
{
    char    r;  //!< Red component, 0 to 255.
    char    g;  //!< Green component, 0 to 255.
    char    b;  //!< Blue component, 0 to 255.
    char    a;  //!< Alpha component, 0 to 255. 0: transparent, 255: opaque.

    GiColor() : r(0), g(0), b(0), a(255)
    {
    }

    GiColor(char _r, char _g, char _b, char _a = 255)
        : r(_r), g(_g), b(_b), a(_a)
    {
    }

    GiColor(const GiColor& c) : r(c.r), g(c.g), b(c.b), a(c.a)
    {
    }

    static GiColor White() { return GiColor(255, 255, 255); }
    static GiColor Black() { return GiColor(0, 0, 0); }
    static GiColor Invalid() { return GiColor(0, 0, 0, 0); }

    void set(char _r, char _g, char _b)
    {
        r = _r;
        g = _g;
        b = _b;
        a = a ? a : 255;
    }

    void set(char _r, char _g, char _b, char _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    bool isInvalid() const
    {
        return !a && !r && !g && !b;
    }

    bool equals(const GiColor& src) const
    {
        return r==src.r && g==src.g && b==src.b && a==src.a;
    }

    bool operator==(const GiColor& src) const
    {
        return equals(src);
    }

    bool operator!=(const GiColor& src) const
    {
        return !equals(src);
    }
};

#endif // __GEOMETRY_GICOLOR_H_
