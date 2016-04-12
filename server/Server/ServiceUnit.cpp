#include <iostream>
#include <stdlib.h>
#include <boost/bind.hpp>

#include "ServiceUnit.h"
#include "Address.h"
#include "IEventIn.h"
#include "IEventOut.h"
#include "ServerLog.h"
#include "MSSqlPool.h"
#include "Define.h"

#ifdef _OPEN_LUA_MODULE_
#include "LuaCppModule.h"
#endif

using namespace Kernel;

//////////////////////////////////////////////////////////////////////////
static CServiceUnit* g_ServiceUnit = NULL;

//////////////////////////////////////////////////////////////////////////
CServiceUnit::CServiceUnit(const char* path)
	:m_AttemperEvent(new CAttemperEvent())
	,m_CorrespondEvent(new CCorrespondEvent())
	,m_Config(path)
{
	g_ServiceUnit = this;
	m_ServerKernel = new CServerKernel(m_Config, (IEventIn*)m_AttemperEvent, m_CorrespondEvent);
	m_State = SERVICE_UNIT_START;
}

//////////////////////////////////////////////////////////////////////////
CServiceUnit::~CServiceUnit()
{
	 Stop();
	 _SAFE_DELETE_(m_CorrespondEvent);
	 _SAFE_DELETE_(m_AttemperEvent);
	 _SAFE_DELETE_(m_ServerKernel);
}

//////////////////////////////////////////////////////////////////////////
CServiceUnit& CServiceUnit::Instance()
{
	assert(g_ServiceUnit);
	return *g_ServiceUnit;
}

//////////////////////////////////////////////////////////////////////////
bool CServiceUnit::Run()
{
	// 获取引擎操作接口
	IEventOut* out = m_ServerKernel->GetEventOut();
	IClientOut* clientOut = m_ServerKernel->GetClientEventOut();
	if(!out)
		return false;

	// 设置接口到调度引擎
	m_AttemperEvent->SetEventOut(out);
	m_AttemperEvent->SetClientOut(clientOut);
	m_CorrespondEvent->SetEventOut(clientOut);

	// 设置服务器关闭回调
	m_ServerKernel->SetStopCall(boost::bind(&CServiceUnit::Stop, this));

	// 服务器初始化
	if(!m_ServerKernel->Init())
		return false;

	// 初始化
	if(!m_AttemperEvent->Init())
		return false;

	// 初始化
	if(!m_CorrespondEvent->Init())
		return false;

	// 服务器启动
	if(!m_ServerKernel->Start())
		return false;	
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CServiceUnit::Stop()
{
	if(m_State == SERVICE_UNIT_STOP)
		return true;

	WRITE_LOG_NORMAL("server stop ....");

	// 停止服务器引擎
	m_ServerKernel->Stop();
	m_State = SERVICE_UNIT_STOP;

	return true;
}