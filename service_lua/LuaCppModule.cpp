#include "LuaCppModule.h"
#include "LuaCppManager.h"
#include "ByteArray.h"
#include "LuaEventOut.h"
#include "LuaClientEventOut.h"
#include "Define.h"
#include "STLPolicy.h"
#include "LuaCppDefine.h"

#include "ServerLog.h"
#include "Exception.h"
#include "IEventOut.h"
#include "IClientOut.h"
#include "BlockQueue.h"

#ifdef _OPEN_DB_MODULE_
#ifdef __MSSQL_DB_MODULE__
#include "MSSqlPool.h"
#include "MSSqlHelper.h"
#endif
#ifdef __MYSQL_DB_MODULE__
#include "MysqlHelper.h"
#include "MysqlPool.h"
#endif
#ifdef __REDIS_DB_MODULE__
#include "RedisHelper.h"
#include "RedisPool.h"
#endif
#endif //_OPEN_DB_MODULE_

//////////////////////////////////////////////////////////////////////////
using namespace Kernel;

// lua 错误回调函数
int PcallbackErrFun(lua_State* L)
{
	lua_Debug debug = { 0 };
	lua_getstack(L, 2, &debug);
	lua_getinfo(L, "Sln", &debug);

	std::string err = lua_tostring(L, -1);
	lua_pop(L, 1);
	string msg;
	char tmp[1024] = { 0 };
	int ret = snprintf(tmp, sizeof(tmp), "%s line: %d ", debug.short_src, debug.currentline);
	if(debug.name != 0)
		ret = snprintf(tmp + ret, sizeof(tmp) - ret, "(%s %s) ", debug.namewhat, debug.name);
	snprintf(tmp + ret, sizeof(tmp) - ret, "[%s]", err.c_str());
	lua_pushstring(L, tmp);
	//WRITE_LOG_ERROR("run lua %s ", tmp);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
static char* lua_type_tostring(lua_State *L, int index, char* tmp)
{
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TNUMBER:
		snprintf(tmp, sizeof(tmp), "%.2f ", lua_tonumber(L, index));
		break;
	case LUA_TSTRING:
		snprintf(tmp, sizeof(tmp), "%s ", lua_tostring(L, index));
		break;
	case LUA_TBOOLEAN:
		snprintf(tmp, sizeof(tmp), "%s ", lua_toboolean(L, index) == 0 ? "false" : "true");
		break;
	default :
		snprintf(tmp, sizeof(tmp), "unknow ");
		break;
	} //switch

	return tmp;
}

//////////////////////////////////////////////////////////////////////////
static int printtb(lua_State *L, int index, char* buf)  
{
	int len = 0;
	len += snprintf(buf + len, 256, "{ ");

	// Push another reference to the table on top of the stack (so we know  
	// where it is, and this function can work for negative, positive and  
	// pseudo indices  
	lua_pushvalue(L, index);  
	// stack now contains: -1 => table  
	lua_pushnil(L);  
	// stack now contains: -1 => nil; -2 => table  
	while (lua_next(L, -2))  
	{  
		// stack now contains: -1 => value; -2 => key; -3 => table  
		// copy the key so that lua_tostring does not modify the original  
		lua_pushvalue(L, -2);

		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table  
		//printf("%s => %s \n", lua_tostring(L, -1), lua_tostring(L, -2));
		char tmp[128] = { 0 };
		len += snprintf(buf + len, 256, "%s = %s ", lua_tostring(L, -1), lua_type_tostring(L, -2, tmp));

		// pop value + copy of key, leaving original key  
		lua_pop(L, 2);  
		// stack now contains: -1 => key; -2 => table  
	}  
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key  
	// but does not push anything.)  
	// Pop table  
	lua_pop(L, 1);  
	// Stack is now the same as it was on entry to this function  

	len += snprintf(buf + len, 256, "} ");
	return len;
}

