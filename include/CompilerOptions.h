#pragma once
#ifndef __COMPILER_OPTIONS_H__
#define __COMPILER_OPTIONS_H__

// 编译配置文件

//////////////////////////////////////////////////////////////////////////
// 1.系统平台选项

#if defined(_WIN32) || defined(_WIN64)
#define __WIN_PLATFORM
#elif (defined(_LINUX) || defined(__GNU__) || defined(__GLIBC__) || defined(linux) || defined(__linux) || defined(__linux__)) && !defined(_CRAYC)
#define __LINUX_PLATFORM
#ifdef __ANDROID__
#define __ANDROID_PLATFORM
#endif //__ANDROID__
#elif defined(__APPLE__) || defined(__APPLE_CC__) || defined(macintosh)
#define __MACOS_PLATFORM
#elif defined(__CYGWIN__)
#define __CYGWIN_PLATFORM
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#define __BSD_PLATFORM
#else
#define __OTHER_PLATFORM
#endif

#ifdef __WIN_PLATFORM
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_WARNINGS
//#define _WIN32_SERVER_	// xp or windows server 2003 必须开启
#pragma warning(disable:4996)
#pragma warning(disable:4275)
#pragma warning(disable:4251)
#pragma warning(disable:4005)
#endif //__WIN_PLATFORM

#ifdef KERNEL_NO_COMPILE_MESSAGES
#	define KERNEL_COMPILE_MESSAGE(msg) 
#else
#	define KERNEL_COMPILRE_MESSAGE(msg) __FILE__ "(" BOOST_STRINGIZE(__LINE__)") : DESC - " msg " - [ "__FUNCTION__ " ]" 
#	if defined(_MSC_VER) // msc_ver only supported on msvc
#		define COMPILER_MESSAGE(msg) __pragma( message( KERNEL_COMPILRE_MESSAGE(msg) ) )
#	elif defined(__GNUC__) && defined(GCC_VERSION) && GCC_VERSION >= 40400  // And GCC > 4.4.x 
#		define COMPILER_MESSAGE(msg) _Pragma(#msg)
#		define COMPILER_MESSAGE(msg) COMPILER_MESSAGE_GNUC( message(KERNEL_COMPILER_MESSAGE(msg)) )
#	else
#		define COMPILER_MESSAGE(msg) 
#	endif
#endif

// 校验windows编译版本
#ifdef __WIN_PLATFORM
#if _MSC_VER == 1900
#define __MSVC_2015_VER__		// vc14
#elif _MSC_VER == 1800
#define __MSVC_2013_VER__		// vc12
#elif _MSC_VER == 1700
#define __MSVC_2012_VER__		// vc11
#elif _MSC_VER == 1600
#define __MSVC_2010_VER__		// vc10
#elif _MSC_VER == 1500
#define __MSVC_2008_VER__		// vc9
#elif _MSC_VER == 1400
#define __MSVC_2005_VER__		// vc8
#elif _MSC_VER == 1310
#define __MSVC_2003_VER__		// vc7.1
#endif //_MSC_VER
#endif

#ifdef __WIN_PLATFORM
#if _MSC_VER < 1310 // vc7.1以上(vs2003)
#error Does not support this version of the compiler
#endif //_MSC_VER
#endif

//////////////////////////////////////////////////////////////////////////
// 2.引擎网络模式选项

#ifdef __WIN_PLATFORM
#define _IOCP_NET_MODE_
//#define _SELECT_NET_MODE_
#pragma comment(lib, "ws2_32.lib")
#elif defined(__LINUX_PLATFORM)
#define _EPOLL_NET_MODE_
#elif defined(__MACOS_PLATFORM)
#define _KQUEUE_NET_MODE_
#endif

//////////////////////////////////////////////////////////////////////////
// 3.数据库选项

// 开启数据库模块
//#define _OPEN_DB_MODULE_

// 要加载的数据库模块
#ifdef _OPEN_DB_MODULE_
//#define __MYSQL_DB_MODULE__		// mysql
//#define __MSSQL_DB_MODULE__		// sql server
//#define __REDIS_DB_MODULE__		// redis
//#define __MONGODB_DB_MODULE__		// mongodb
#endif //_OPEN_DB_MODULE_

//////////////////////////////////////////////////////////////////////////
// 4.lua模块选项

// 开启lua模块
//#define _OPEN_LUA_MODULE_

#ifdef _OPEN_LUA_MODULE_
#ifdef __WIN_PLATFORM
#pragma comment(lib, "LuaCppModule.lib") 
#endif //__WIN_PLATFORM
#endif //_OPEN_LUA_MODULE_

//////////////////////////////////////////////////////////////////////////
// 5.其他模块选项 linux 由makefile控制编译

#ifdef __WIN_PLATFORM
//#define _OPEN_ENCRYPT_MODULE_		// 引擎aes加密算法模块
//#define _OPEN_COMPRESS_MODULE_		// 引擎解压缩模块
//#define _OPEN_WEB_CLIENT_MODULE_	// 引擎web客户端模块

#ifdef _OPEN_ENCRYPT_MODULE_
#pragma comment(lib, "libeay32.lib")
#endif

#ifdef _OPEN_COMPRESS_MODULE_
#ifdef WIN32
#ifdef NDEBUG
#pragma comment(lib, "Zlib.lib")
#else
#pragma comment(lib, "ZlibD.lib")
#endif
#elif WIN64
#pragma comment(lib, "zlibwapi.lib")
#endif //_WIN32
#endif

#ifdef _OPEN_WEB_CLIENT_MODULE_
#ifdef NDEBUG
#pragma comment(lib, "libcurl.lib")
#else
#pragma comment(lib, "libcurld.lib")
#endif //_DEBUG
#endif
#endif //__WIN_PLATFORM

#endif //__COMPILER_OPTIONS_H__