//! \file gicolor.h
//! \brief 定义RGB颜色类: GiColor
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_GICOLOR_H_
#define __GEOMETRY_GICOLOR_H_

#include <mgtype.h>

_GEOM_BEGIN

//! RGB颜色
struct GiColor
{
    UInt8   r;
    UInt8   g;
    UInt8   b;
    UInt8   a;  //!< 0: transparent, 255: opaque

    GiColor() : r(0), g(0), b(0), a(255)
    {
    }

    GiColor(UInt8 _r, UInt8 _g, UInt8 _b, UInt8 _a = 255)
        : r(_r), g(_g), b(_b), a(_a)
    {
    }

    GiColor(const GiColor& c) : r(c.r), g(c.g), b(c.b), a(c.a)
    {
    }

    static GiColor White() { return GiColor(255, 255, 255); }
    static GiColor Black() { return GiColor(0, 0, 0); }

    void set(UInt8 _r, UInt8 _g, UInt8 _b)
    {
        r = _r;
        g = _g;
        b = _b;
        a = a ? a : 255;
    }

    void set(UInt8 _r, UInt8 _g, UInt8 _b, UInt8 _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    bool invalid() const
    {
        return !a && !r && !g && !b;
    }

    bool operator==(const GiColor& src) const
    {
        return r==src.r && g==src.g && b==src.b && a==src.a;
    }

    bool operator!=(const GiColor& src) const
    {
        return !operator==(src);
    }
};

//! 无效颜色值或非RGB的特殊意义值
#define GICOLOR_INVALID GiColor(0, 0, 0, 0)

_GEOM_END
#endif // __GEOMETRY_GICOLOR_H_