#include <string.h>
#include "AttemperEvent.h"
#include "ServiceUnit.h"
#include "Buffer.h"
#include "ServerLog.h"
#include "IDBPool.h"
#include "ServerKernel.h"
#include "IDBHelper.h"
#include "Address.h"
#include <fstream>
#include "Ping.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lockfree/queue.hpp>
#include "ConfigParse.h"

using namespace Kernel;
using namespace boost::posix_time;

//////////////////////////////////////////////////////////////////////////
CAttemperEvent *g_AttemperEvent = NULL;
static string g_ReloadScriptName = "server_message_queue";

//////////////////////////////////////////////////////////////////////////
CAttemperEvent::CAttemperEvent()
	:m_EventOut(NULL)
	,m_ClientOut(NULL)
#ifdef _OPEN_LUA_MODULE_
	,m_LuaCppManager(new CLuaCppManager())
	,m_LogicModule(NULL)
	,m_DBModule(NULL)
	//,m_ReloadScriptMQ(open_or_create, g_ReloadScriptName.c_str(), 100, 1024)
	,m_MainLogicName(MAIN_LOGIC)
#endif
{
	g_AttemperEvent = this;
}

//////////////////////////////////////////////////////////////////////////
CAttemperEvent::~CAttemperEvent()
{
#ifdef _OPEN_LUA_MODULE_
	_SAFE_DELETE_(m_LuaCppManager);
	m_LogicModule = NULL;
	m_DBModule = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////
CAttemperEvent& CAttemperEvent::Instance()
{
	assert(g_AttemperEvent);
	return *g_AttemperEvent;
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::LogicThreadInit() /*= 0*/
{

}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnStarted() /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		m_LuaCppManager->Start();
		m_EventOut->PostUserTask(boost::bind(&CAttemperEvent::ReloadScriptTask, this), "reload_script", (unsigned int)-1);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::OnReadyStop() /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		//message_queue::remove(g_ReloadScriptName.c_str());
		m_LuaCppManager->Stop();
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	// 通知真正关闭
	Kernel::CServerKernel::Instance().SetServerRealStop();
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::OnNodeRegisterSuccess(uint64_t sessionID, unsigned int serverId, ServerType type, Kernel::CAddress& addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		luabind::call_function<void>(m_LogicModule->GetLuaState(), "OnNodeRegisterSuccess", (double)sessionID, serverId, type, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnSelfContorlBuf(uint64_t sessionID, void* inBuf, unsigned int inLen, void* out, unsigned int &outLen, unsigned int type /*= 0*/)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnSelfContorlEvent(unsigned int eventID, void* data, unsigned short size) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		// reload脚本事件
		if(eventID == 0)
			return m_LuaCppManager->Reload();

		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnSelfContorlEvent", eventID, string((char*)data, size), size);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", MAIN_LOGIC, e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////
unsigned int CAttemperEvent::OnSelfEncrypt(const char* src, unsigned int srcLen, char* dst) /*= 0*/
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
unsigned int CAttemperEvent::OnSelfDecrypt(const char* src, unsigned int srcLen, char* dst) /*= 0*/
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnConnect(uint64_t sessionID, unsigned int bindID, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnConnect", (double)sessionID, bindID, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnReadEvent(uint64_t sessionID, void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnReadEvent", (double)sessionID, string((char*)data, size), size, (unsigned int)exter, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::OnCloseEvent(uint64_t sessionID, const Kernel::CAddress* addr)/* = 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		luabind::call_function<void>(m_LogicModule->GetLuaState(), "OnCloseEvent", (double)sessionID, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnTimerEvent(unsigned int timerID, unsigned int interval, unsigned int overTime, const void* param /*= NULL*/, unsigned short paramLen /*= 0*/) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnTimerEvent", timerID, interval, overTime, string((char*)param, paramLen), paramLen);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnDataBaseRequest(uint64_t sessionID, unsigned int requestID, void* data, unsigned short size) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_DBModule->GetLuaState(), "OnDataBaseRequest", (double)sessionID, requestID, string((char*)data, size), size);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", DB_LOGIC, e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnDatabaseEvent(uint64_t sessionID, unsigned int dbID, void* data, unsigned short size) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnDatabaseEvent", (double)sessionID, dbID, string((char*)data, size), size);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::OnHttpServerEvent(Kernel::CHttpRequest* httpReq, Kernel::CHttpResponse* httRes) /*= 0*/
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::LocalCreateService(string serviceName, int result) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	m_LogicModule->LocalCreateService(serviceName, result);
#endif
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::LocalDeleteService(string serviceName, int result) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	m_LogicModule->LocalDeleteService(serviceName, result);
#endif
}

//////////////////////////////////////////////////////////////////////////
string CAttemperEvent::QueryServiceQueue(uint64_t sessionID, const void* data, unsigned short size) /*= 0*/
{ 
	return "";
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::RemoteCreateService(string serviceName, uint64_t sessioinID, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	m_LogicModule->RemoteCreateService(serviceName, sessioinID, addr);
#endif
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::RemoteDeleteService(string serviceName, bool result) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	m_LogicModule->RemoteDeleteService(serviceName, result);
#endif
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::ServiceSendData(uint64_t sessionID, string func, const void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		luabind::call_function<void>(m_LogicModule->GetLuaState(), "ServiceSend", (double)sessionID, func, string((char*)data, size), size, (double)exter, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
string CAttemperEvent::ServiceCallData(uint64_t sessionID, string func, const void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<string>(m_LogicModule->GetLuaState(), "ServiceCall", (double)sessionID, func, string((char*)data, size), size, (double)exter, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
	return "";
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::Init()
{
#ifdef _OPEN_LUA_MODULE_
	if(!m_LuaCppManager)
	{
		assert(false);
		return false;
	}

	bool isCreateDB = false;		// 是否创建db虚拟机
	m_LuaCppManager->SetEventOut(m_EventOut);
	m_LuaCppManager->SetClientEventOut(m_ClientOut);

	string selfConfig = "";
	
#ifdef __WIN_PLATFORM
	string scriptRootPath = "E:/Kernel/script_lua";
	string serverRootPath = "E:/Kernel/script_lua/server/";
#elif defined(__LINUX_PLATFORM)
	string scriptRootPath = "/work/Kernel/script_lua";
	string serverRootPath = "/work/Kernel/script_lua/server/";
#endif

	// 读取配置文件
	CConfigParse config(CServiceUnit::Instance().GetPath());
	int configType = config.GetInt("ConfigType");
	//config.Print();
	if(configType == CONFIG_TYPE_ENGINE)
	{
		isCreateDB = config.Get("DBEvent") == "yes" ? true : false;
	}
	else if(configType == CONFIG_TYPE_SERVER)
	{
		string engienPath = config.Get("EnginePath");
		selfConfig = config.Get("SelfConfigPath");

		// 解析引擎配置文件
		if(config.Parse(engienPath))
		{
			//config.Print();
			isCreateDB = config.Get("DBEvent") == "yes" ? true : false;
		}

		// 读取自定义配置文件,获取lua路径配置
		if(selfConfig.length() > 0 && config.Parse(selfConfig))
		{
			//config.Print();
			scriptRootPath = config.Get("ScriptRootPath");
			serverRootPath = config.Get("ServerRootPath");
			m_MainLogicName = config.Get("MainLogic");
		}
	}

	// 设置主逻辑名
	m_EventOut->SetMainLogicName(m_MainLogicName);
	m_LuaCppManager->SetMainLogic(m_MainLogicName);

	// 创建logic模块lua虚拟机
	m_LogicModule = m_LuaCppManager->CreateModule(m_MainLogicName, scriptRootPath, serverRootPath);
	assert(m_LogicModule);
	
	// 创建db模块lua虚拟机
	if(isCreateDB)
	{
		m_DBModule = m_LuaCppManager->CreateModule(DB_LOGIC, scriptRootPath, serverRootPath);
		assert(m_DBModule);
	}

	// 设置接口
	m_LuaCppManager->Init();
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CAttemperEvent::Send(uint64_t sessionID, unsigned short cmd, const char* buf, unsigned int size)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CAttemperEvent::ReloadScriptTask()
{
	try
	{
		std::string data = "reload";
		char buf[1024] = { 0 };
		size_t recv_size = 0;
		//size_t priority = 0;
		boost::posix_time::ptime abstime(second_clock::local_time());
		bool ret = false; //m_ReloadScriptMQ.try_receive(buf, sizeof(buf), recv_size, priority);
		if(ret)
		{
			if(recv_size == data.length() && strncmp(buf, data.c_str(), data.length()) == 0)
			{
				m_EventOut->PostSelfEvent(0, NULL, 0);
				SocketOpt::Wait(1000);
				//m_EventOut->PostDatabaseRequest(0, 0, NULL, 0);
			}
		}
	}
	catch(interprocess_exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

