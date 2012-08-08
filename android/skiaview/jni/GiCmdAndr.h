//! \file GiCmdAndr.h
//! \brief 定义命令控制器类 GiCmdController
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIA_CMDCONTROLLER_H_
#define __TOUCHVG_SKIA_CMDCONTROLLER_H_

//! 命令控制器类，代理调用内部命令(MgCommand)
/** \ingroup GRAPH_SKIA
 * 	\see MgCommand
*/
class GiCmdController
{
public:
    GiCmdController();
    virtual ~GiCmdController();
};

#endif // __TOUCHVG_SKIA_CMDCONTROLLER_H_
