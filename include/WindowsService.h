#include "Types.h"
#ifdef __WIN_PLATFORM
#define _WINDOWS_SERVICE_
#endif //__WIN_PLATFORM

#ifdef _WINDOWS_SERVICE_
#ifndef __WINDOWS_SERVICE_H__
#define __WINDOWS_SERVICE_H__

#include <boost/function.hpp>

// windows service 运行回调
typedef boost::function<bool(const char*)>	WinServiceCall;
  
/// @function 初始化windows服务
/// 
/// @param serviceName 服务名称
/// @param runCallback 运行回调函数
/// @param serviceDesc 服务描述信息
_KERNEL_EXPORTS void InitWinService(TCHAR serviceName[128], WinServiceCall runCallback, const char* runParam, TCHAR serviceDesc[512]);  

/// @function 安装服务
/// 
/// @return 成功返回true，失败返回false
_KERNEL_EXPORTS bool WinInstall();  

/// @function 卸载服务
/// 
/// @return 成功返回true，失败返回false
_KERNEL_EXPORTS bool WinUninstall();

/// @function 启动服务
/// 
_KERNEL_EXPORTS void WinStart();

#endif //__WINDOWS_SERVICE_H__
#endif //_WINDOWS_SERVICE_