//////////////////////////////////////////////////////////////////////////
static int log(lua_State* L, int level)
{
	//返回栈中元素的个数  
	char buf[2048] = { 0 };
	int size = 0;
	bool table = false;
	
	int n = lua_gettop(L);
	for(int i = 1; i <= n; ++i)
	{
		int len = 0;
		char tmp[512] = { 0 };
		int type = lua_type(L, i);
		switch (type)
		{
		case LUA_TNIL:
		case LUA_TNUMBER:
			len = snprintf(tmp, sizeof(tmp), "%d ", (int)lua_tointeger(L, i));
			break;
		case LUA_TSTRING:
			len = snprintf(tmp, sizeof(tmp), "%s ", lua_tolstring(L, i, NULL));
			break;
		case LUA_TBOOLEAN:
			len = snprintf(tmp, sizeof(tmp), "%s ", lua_toboolean(L, i) == 0 ? "false" : "true");
			break;
		case LUA_TTABLE:
			if(table)
				len = snprintf(tmp, sizeof(tmp), "Only output one table ");
			else
				len = printtb(L, n, tmp);
			table = true;
			break;
		default :
			len = snprintf(tmp, sizeof(tmp), "unknow ");
			break;
		} //switch

		snprintf(buf + size, sizeof(tmp), "%s", tmp);
		size += len;
	}

	// 获取堆栈信息
	lua_Debug debug = { 0 };
	lua_getstack(L, 1, &debug);
	lua_getinfo(L, "Sln", &debug);

	// 记录日志
	WriteLog(debug.short_src, debug.currentline, level, buf);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
int log_normal(lua_State* L)
{
	return log(L, LOG_LEVEL_NORMAL);
}

//////////////////////////////////////////////////////////////////////////
int log_debug(lua_State* L)
{
	return log(L, LOG_LEVEL_DEBUG);
}

//////////////////////////////////////////////////////////////////////////
int log_warning(lua_State* L)
{
	return log(L, LOG_LEVEL_WARNING);
}

//////////////////////////////////////////////////////////////////////////
int log_error(lua_State* L)
{
	return log(L, LOG_LEVEL_ERROR);
}

//////////////////////////////////////////////////////////////////////////
double GetTime()
{
	return (double)SystemInfo::GetTime();
}

//////////////////////////////////////////////////////////////////////////
CLuaCppModule::CLuaCppModule(string serviceName, string scriptRootPath, string serverRootPath)
:L(lua_open())
,m_ServiceName(serviceName)
,m_ScriptRootPath(scriptRootPath)
,m_ServerRootPath(serverRootPath)
,m_Status(SERVICE_STATUS_NULL)
{
	assert(L);
	luaL_openlibs(L);
    luabind::open(L);
}

//////////////////////////////////////////////////////////////////////////
CLuaCppModule::~CLuaCppModule()
{
	lua_close(L);
	m_Status = SERVICE_STATUS_NULL;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppModule::Init()
{
	if(m_Status >= SERVICE_STATUS_INIT)
		return true;

	try
	{
		// 加载主脚本文件
		LoadFile("Main.lua");
		//luaL_dofile(L, "Main.lua");

		// 注册所有c++调用相关
		RegisterAll();

		// 注册错误回调函数
		luabind::set_pcall_callback(PcallbackErrFun);
		lua_pushcfunction(L, PcallbackErrFun);

		// 设置环境变量
		luabind::call_function<void>(L, "SetEnv", SCRIPT_ROOT_PATH, m_ScriptRootPath);
		luabind::call_function<void>(L, "SetEnv", SERVER_ROOT_PATH, m_ServerRootPath);
		luabind::call_function<void>(L, "SetEnv", SERVICE_NAME, m_ServiceName);

		// 添加服务包路径
		string path = m_ServerRootPath + "/" + m_ServiceName + "_service/";
		AddPackagePath(path, false);

		// 设置平台环境
#if defined(__WIN_PLATFORM)
		luabind::call_function<bool>(L, "SetPlatform", "windows");
#elif defined(__LINUX_PLATFORM)
		luabind::call_function<bool>(L, "SetPlatform", "linux");
#elif defined(__MACOS_PLATFORM)
		luabind::call_function<bool>(L, "SetPlatform", "macos");
#else
		luabind::call_function<bool>(L, "SetPlatform", "other");
#endif

		LoadFile("LoadModule.lua");
		LoadFile("ServerEvent.lua");

		// 初始化
		luabind::call_function<bool>(L, "OnInit");
	}
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
		return false;
	}

	m_Status = SERVICE_STATUS_INIT;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppModule::Start()
{
	if(m_Status >= SERVICE_STATUS_START)
		return true;

	try
	{
		luabind::call_function<bool>(L, "OnStart");
	}
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}

	m_Status = SERVICE_STATUS_START;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppModule::Stop()
{
	if(m_Status == SERVICE_STATUS_STOP || m_Status == SERVICE_STATUS_NULL)
		return true;

	try
	{
		luabind::call_function<bool>(L, "OnStop");
	}
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}

	m_Status = SERVICE_STATUS_STOP;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppModule::Reload()
{
	try
	{
		luabind::call_function<bool>(L, "OnReload");
	}
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppModule::LocalCreateService(string serviceName, int result) /*= 0*/
{
	try 
	{ 
		// 创建失败，删除服务
		if(result > 0)
			CLuaCppManager::Instance().DeleteService(serviceName);

		luabind::call_function<void>(L, "LocalCreateService", serviceName, result);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppModule::LocalDeleteService(string serviceName, int result) /*= 0*/
{
	try 
	{ 
		luabind::call_function<void>(L, "LocalDeleteService", serviceName, result);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppModule::RemoteCreateService(string serviceName, uint64_t sessioinID, const Kernel::CAddress* addr) /*= 0*/
{
	try 
	{ 
		luabind::call_function<void>(L, "RemoteCreateService", serviceName, (double)sessioinID, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppModule::RemoteDeleteService(string serviceName, bool result) /*= 0*/
{
	try 
	{ 
		luabind::call_function<void>(L, "RemoteDeleteService", serviceName, result);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
	}
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppModule::LoadFile(const char* fileName)
{
	if(fileName == NULL)
		return false;

	string path = "";
	path.append(m_ServerRootPath.c_str());
	if(path[m_ServerRootPath.length() - 1] != '/' && path[m_ServerRootPath.length() - 1] != '\\')
		path.append("/");
	path.append(fileName);
	int ret = luaL_dofile(L, path.c_str());
	if(ret == 0)
		return true;

	WRITE_LOG_ERROR("%s module Error: %s", m_ServiceName.c_str(), lua_tostring(L, -1));
	assert(false);
	return false;
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppModule::AddPackagePath(string path, bool is_cpath /*= false*/)
{
	luabind::call_function<void>(L, "AddPackagePath", path, is_cpath);
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppModule::SetEnv(string key, string val)
{
	luabind::call_function<void>(L, "SetEnv", key, val);
}

//////////////////////////////////////////////////////////////////////////
string CLuaCppModule::GetEnv(string key)
{
	return luabind::call_function<string>(L, "GetEnv", key);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppModule::RegisterAll()
{
	try
	{
		// 自定义函数
		lua_register(L, "log_normal", log_normal);
		lua_register(L, "log_debug", log_debug);
		lua_register(L, "log_warning", log_warning);
		lua_register(L, "log_error", log_error);

		// event out
		module(L)
			[
#ifdef __MSSQL_DB_MODULE__
				def("GetMSSqlPool", &GetMSSqlPool),
#endif //__MSSQL_DB_MODULE__

#ifdef __MYSQL_DB_MODULE__
				def("GetMysqlPool", &GetMysqlPool),
#endif //__MYSQL_DB_MODULE__

#ifdef __REDIS_DB_MODULE__
				def("GetRedisPool", &GetRedisPool),
#endif //__REDIS_DB_MODULE__

				def("GetServerLuaOut", &GetServerLuaOut),
				def("GetClientLuaOut", &GetClientLuaOut)
			];

		module(L, "Options")
			[
				def("Wait", &SocketOpt::Wait),
				def("RandSeed", &SystemInfo::RandSeed),
				def("GetTick", &SystemInfo::GetTick),
				//def("GetTime", &SystemInfo::GetTime)
				def("GetTime", &GetTime)
			];

		// CByteArray
		module(L)
			[
				class_<CByteArray>("CByteArray")
				.def(constructor<>())
				.def(constructor<unsigned int>())
				.def("Free", &CByteArray::Free)
				.def("GetByte", &CByteArray::GetByte)
				.def("GetChar", &CByteArray::GetChar)
				.def("GetSize", &CByteArray::GetSize)
				.def("GetLength", &CByteArray::GetLength)
				.def("SetLength", &CByteArray::SetLength)
				.def("GetString", &CByteArray::GetString)
				.def("ToString", &CByteArray::ToString)
				.def("Reset", &CByteArray::Reset)
			];

		// SocketType enum
		module(L)
			[
				class_<SocketType>("SocketType")
				.enum_("constants")
				[
					value("SOCKET_TYPE_NULL", 0),
					value("SOCKET_TYPE_TCP", 1),
					value("SOCKET_TYPE_UDP", 2),
					value("SOCKET_TYPE_UDT", 3),
					value("SOCKET_TYPE_ICMP", 4)
				]
			];

		// LogLevel enum
		module(L)
			[
				class_<LogLevel>("LogLevel")
				.enum_("constants")
				[
					value("LOG_LEVEL_NONE", 0),
					value("LOG_LEVEL_NORMAL", 1),
					value("LOG_LEVEL_NORMAL_SYSTEM", 2),
					value("LOG_LEVEL_DEBUG", 3),
					value("LOG_LEVEL_DEBUG_SYSTEM", 4),
					value("LOG_LEVEL_WARNING", 5),
					value("LOG_LEVEL_WARNING_SYSTEM", 6),
					value("LOG_LEVEL_ERROR", 7),
					value("LOG_LEVEL_ERROR_SYSTEM", 8),
					value("LOG_LEVEL_MAX", 9)
				]
			];

		// SendDataLevel enum
		module(L)
			[
				class_<SendDataLevel>("SendDataLevel")
				.enum_("constants")
				[
					value("DATA_LEVEL_NORMAL", 0),
					value("DATA_LEVEL_IMPORTANT", 1),
					value("DATA_LEVEL_VERY_IMPORTANT", 2),
					value("DATA_LEVEL_NOT_DISCARD", 3)
				]
			];

		// Kernel::CAddress class
		module(L)
			[
				class_<CAddress>("CAddress")
				.def(constructor<>())
				.def(constructor<unsigned short, SocketType>())
				.def(constructor<const char*, unsigned short, SocketType>())
				.def("Set", &CAddress::Set)
				.def("SetSockAddr", (void(CAddress::*)(const char*, unsigned short, SocketType))&CAddress::SetSockAddr)
				.def("SetSockPort", &CAddress::SetSockPort)
				.def("SetAddr", &CAddress::SetAddr)
				.def("SetType", &CAddress::SetType)
				.def("ConvertSockAddr", &CAddress::ConvertSockAddr)
				.def("GetStrIP", &CAddress::GetStrIP)
				.def("GetIP", &CAddress::GetIP)
				.def("GetPort", &CAddress::GetPort)
				.def("GetAddrStr", &CAddress::GetAddrStr)
				.def("GetType", &CAddress::GetType)
				.def("Reset", &CAddress::Reset)
			];

		// CLuaEventOut class
		module(L)
			[
				class_<CLuaEventOut>("CLuaEventOut")
				.scope
				[ 
					def("Instance", &CLuaEventOut::Instance)
				]
				.def("CreateBindSocket", (int(CLuaEventOut::*)(Kernel::CAddress, int, bool))&CLuaEventOut::CreateBindSocket)
				.def("CreateBindSocket", (int(CLuaEventOut::*)(const char*, unsigned short, int, bool))&CLuaEventOut::CreateBindSocket)
				.def("Send", (bool(CLuaEventOut::*)(double, const char*, unsigned int))&CLuaEventOut::Send)
				.def("Send", (bool(CLuaEventOut::*)(double, const char*, unsigned int, const Kernel::CAddress*))&CLuaEventOut::Send)
				.def("Send", (bool(CLuaEventOut::*)(double, const char*, unsigned int, const Kernel::CAddress*, int))&CLuaEventOut::Send)
				.def("Broadcast", (void(CLuaEventOut::*)(const char*, unsigned int))&CLuaEventOut::Broadcast)
				.def("Broadcast", (void(CLuaEventOut::*)(const char*, unsigned int, const std::list<double>&))&CLuaEventOut::Broadcast)
				.def("Close", (void(CLuaEventOut::*)(double))&CLuaEventOut::Close)
				.def("Shutdown", (void(CLuaEventOut::*)(double))&CLuaEventOut::Shutdown)
				.def("SetTimer", (bool(CLuaEventOut::*)(unsigned int, unsigned int, unsigned int))&CLuaEventOut::SetTimer)
				.def("SetTimer", (bool(CLuaEventOut::*)(unsigned int, unsigned int, unsigned int, const char*, unsigned short))&CLuaEventOut::SetTimer)
				.def("KillTimer", &CLuaEventOut::KillTimer)
				.def("PostDatabaseRequest", (bool(CLuaEventOut::*)(unsigned int, unsigned int, const char*, unsigned short))&CLuaEventOut::PostDatabaseRequest)
				.def("PostDatabaseEvent", (bool(CLuaEventOut::*)(double, unsigned int, const char*, unsigned short, bool))&CLuaEventOut::PostDatabaseEvent)
				.def("PostSelfEvent", &CLuaEventOut::PostSelfEvent)
				.def("SetUserToSession", (void(CLuaEventOut::*)(double, double))&CLuaEventOut::SetUserToSession)
				.def("GetSessionUserID", (double(CLuaEventOut::*)(double))&CLuaEventOut::GetSessionUserID)
			];

		// CLuaClientEventOut class
		module(L)
			[
				class_<CLuaClientEventOut>("CLuaClientEventOut")
				.scope
				[ 
					def("Instance", &CLuaClientEventOut::Instance)
				]
				.def("Connect", (double(CLuaClientEventOut::*)(const Kernel::CAddress*, double)) &CLuaClientEventOut::Connect)
				.def("Connect", (double(CLuaClientEventOut::*)(const char*, unsigned short, SocketType, double))&CLuaClientEventOut::Connect)
				.def("SendData", (bool(CLuaClientEventOut::*)(double, const char*, unsigned short))&CLuaClientEventOut::SendData)
				.def("SendData", (bool(CLuaClientEventOut::*)(double, const char*, unsigned short, const Kernel::CAddress*))&CLuaClientEventOut::SendData)
				.def("SendData", (bool(CLuaClientEventOut::*)(double, const char*, unsigned short, const Kernel::CAddress*, int))&CLuaClientEventOut::SendData)
				.def("SendData", (bool(CLuaClientEventOut::*)(double, const char*, unsigned short, SocketType))&CLuaClientEventOut::SendData)
				.def("Close", (void(CLuaClientEventOut::*)(double))&CLuaClientEventOut::Close)
				.def("Shutdown", (void(CLuaClientEventOut::*)(double))&CLuaClientEventOut::Shutdown)
				.def("SetTimer", (bool(CLuaClientEventOut::*)(unsigned int, unsigned int, unsigned int))&CLuaClientEventOut::SetTimer)
				.def("SetTimer", (bool(CLuaClientEventOut::*)(unsigned int, unsigned int, unsigned int, const char*, unsigned short))&CLuaClientEventOut::SetTimer)
				.def("KillTimer", &CLuaClientEventOut::KillTimer)
				.def("SetUserToSession", (void(CLuaClientEventOut::*)(double, double))&CLuaClientEventOut::SetUserToSession)
				.def("GetSessionUserID", (double(CLuaClientEventOut::*)(double))&CLuaClientEventOut::GetSessionUserID)
			];

		//////////////////////////////////////////////////////////////////////////
		// 数据库导入

#ifdef _OPEN_DB_MODULE_
		// DbHelperStatus enum
		module(L)
			[
				class_<DbHelperStatus>("DbHelperStatus")
				.enum_("constants")
				[
					value("HELPER_STATUS_NULL", 0),
					value("HELPER_STATUS_INIT", 1),
					value("HELPER_STATUS_CONNECTED", 2),
					value("HELPER_STATUS_FREE", 3),
					value("HELPER_STATUS_IDLE", 4),
					value("HELPER_STAUTS_STOP", 5),
					value("HELPER_STATUS_MAX", 6)
				]
			];

		// SQL SERVER
#ifdef __MSSQL_DB_MODULE__
		// ParameterDirectionEnum enum
		module(L)
			[
				class_<ParameterDirectionEnum>("ParameterDirectionEnum")
				.enum_("constants")
				[
					value("dbParamInput", 1),
					value("dbParamOutput", 2),
					value("dbParamInputOutput", 3),
					value("dbParamReturnValue", 4)
				]
			];

		// CMSSqlPool
		module(L)
			[
				class_<CMSSqlPool>("CMSSqlPool")
				.def(constructor<string&>())
				.scope
				[ 
					def("Instance", &CMSSqlPool::Instance)
				]
				.def("GetPoolName", &CMSSqlPool::GetPoolName)
				.def("Connect", (int(CMSSqlPool::*)(const char*, const char*))&CMSSqlPool::Connect)
				.def("Connect", (int(CMSSqlPool::*)(const char*, unsigned short, const char*))&CMSSqlPool::Connect)
				.def("Connect", (int(CMSSqlPool::*)(const char*, const char*, const char*, const char*, unsigned short))&CMSSqlPool::Connect)
				.def("GetMSHelper", (CMSSqlHelper&(CMSSqlPool::*)(int))&CMSSqlPool::GetMSHelper)
				.def("GetMSHelper", (CMSSqlHelper&(CMSSqlPool::*)(const char*))&CMSSqlPool::GetMSHelper)
				.def("FreeMSHelper", &CMSSqlPool::FreeMSHelper)
				.def("Close", &CMSSqlPool::Close)
				.def("Clear", &CMSSqlPool::Clear)
			];

		// CMSSqlHelper class
		module(L)
			[
				class_<CMSSqlHelper>("CMSSqlHelper")
				.def(constructor<>())
				.def("Init", &CMSSqlHelper::Init)
				.def("GetDbName", &CMSSqlHelper::GetDbName)
				.def("GetHelperAddrInfo", &CMSSqlHelper::GetHelperAddrInfo)
				.def("SetStatus", &CMSSqlHelper::SetStatus)
				.def("GetStatus", &CMSSqlHelper::GetStatus)
				.def("SetHelperNO", &CMSSqlHelper::SetHelperNO)
				.def("GetHelperNO", &CMSSqlHelper::GetHelperNO)
				.def("Connect", (bool(CMSSqlHelper::*)(const char*))&CMSSqlHelper::Connect)
				.def("Connect", (bool(CMSSqlHelper::*)(const char*, unsigned short))&CMSSqlHelper::Connect)
				.def("Connect", (bool(CMSSqlHelper::*)(const char*, const char*, const char*, const char*))&CMSSqlHelper::Connect)
				.def("Close", &CMSSqlHelper::Close)
				.def("Execute", &CMSSqlHelper::Execute)
				.def("GetRows", &CMSSqlHelper::GetRows)
				.def("GetConlums", &CMSSqlHelper::GetConlums)
				.def("MoveNext", &CMSSqlHelper::MoveNext)
				.def("IsRecordsEnd", &CMSSqlHelper::IsRecordsEnd)
				.def("ResetRecordset", &CMSSqlHelper::ResetRecordset)
				.def("GetAffectedRows", &CMSSqlHelper::GetAffectedRows)
				.def("DataSeek", &CMSSqlHelper::DataSeek)
				.def("FieldSeek", &CMSSqlHelper::FieldSeek)
				.def("GetValus", &CMSSqlHelper::GetValus)
				.def("GetBool", (bool(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetBool)
				.def("GetBool", (bool(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetBool)
				.def("GetByte", (unsigned char(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetByte)
				.def("GetByte", (unsigned char(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetByte)
				.def("GetShort", (short(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetShort)
				.def("GetShort", (short(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetShort)
				.def("GetUshort", (unsigned short(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetUshort)
				.def("GetUshort", (unsigned short(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetUshort)
				.def("GetInt", (int(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetInt)
				.def("GetInt", (int(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetInt)
				.def("GetUInt", (unsigned int(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetUInt)
				.def("GetUInt", (unsigned int(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetUInt)
				.def("GetLong", (long(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetLong)
				.def("GetLong", (long(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetLong)
				.def("GetULong", (unsigned long(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetULong)
				.def("GetULong", (unsigned long(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetULong)
				.def("GetLonglong", (long long(CMSSqlHelper::*)(unsigned int, CByteArray&))&CMSSqlHelper::GetLonglong)
				.def("GetLonglong", (long long(CMSSqlHelper::*)(const char*, CByteArray&))&CMSSqlHelper::GetLonglong)
				.def("GetFloat", (float(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetFloat)
				.def("GetFloat", (float(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetFloat)
				.def("GetDouble", (double(CMSSqlHelper::*)(unsigned int))&CMSSqlHelper::GetDouble)
				.def("GetDouble", (double(CMSSqlHelper::*)(const char*))&CMSSqlHelper::GetDouble)
				.def("GetString", (const char*(CMSSqlHelper::*)(unsigned int, CByteArray&))&CMSSqlHelper::GetString)
				.def("GetString", (const char*(CMSSqlHelper::*)(const char*, CByteArray&))&CMSSqlHelper::GetString)
				.def("GetBlob", (unsigned char*(CMSSqlHelper::*)(unsigned int, unsigned char*, unsigned int))&CMSSqlHelper::GetBlob)
				.def("GetBlob", (unsigned char*(CMSSqlHelper::*)(const char*, unsigned char*, unsigned int))&CMSSqlHelper::GetBlob)
				.def("GetTime", (const char*(CMSSqlHelper::*)(unsigned int, CByteArray&))&CMSSqlHelper::GetTime)
				.def("GetTime", (const char*(CMSSqlHelper::*)(const char*, CByteArray&))&CMSSqlHelper::GetTime)
			
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, bool, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, char, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, unsigned char, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, int, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, unsigned int, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, long, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, unsigned long, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddLLParameter", &CMSSqlHelper::AddLLParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, short, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, unsigned short, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, float, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, double, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddParameter", (void(CMSSqlHelper::*)(const char*, const char*, unsigned int, ParameterDirectionEnum))&CMSSqlHelper::AddParameter)
				.def("AddBlobParameter", (void(CMSSqlHelper::*)(const char*, const unsigned char*, unsigned int, ParameterDirectionEnum))&CMSSqlHelper::AddBlobParameter)
				.def("AddDateParameter", (void(CMSSqlHelper::*)(const char*, const char*, unsigned long, ParameterDirectionEnum))&CMSSqlHelper::AddDateParameter)
				.def("AddOutParameter", (void(CMSSqlHelper::*)(const char*, CByteArray&, ParameterDirectionEnum))&CMSSqlHelper::AddOutParameter)

				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, bool&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, char&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, unsigned char&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, short&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, unsigned short&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, int&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, unsigned int&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, long&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, unsigned long&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, float&))&CMSSqlHelper::GetParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, double&))&CMSSqlHelper::GetParameter)
				.def("GetLLParameter", &CMSSqlHelper::GetLLParameter)
				.def("GetParameter", (void(CMSSqlHelper::*)(const char*, CByteArray&))&CMSSqlHelper::GetParameter)
				
				.def("AddReturnParameter", &CMSSqlHelper::AddReturnParameter)
				.def("ResetParameter", &CMSSqlHelper::ResetParameter)
				.def("GetReturnValue", &CMSSqlHelper::GetReturnValue)
				.def("IsConnectError", &CMSSqlHelper::IsConnectError)
				.def("MoveFirst", &CMSSqlHelper::MoveFirst)
				.def("MoveNextRecordset", &CMSSqlHelper::MoveNextRecordset)

			];
#endif // __MSSQL_DB_MODULE__

		// Mysql
#ifdef __MYSQL_DB_MODULE__
		// CMysqlPool
		module(L)
			[
				class_<CMysqlPool>("CMysqlPool")
				.def(constructor<string&>())
				.scope
				[ 
					def("Instance", &CMysqlPool::Instance)
				]
				.def("GetPoolName", &CMysqlPool::GetPoolName)
				.def("Connect", (int(CMysqlPool::*)(const char*, const char*))&CMysqlPool::Connect)
				.def("Connect", (int(CMysqlPool::*)(const char*, unsigned short, const char*))&CMysqlPool::Connect)
				.def("Connect", (int(CMysqlPool::*)(const char*, const char*, const char*, const char*, unsigned short))&CMysqlPool::Connect)
				.def("GetMysqlHelper", (CMysqlHelper&(CMysqlPool::*)(int))&CMysqlPool::GetMysqlHelper)
				.def("GetMysqlHelper", (CMysqlHelper&(CMysqlPool::*)(const char*))&CMysqlPool::GetMysqlHelper)
				.def("FreeMysqlHelper", &CMysqlPool::FreeMysqlHelper)
				.def("Close", &CMysqlPool::Close)
				.def("Clear", &CMysqlPool::Clear)
			];

		// CMysqlHelper class
		module(L)
			[
				class_<CMysqlHelper>("CMysqlHelper")
				.def(constructor<>())
				.def("Init", &CMysqlHelper::Init)
				.def("GetDbName", &CMysqlHelper::GetDbName)
				.def("GetHelperAddrInfo", &CMysqlHelper::GetHelperAddrInfo)
				.def("SetStatus", &CMysqlHelper::SetStatus)
				.def("GetStatus", &CMysqlHelper::GetStatus)
				.def("SetHelperNO", &CMysqlHelper::SetHelperNO)
				.def("GetHelperNO", &CMysqlHelper::GetHelperNO)
				.def("Connect", (bool(CMysqlHelper::*)(const char*))&CMysqlHelper::Connect)
				.def("Connect", (bool(CMysqlHelper::*)(const char*, unsigned short))&CMysqlHelper::Connect)
				.def("Connect", (bool(CMysqlHelper::*)(const char*, const char*, const char*, const char*))&CMysqlHelper::Connect)
				.def("Close", &CMysqlHelper::Close)
				.def("Execute", &CMysqlHelper::Execute)
				.def("GetRows", &CMysqlHelper::GetRows)
				.def("GetConlums", &CMysqlHelper::GetConlums)
				.def("MoveNext", &CMysqlHelper::MoveNext)
				.def("IsRecordsEnd", &CMysqlHelper::IsRecordsEnd)
				.def("ResetRecordset", &CMysqlHelper::ResetRecordset)
				.def("GetAffectedRows", &CMysqlHelper::GetAffectedRows)
				.def("DataSeek", &CMysqlHelper::DataSeek)
				.def("FieldSeek", &CMysqlHelper::FieldSeek)
				.def("GetValus", &CMysqlHelper::GetValus)
				.def("GetBool", (bool(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetBool)
				.def("GetBool", (bool(CMysqlHelper::*)(const char*))&CMysqlHelper::GetBool)
				.def("GetByte", (unsigned char(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetByte)
				.def("GetByte", (unsigned char(CMysqlHelper::*)(const char*))&CMysqlHelper::GetByte)
				.def("GetShort", (short(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetShort)
				.def("GetShort", (short(CMysqlHelper::*)(const char*))&CMysqlHelper::GetShort)
				.def("GetUshort", (unsigned short(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetUshort)
				.def("GetUshort", (unsigned short(CMysqlHelper::*)(const char*))&CMysqlHelper::GetUshort)
				.def("GetInt", (int(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetInt)
				.def("GetInt", (int(CMysqlHelper::*)(const char*))&CMysqlHelper::GetInt)
				.def("GetUInt", (unsigned int(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetUInt)
				.def("GetUInt", (unsigned int(CMysqlHelper::*)(const char*))&CMysqlHelper::GetUInt)
				.def("GetLong", (long(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetLong)
				.def("GetLong", (long(CMysqlHelper::*)(const char*))&CMysqlHelper::GetLong)
				.def("GetULong", (unsigned long(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetULong)
				.def("GetULong", (unsigned long(CMysqlHelper::*)(const char*))&CMysqlHelper::GetULong)
				.def("GetLonglong", (long long(CMysqlHelper::*)(unsigned int, CByteArray&))&CMysqlHelper::GetLonglong)
				.def("GetLonglong", (long long(CMysqlHelper::*)(const char*, CByteArray&))&CMysqlHelper::GetLonglong)
				.def("GetFloat", (float(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetFloat)
				.def("GetFloat", (float(CMysqlHelper::*)(const char*))&CMysqlHelper::GetFloat)
				.def("GetDouble", (double(CMysqlHelper::*)(unsigned int))&CMysqlHelper::GetDouble)
				.def("GetDouble", (double(CMysqlHelper::*)(const char*))&CMysqlHelper::GetDouble)
				.def("GetString", (const char*(CMysqlHelper::*)(unsigned int, CByteArray&))&CMysqlHelper::GetString)
				.def("GetString", (const char*(CMysqlHelper::*)(const char*, CByteArray&))&CMysqlHelper::GetString)

				.def("AddParameter", (void(CMysqlHelper::*)(char&, bool isBindParam))&CMysqlHelper::AddParameter)
				.def("AddParameter", (void(CMysqlHelper::*)(short&, bool isBindParam))&CMysqlHelper::AddParameter)
				.def("AddParameter", (void(CMysqlHelper::*)(float&, bool isBindParam))&CMysqlHelper::AddParameter)
				.def("AddParameter", (void(CMysqlHelper::*)(double&, bool isBindParam))&CMysqlHelper::AddParameter)
				.def("AddParameter", (void(CMysqlHelper::*)(int&, bool isBindParam))&CMysqlHelper::AddParameter)
				.def("AddParameter", (void(CMysqlHelper::*)(CByteArray&, bool isBindParam))&CMysqlHelper::AddParameter)
				.def("AddLLParameter", (void(CMysqlHelper::*)(CByteArray&, bool isBindParam))&CMysqlHelper::AddLLParameter)
				.def("AddBlobParameter", (void(CMysqlHelper::*)(CByteArray& bytes, bool isBindParam))&CMysqlHelper::AddBlobParameter)
				.def("AddDateParameter", (void(CMysqlHelper::*)(CByteArray& bytes, bool isBindParam))&CMysqlHelper::AddDateParameter)

				.def("ResetParameter", &CMysqlHelper::StmtPrepare)
				.def("GetReturnValue", &CMysqlHelper::ExecuteStmt)
				.def("MoveNextRecordset", &CMysqlHelper::MoveNextRecordset)

			];
#endif //__MYSQL_DB_MODULE__

#endif //_OPEN_DB_MODULE_

		// service
		module(L)
			[
				class_<CLuaCppManager>("CLuaCppManager")
				.scope
				[ 
					def("Instance", &CLuaCppManager::Instance)
				]
 				.def("CreateService", (bool(CLuaCppManager::*)(string, const Kernel::CAddress*, double))&CLuaCppManager::CreateService)
				.def("DeleteService", &CLuaCppManager::DeleteService)
				.def("IsService", &CLuaCppManager::IsService)
				.def("FindService", &CLuaCppManager::FindService)
				.def("Send", &CLuaCppManager::Send)
				.def("Call", &CLuaCppManager::Call)
			];

		// service
		module(L)
			[
				class_<CDistributedService>("CDistributedService")
				.def("Send", &CDistributedService::Send)
				.def("Call", &CDistributedService::Call)
			];

	}
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("register cpp to lua fail %s", e.what());
		WRITE_LOG_ERROR(lua_tostring(L, -1));
		//printf("AI throw error: err_msg[%s]", lua_tostring(L, -1));
		return false;
	}
	return true;
}
