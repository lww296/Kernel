#include <iostream>
#include "ServiceUnit.h"
#include "WindowsService.h"
#include <boost/bind.hpp>
#include "Types.h"

#ifdef __WIN_PLATFORM
//可以定位到发生内存泄露 所在的文件和具体那一行，用于检测 malloc 分配的内存
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>

//把分配内存的信息保存下来，可以定位到那一行发生了内存泄露。用于检测 new 分配的内存
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

_inline void EnableMemLeakCheck()
{
	//该语句在程序退出时自动调用 _CrtDumpMemoryLeaks(),用于多个退出出口的情况.
	//如果只有一个退出位置，可以在程序退出之前调用 _CrtDumpMemoryLeaks()
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
#endif //__WIN_PLATFORM

//#define _WIN_SERIVCE_RUN_

bool Run(const char* path)
{
	CServiceUnit* unit = new CServiceUnit(path);
	if(unit)
		unit->Run();
	_SAFE_DELETE_(unit);
	return true;
}

int main(int argc, char* argv[])
{
#if defined(__WIN_PLATFORM) && defined(_DEBUG)
	EnableMemLeakCheck();
	//_CrtSetBreakAlloc(28428);
#endif //__WIN_PLATFORM

#ifdef __WIN_PLATFORM
#ifdef _WIN_SERIVCE_RUN_
	string config = "";
	if(argc > 2)
		config = argv[2];
	else
		config = "E:\\Kernel\\server\\config\\server_win.conf";
#else
	string config = "";
	if(argc >= 2)
		config = argv[1];
	else
		config = "..\\config\\server_win.conf";
#endif
#else
	string config = "";
	if(argc >= 2)
		config = argv[1];
	else
		config = "/work/Kernel/server/config/server.conf";
#endif

#ifdef __WIN_PLATFORM
#ifdef _WIN_SERIVCE_RUN_
	// 以服务的方式运行
	TCHAR szServiceName[128] = _T("Server");
	TCHAR szServiceDesc[512] = _T("服务器");
	InitWinService(szServiceName, boost::bind(&Run, _1), config.c_str(), szServiceDesc);

	if(argc > 1)
	{
		if (strcmp(argv[1], "install") == 0 || strcmp(argv[1], "--install") == 0)  
		{  
			WinInstall();
		}  
		else if(strcmp(argv[1], "uninstall") == 0 || strcmp(argv[1], "--uninstall") == 0)
		{  
			WinUninstall();
		} 
	}
	else
	{
		WinStart();
	}
#else
	Run(config.c_str());
#endif //_WIN_SERIVCE_RUN_
#else
	Run(config.c_str());
#endif //__WIN_PLATFORM

	return 0;
}
