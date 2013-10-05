//! \file DemoCmds.h
//! \brief 定义示例应用的功能入口类 DemoCmdsGate

#ifndef DEMOCMDS_GATE_H_
#define DEMOCMDS_GATE_H_

#include "mgvector.h"
#include "mgviewhandle.h"

//! 示例应用的功能入口类
/*! \ingroup CORE_VIEW
    参数 mgView 为 GiCoreView 的 viewAdapter() 返回值，可转换为 MgView 指针。
 */
struct DemoCmdsGate {
    //! 登记绘图命令和图形类型
    static int registerCmds(MGVIEW_HANDLE mgView);

    //! 得到当前图形的各种度量尺寸
    static int getDimensions(MGVIEW_HANDLE mgView, 
        mgvector<float>& vars, mgvector<char>& types);
};

#endif // DEMOCMDS_GATE_H_
