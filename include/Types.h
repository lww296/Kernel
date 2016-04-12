////////////////////////////////////////////////////////////////////
//							_ooOoo_								  //
//						   o8888888o							  //
//						   88" . "88							  //
//						   (| ^_^ |)							  //
//						   O\  =  /O							  //
//						____/`---'\____							  //
//					  .'  \\|     |//  `.						  //
//					 /  \\|||  :  |||//  \						  //
//				    /  _||||| -:- |||||-  \						  //
//				    |   | \\\  -  /// |   |						  //
//					| \_|  ''\---/''  |   |						  //
//					\  .-\__  `-`  ___/-. /						  //
//				  ___`. .'  /--.--\  `. . ___					  //
//				."" '<  `.___\_<|>_/___.'  >'"".				  //
//			  | | :  `- \`.;`\ _ /`;.`/ - ` : | |				  //
//			  \  \ `-.   \_ __\ /__ _/   .-` /  /                 //
//		========`-.____`-.___\_____/___.-`____.-'========		  //
//				             `=---='                              //
//		^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^        //
//         佛祖保佑       永无BUG		永不修改				  //
////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __TYPES_PALTFORM_H__
#define __TYPES_PALTFORM_H__

// 编译选项文件
#include "CompilerOptions.h"

/// @Note 公共定义及导出定义
#ifdef __WIN_PLATFORM
#ifdef SERVERKERNEL_EXPORTS
#	define _KERNEL_EXPORTS __declspec(dllexport)
#else
#	define _KERNEL_EXPORTS __declspec(dllimport)
#endif
#else
#define _KERNEL_EXPORTS __attribute__((visibility("default")))
#endif

#ifdef __WIN_PLATFORM
#ifndef __AFXWIN_H__
#include <windows.h>
#endif //__AFXWIN_H__
#include <WinBase.h>
#include <io.h>
#include <process.h>
#include <winsock2.h>
#include <mmsystem.h>
#include <mswsock.h>
#include <tchar.h>

#if _MSC_VER >= 1600 // vc10以上(vs2010)
#include <stdint.h>
#else
typedef unsigned long long uint64_t;
typedef long long int64_t;
#endif

#if _MSC_VER >= 1500 
#include <WS2tcpip.h>
#else
#pragma warning(disable:4267)
#pragma warning(disable:4616)
#pragma warning(disable:4288)
#pragma warning(disable:4312)
#pragma warning(disable:4244)
#define _WIN32_WINDOWS
typedef int socklen_t;
#define INET_ADDRSTRLEN 22
#endif

#if (_MSC_VER != 1500 && _MSC_VER < 1900)
#define vsnprintf	_vsnprintf
#endif

#if (_MSC_VER < 1900)
#define snprintf	_snprintf
#endif

#define	pthread_self	GetCurrentThreadId
#define usleep	Sleep
#define atoll	_atoi64
#define access	_access_s
#define lseek	_lseek

#endif //__WIN_PLATFORM

#ifdef __LINUX_PLATFORM
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include <netinet/tcp.h>
#ifndef __ANDROID_PLATFORM
#include <execinfo.h>
#include <syslog.h>
#include <sys/timerfd.h>
#endif //__ANDROID_PLATFORM
#include <stdint.h>
#include <netdb.h>
#endif //__LINUX_PLATFORM

#ifdef __MACOS_PLATFORM
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include <netinet/tcp.h>
#include <execinfo.h>
#include <syslog.h>
#include <stdint.h>
#include <netdb.h>

#define siginfo __siginfo

#endif //__MACOS_PLATFORM

// 公共头文件
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

using std::string;

#ifndef NULL
#define NULL 0
#endif

#ifndef _SAFE_DELETE_
#define _SAFE_DELETE_(ptr) { if(ptr) { delete(ptr); (ptr) = NULL; } }
#endif

#ifndef _SAFE_DELETE_ARRAY_
#define _SAFE_DELETE_ARRAY_(ptr) { if(ptr) { delete[] (ptr); (ptr) = NULL; } }
#endif

#ifndef SHUT_WR
#define SHUT_WR	2
#endif

#ifdef __WIN_PLATFORM
#if defined(_CRT_NO_TIME_T) || (_MSC_VER < 1900)
struct timespec
{
	time_t tv_sec;  // Seconds - >= 0
	long   tv_nsec; // Nanoseconds - [0, 999999999]
};
#endif
#endif

/// @brief 网络工作模式
enum NetWorkMode
{
	NET_MODE_NULL,
	NET_MODE_SELECT,		// select
	NET_MODE_EPOLL,			// epoll
	NET_MODE_IOCP,			// iocp
	NET_MODE_KQUEUE,		// kqueue
	NET_MODE_POLL,			// poll
};

/// @Brief 创建网络类型
enum SocketType
{
	SOCKET_TYPE_NULL,			/// null
	SOCKET_TYPE_TCP,			/// tcp
	SOCKET_TYPE_UDP,			/// udp
	SOCKET_TYPE_UDT,			/// udt
	SOCKET_TYPE_ICMP,			/// icmp
};

/// @brief 服务器类型
enum ServerType
{
	SERVER_TYPE_NULL,		// 未知

	SERVER_TYPE_CENTRE,		// 中心服
	SERVER_TYPE_GATEWAY,	// 网关服
	SERVER_TYPE_LOGIN,		// 登录服
	SERVER_TYPE_HALL,		// 大厅服
	SERVER_TYPE_GAME,		// 游戏服
	SERVER_TYPE_CHAT,		// 聊天服
	SERVER_TYPE_AI,			// AI服务器
	SERVER_TYPE_DB,			// 数据库服务器
	SERVER_TYPE_LB,			// 负载服务器
	SERVER_TYPE_LOG,		// 日志服务器
	SERVER_TYPE_VIDEO,		// 语音,视频
	SERVER_TYPE_ADMIN,		// 管理员服务器(总调度)
	SERVER_TYPE_PAY,		// 充值服务器
	SERVER_TYPE_SECNE,		// 场景服务器
	SERVER_TYPE_EXT1,		// 自定义扩展服务器1
	SERVER_TYPE_EXT2,		// 自定义扩展服务器2
	SERVER_TYPE_EXT3,		// 自定义扩展服务器3
	SERVER_TYPE_DISTRIBUTE,	// 分布式服务


	SERVER_TYPE_MAX
};

#endif //__TYPES_PALTFORM_H__