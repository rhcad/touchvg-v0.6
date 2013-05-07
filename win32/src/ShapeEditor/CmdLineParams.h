// CmdLineParams.h: 定义命令行参数接口
//

#ifndef _APP_ICMDLINE_PARAMS_H
#define _APP_ICMDLINE_PARAMS_H

//! 分析命令行参数
bool SetCmdLineParams(LPCTSTR cmdLine);

//! 查询命令行参数
/*!
    \param[in] name 参数名称，没有标点符号
    \param[out] value 如果不为NULL则填充参数对应的值
    \return 命令行中是否有此参数
*/
bool FindCmdLineParam(LPCTSTR name, LPCTSTR* value = NULL);

//! 去掉一个命令行参数
bool RemoveCmdLineParam(LPCTSTR name);

#endif // _APP_ICMDLINE_PARAMS_H
