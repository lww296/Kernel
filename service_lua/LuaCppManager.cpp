#include "LuaCppManager.h"
#include "LuaCppModule.h"
#include "ServerLog.h"
#include "ServerKernel.h"
#include "BlockQueue.h"
#include "DistributedService.h"
#include "LuaCppDefine.h"

using namespace Kernel;

//////////////////////////////////////////////////////////////////////////
static CLuaCppManager* g_LuaCppManager = NULL;

//////////////////////////////////////////////////////////////////////////
CLuaCppManager::CLuaCppManager()
	:m_LuaEventOut(new CLuaEventOut(NULL))
	,m_LuaClientEventOut(new CLuaClientEventOut(NULL))
	,m_EventOut(NULL)
	,m_ClientOut(NULL)
	,m_Status(SERVICE_STATUS_NULL)
	,m_MainLogicName(MAIN_LOGIC)
{
	g_LuaCppManager = this;
}

//////////////////////////////////////////////////////////////////////////
CLuaCppManager::~CLuaCppManager()
{
	Clear();
	_SAFE_DELETE_(m_LuaEventOut);
	_SAFE_DELETE_(m_LuaClientEventOut);
	m_EventOut = NULL;
	m_ClientOut = NULL;
	g_LuaCppManager = NULL;
	m_Status = SERVICE_STATUS_NULL;
}

