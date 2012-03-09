//! \file gidef.h
//! \brief 定义图形接口库的基本宏
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

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

#ifndef GetRValue
typedef struct tagRECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
} RECT;
typedef struct tagPOINT
{
    long  x;
    long  y;
} POINT;
#endif // _WINDOWS_

#endif // __GEOMETRY_GIDEF_H_