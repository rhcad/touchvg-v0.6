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

//! 矢量路径节点类型
/*! \see GiPath
*/
enum kGiPathNode {
    kGiCloseFigure = 1,
    kGiLineTo = 2,
    kGiBeziersTo = 4,
    kGiMoveTo = 6,
};

#endif // __GEOMETRY_GIDEF_H_