//////////////////////////////////////////////////////////////////////////
CLuaCppManager& CLuaCppManager::Instance()
{
	assert(g_LuaCppManager);
	return *g_LuaCppManager;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::Init()
{
	m_Status = SERVICE_STATUS_INIT;

	for(DistributeServiceMap::MapIt it = m_DistributedServiceMap.Begin(); it != m_DistributedServiceMap.End(); ++it)
	{
		CDistributedService* service = it->second;
		if(service)
			service->Init();
	}

	for(LuaCppModuleMap::MapIt it = m_LuaCppModuleMap.Begin(); it != m_LuaCppModuleMap.End(); ++it)
	{
		CLuaCppModule* module = it->second;
		if(module)
			module->Init();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::Start()
{
	for(DistributeServiceMap::MapIt it = m_DistributedServiceMap.Begin(); it != m_DistributedServiceMap.End(); ++it)
	{
		CDistributedService* service = it->second;
		if(service)
			service->Start();
	}

	for(LuaCppModuleMap::MapIt it = m_LuaCppModuleMap.Begin(); it != m_LuaCppModuleMap.End(); ++it)
	{
		CLuaCppModule* module = it->second;
		if(module)
			module->Start();
	}

	m_Status = SERVICE_STATUS_START;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::Stop()
{
	for(DistributeServiceMap::MapIt it = m_DistributedServiceMap.Begin(); it != m_DistributedServiceMap.End(); ++it)
	{
		CDistributedService* service = it->second;
		if(service)
			service->Stop();
	}

	for(LuaCppModuleMap::MapIt it = m_LuaCppModuleMap.Begin(); it != m_LuaCppModuleMap.End(); ++it)
	{
		CLuaCppModule* module = it->second;
		if(module)
			module->Stop();
	}

	m_Status = SERVICE_STATUS_STOP;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::Reload()
{
	for(LuaCppModuleMap::MapIt it = m_LuaCppModuleMap.Begin(); it != m_LuaCppModuleMap.End(); ++it)
	{
		CLuaCppModule* module = it->second;
		if(module)
			module->Reload();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
CLuaCppModule* CLuaCppManager::CreateModule(string serviceName, string scriptRootPath, string serverRootPath, bool isCreate /*= false*/)
{
	CLuaCppModule* module = m_LuaCppModuleMap.FindActiveObj(serviceName);
	if(module)
	{
		WRITE_LOG_NORMAL("luacpp module %s existern", serviceName.c_str());
		return module;
	}

	// 创建服务
	CDistributedService* service = new CDistributedService(serviceName, NULL, isCreate);
	assert(service);
	m_DistributedServiceMap.InsertActiveObj(serviceName, service);

	// 创建模块
	module = new CLuaCppModule(serviceName, scriptRootPath, serverRootPath);
	assert(module);
	m_LuaCppModuleMap.InsertActiveObj(serviceName, module);

	// 设置服务模块
	service->SetLuaMoudle(module);

	return module;
}

//////////////////////////////////////////////////////////////////////////
CLuaCppModule* CLuaCppManager::FindModule(string serviceName)
{
	CLuaCppModule* module = m_LuaCppModuleMap.FindActiveObj(serviceName);
	if(module)
		return module;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppManager::DestoryModule(string serviceName)
{
	CLuaCppModule* module = m_LuaCppModuleMap.FindActiveObj(serviceName);
	if(module)
	{
		module->Stop();
		_SAFE_DELETE_(module);
		m_LuaCppModuleMap.RemoveActiveObj(serviceName);
	}
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppManager::Clear()
{
	m_DistributedServiceMap.Release();
	m_LuaCppModuleMap.Release();
	m_Status = SERVICE_STATUS_NULL;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::CreateService(string serviceName, const Kernel::CAddress* remoteAddr /*= NULL*/, double sessionID /*= 0*/)
{
	CDistributedService* service = FindService(serviceName);
	if(service)
	{
		WRITE_LOG_DEBUG("Distributed service already existern");
		return false;
	}
	
	uint64_t id = (uint64_t)sessionID;
	if(remoteAddr == NULL)
	{
		// 创建本地服务
		CLuaCppModule* mainModule = FindModule(m_MainLogicName);
		if(!mainModule)
		{
			WRITE_LOG_ERROR("%s not create", m_MainLogicName.c_str());
			return false;
		}

		// 创建模块
		CLuaCppModule* luaModule = CreateModule(serviceName, mainModule->GetEnv(SCRIPT_ROOT_PATH), mainModule->GetEnv(SERVER_ROOT_PATH), true);
		assert(luaModule);

		service = m_DistributedServiceMap.FindActiveObj(serviceName);
		assert(service);

		// init
		if(m_Status >= SERVICE_STATUS_INIT)
		{
			service->Init();
			luaModule->Init();
		}

		if(!m_EventOut->CreateService(serviceName, CAddress(), service->GetQueue(), id))
		{
			DestoryModule(serviceName);
			return false;
		}

		// ×¢²áÏß³ÌÊÂ¼þ
		m_EventOut->PostUserTask(serviceName, boost::bind(&CDistributedService::Run, service));

		// start
		if(m_Status >= SERVICE_STATUS_START)
		{
			service->Start();
			luaModule->Start();
		}
	}
	else
	{
		assert(remoteAddr);
		m_EventOut->CreateService(serviceName, *remoteAddr, NULL, id);
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
CDistributedService* CLuaCppManager::FindService(string serviceName)
{
	CDistributedService* service = m_DistributedServiceMap.FindActiveObj(serviceName);
	if(service)
		return service;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::IsService(string serviceName)
{
	CDistributedService* service = m_DistributedServiceMap.FindActiveObj(serviceName);
	if(service)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
void CLuaCppManager::DeleteService(string serviceName)
{
	CDistributedService* service = FindService(serviceName);
	if(!service)
	{
		m_EventOut->DeleteService(serviceName);
		return;
	}

	bool local = service->IsLocalService();
	service->Stop();
	m_EventOut->DeleteService(serviceName);

	// 本地服务等待线程队列退出
	if(local)
		SocketOpt::Wait(100);

	m_DistributedServiceMap.DelActiveObj(serviceName);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaCppManager::Send(string srcServiceName, string detServiceName, string func, string param)
{
	CDistributedService* srcService = FindService(srcServiceName);
	if(!srcService)
		return false;

	if(srcService->IsLocalService())
		return srcService->Send(detServiceName, func, param);
	else
		return m_EventOut->SendData(srcServiceName, detServiceName, func, param.c_str(), param.length());
	return true;
}

//////////////////////////////////////////////////////////////////////////
string CLuaCppManager::Call(string srcService, string detService, string func, string param, int timeout)
{
	CDistributedService* service = FindService(srcService);
	if(!service)
		return "";

	if(service->IsLocalService())
		return service->Call(detService, func, param, timeout);
	else
		return m_EventOut->CallService(srcService, detService, func, param.c_str(), param.length(), timeout);
	return "";
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CLuaEventOut& GetServerLuaOut()
{
	return *CLuaCppManager::Instance().GetLuaEventOut();
}

//////////////////////////////////////////////////////////////////////////
CLuaClientEventOut& GetClientLuaOut()
{
	return *CLuaCppManager::Instance().GetLuaClientEventOut();
}

#ifdef __MSSQL_DB_MODULE__
//////////////////////////////////////////////////////////////////////////
Kernel::CMSSqlPool& GetMSSqlPool()
{
	return *(CMSSqlPool*)CServerKernel::Instance().GetPool("MSSQLPool");
}
#endif

#ifdef __MYSQL_DB_MODULE__
//////////////////////////////////////////////////////////////////////////
Kernel::CMysqlPool& GetMysqlPool()
{
	return *(CMysqlPool*)CServerKernel::Instance().GetPool("MysqlPool");
}
#endif

#ifdef __REDIS_DB_MODULE__
//////////////////////////////////////////////////////////////////////////
Kernel::CRedisPool& GetRedisPool()
{
	return *(CRedisPool*)CServerKernel::Instance().GetPool("RedisPool");
}
#endif
