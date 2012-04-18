//! \file gidef.h
//! \brief 定义图形接口库的基本宏
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_GIDEF_H_
#define __GEOMETRY_GIDEF_H_

#ifndef _WIN32
inline long giInterlockedIncrement(long *p) { return ++*p; }
inline long giInterlockedDecrement(long *p) { return --*p; }
#else
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
inline long giInterlockedIncrement(long *p) { return InterlockedIncrement(p); }
inline long giInterlockedDecrement(long *p) { return InterlockedDecrement(p); }
#endif

#ifdef GetRValue
#include <mgtype.h>
inline RECT2D giConvertRect(const RECT& rc)
{
    RECT2D rect;
    rect.left = (float)rc.left;
    rect.top = (float)rc.top;
    rect.right = (float)rc.right;
    rect.bottom = (float)rc.bottom;
    return rect;
}
#endif // _WINDOWS_

#endif // __GEOMETRY_GIDEF_H_
