#include "DistributedService.h"
#include "LuaCppModule.h"
#include "LuaCppManager.h"
#include "ServerLog.h"
#include "ThreadPackage.h"

using namespace Kernel;

//////////////////////////////////////////////////////////////////////////
CDistributedService::CDistributedService(string serviceName, const Kernel::CAddress* remoteAddr /*= NULL*/, bool isCreate /*= false*/)
	:m_IsRunning(false)
	,m_LoaclService(true)
	,m_IsCreateQueue(isCreate)
	,m_ServiceName(serviceName)
	,m_ServiceQueue(NULL)
	,m_LuaModule(NULL)
	,m_Status(SERVICE_STATUS_NULL)
{
	if(remoteAddr)
	{
		m_RemoteAddr = *remoteAddr;
		m_LoaclService = false;
	}
}

//////////////////////////////////////////////////////////////////////////
CDistributedService::~CDistributedService()
{
	Stop();
	_SAFE_DELETE_(m_ServiceQueue);
	m_LuaModule = NULL;
	m_Status = SERVICE_STATUS_NULL;
}

//////////////////////////////////////////////////////////////////////////
bool CDistributedService::Init()
{
	if(m_Status >= SERVICE_STATUS_INIT)
		return true;

	if(m_LoaclService && m_IsCreateQueue)
	{
		m_ServiceQueue = new DistributedQueue();
	}

	m_Status = SERVICE_STATUS_INIT;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CDistributedService::Start()
{
	if(m_Status >= SERVICE_STATUS_START)
		return true;

	m_Status = SERVICE_STATUS_START;
	m_IsRunning = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CDistributedService::Stop()
{
	if(m_Status == SERVICE_STATUS_STOP || m_Status == SERVICE_STATUS_NULL || !m_IsRunning)
		return true;

	m_IsRunning = false;
	m_Status = SERVICE_STATUS_STOP;

	m_CallWait.Clear(true);
	if(m_ServiceQueue)
		m_ServiceQueue->Clear();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CDistributedService::Send(string desServiceName, string func, string param)
{
	// 查找目标服务是否存在
	CDistributedService* service = CLuaCppManager::Instance().FindService(desServiceName);
	if(!service)
	{
		if(CLuaCppManager::Instance().GetEventOut())
			return CLuaCppManager::Instance().GetEventOut()->SendData(m_ServiceName, desServiceName, func, param.c_str(), param.length());
		else
			return false;
	}

	if(m_Status != SERVICE_STATUS_START)
		return false;

	// 构造数据
	DistributeServiceHead head = { 0 };
	head.m_Cmd = 0;
	head.m_ReqServiceNameLen = m_ServiceName.length();
	head.m_DstServiceNameLen = desServiceName.length();
	head.m_FuncNameLen = func.length();
	head.m_DataLen = param.length();
	head.m_Timespace = 0;

	// 申请对象
	int len = sizeof(head) + m_ServiceName.length() + desServiceName.length() + func.length() + param.length();
	Kernel::CTEventPackage* pak = CLuaCppManager::Instance().GetEventOut()->AllocPacket(len);
	if(!pak) return "";
	pak->Reset();
	pak->m_Buf.Push((const char*)&head, sizeof(head));
	pak->m_Buf.Push(m_ServiceName.c_str(), m_ServiceName.length());
	pak->m_Buf.Push(desServiceName.c_str(), desServiceName.length());
	pak->m_Buf.Push(func.c_str(), func.length());
	pak->m_Buf.Push(param.c_str(), param.length());
	pak->m_Type = LOGIC_TYPE_SEND_SERVICE;
	pak->m_ID = 0;		
	pak->m_ThreadEventType = 0;
	pak->m_Len = len;
	pak->m_Exter = 0;

	DistributedQueue* distributedQueue = service->GetQueue();
	if(distributedQueue) distributedQueue->Put(pak);
	return true;
}

//////////////////////////////////////////////////////////////////////////
string CDistributedService::Call(string desServiceName, string func, string param, int timeout /*= 3000*/)
{
	// 查找目标服务是否存在
	CDistributedService* dstService = CLuaCppManager::Instance().FindService(desServiceName);
	if(!dstService)
	{
		if(CLuaCppManager::Instance().GetEventOut())
			return CLuaCppManager::Instance().GetEventOut()->CallService(m_ServiceName, desServiceName, func, param.c_str(), param.length(), timeout);
		else
			return "";
	}

	if(m_Status != SERVICE_STATUS_START)
		return "";

	// 释放等待队列的数据
	for(int i = 0; i < (int)m_CallWait.Size(); ++i)
	{
		Kernel::CTEventPackage* task = NULL;
		m_CallWait.Take(task);
		if(task)
			CLuaCppManager::Instance().GetEventOut()->FreePacket(task);
	}

	// 构造数据
	DistributeServiceHead head = { 0 };
	head.m_Cmd = 0;
	head.m_ReqServiceNameLen = m_ServiceName.length();
	head.m_DstServiceNameLen = desServiceName.length();
	head.m_FuncNameLen = func.length();
	head.m_DataLen = param.length();
	head.m_Timespace = SystemInfo::GetTime() + (timeout == 0 ? (unsigned int)-1 : (timeout >= 1000 ? timeout - 5 : timeout));		// 调用超时时间

	// 申请对象
	int len = sizeof(head) + m_ServiceName.length() + desServiceName.length() + func.length() + param.length();
	Kernel::CTEventPackage* pak = CLuaCppManager::Instance().GetEventOut()->AllocPacket(len);
	if(!pak) return "";
	pak->Reset();
	pak->m_Buf.Push((const char*)&head, sizeof(head));
	pak->m_Buf.Push(m_ServiceName.c_str(), m_ServiceName.length());
	pak->m_Buf.Push(desServiceName.c_str(), desServiceName.length());
	pak->m_Buf.Push(func.c_str(), func.length());
	pak->m_Buf.Push(param.c_str(), param.length());
	pak->m_Type = LOGIC_TYPE_CALL_SERVICE;
	pak->m_ID = 0;		
	pak->m_ThreadEventType = 0;
	pak->m_Len = len;
	pak->m_Exter = (uint64_t)&m_CallWait;	// 回调队列地址
	m_CallWait.SetTimeout(timeout);

	// 向目标服务队列投递数据
	DistributedQueue* distributedQueue = dstService->GetQueue();
	if(distributedQueue) distributedQueue->Put(pak);

	// 等待
	Kernel::CTEventPackage* task = NULL;
	m_CallWait.Take(task);
	if(task == NULL)
		return "";

	// 获取返回数据
	string retData = string(task->Buffer(), task->m_Len);

	// 释放对象
	CLuaCppManager::Instance().GetEventOut()->FreePacket(task);
	
	return retData;
}

//////////////////////////////////////////////////////////////////////////
void CDistributedService::Run()
{
	if(!m_IsRunning || !m_ServiceQueue)
		return;

	CTEventPackage* logicPacket = NULL;
	m_ServiceQueue->Take(logicPacket);
	if(!logicPacket)
		return;
	
	try
	{
		// 运行逻辑事件
		tagTimerBody* timerBody = NULL;
		LogicPacketHead head = { (unsigned short)logicPacket->m_Type, logicPacket->m_Len, logicPacket->m_ID, logicPacket->m_Exter };
		int type = (int)logicPacket->m_Type;
		uint64_t id = logicPacket->m_ID;
		bool ret = true;	// 返回结果

		switch(type)
		{
		case LOGIC_TYPE_NET_READ :		///<网络读取事件
			{
				ret =  luabind::call_function<bool>(m_LuaModule->GetLuaState(), "OnReadEvent", (double)id, string(logicPacket->Buffer(), head.m_Len), head.m_Len, (double)head.m_Exter, &logicPacket->m_AddrInfo);
				break;
			}

		case LOGIC_TYPE_TIMER :			///<定时器事件
			{
				timerBody = (tagTimerBody*)(logicPacket->Buffer());
				if(timerBody->m_ParamLen > 0)
					ret = luabind::call_function<bool>(m_LuaModule->GetLuaState(), "OnTimerEvent", static_cast<unsigned int>(head.m_ID), (unsigned int)timerBody->m_Interval, (unsigned int)timerBody->m_OverTime,
						string(((TimerBodyBuffer*)logicPacket->Buffer())->m_Param, timerBody->m_ParamLen), timerBody->m_ParamLen);
				else
					ret = luabind::call_function<bool>(m_LuaModule->GetLuaState(), "OnTimerEvent", static_cast<unsigned int>(head.m_ID), (unsigned int)timerBody->m_Interval, (unsigned int)timerBody->m_OverTime, NULL, 0);
				break;
			}

		case LOGIC_TYPE_SELF_DEF :		///<自定义控制事件
			{
				// reload脚本事件
				if(head.m_ID == 0)
				{
					m_LuaModule->Reload();
					break;
				}

				ret = luabind::call_function<bool>(m_LuaModule->GetLuaState(), "OnSelfContorlEvent", (unsigned int)head.m_ID, string(logicPacket->Buffer(), head.m_Len), head.m_Len);
				break;
			}

		case LOGIC_TYPE_SEND_SERVICE :		/// 接受服务发送数据事件
			{
				// 解析分布式服务包头,获取服务名
				DistributeServiceHead* serviceHead = (DistributeServiceHead*)logicPacket->Buffer();
				char* serviceData = (logicPacket->Buffer() + sizeof(DistributeServiceHead));
				string reqServiceName(serviceData, serviceHead->m_ReqServiceNameLen);
				string dstServiceName(serviceData + serviceHead->m_ReqServiceNameLen, serviceHead->m_DstServiceNameLen);
				string func = string(serviceData + serviceHead->m_ReqServiceNameLen + serviceHead->m_DstServiceNameLen, serviceHead->m_FuncNameLen);
				string param = string(serviceData + serviceHead->m_ReqServiceNameLen + serviceHead->m_DstServiceNameLen + serviceHead->m_FuncNameLen, serviceHead->m_DataLen);
				assert(dstServiceName == m_ServiceName);
				luabind::call_function<bool>(m_LuaModule->GetLuaState(), "ServiceSend", reqServiceName, func, param);
				break;
			}

		case LOGIC_TYPE_CALL_SERVICE :		/// 接受服务调用事件
			{
				// 解析分布式服务包头,获取服务名
				DistributeServiceHead* serviceHead = (DistributeServiceHead*)logicPacket->Buffer();
				char* serviceData = (logicPacket->Buffer() + sizeof(DistributeServiceHead));
				string reqServiceName(serviceData, serviceHead->m_ReqServiceNameLen);
				string dstServiceName(serviceData + serviceHead->m_ReqServiceNameLen, serviceHead->m_DstServiceNameLen);
				string func = string(serviceData + serviceHead->m_ReqServiceNameLen + serviceHead->m_DstServiceNameLen, serviceHead->m_FuncNameLen);
				string param = string(serviceData + serviceHead->m_ReqServiceNameLen + serviceHead->m_DstServiceNameLen + serviceHead->m_FuncNameLen, serviceHead->m_DataLen);
				assert(dstServiceName == m_ServiceName);
				string retData = luabind::call_function<string>(m_LuaModule->GetLuaState(), "ServiceCall", reqServiceName, func, param);

				// 本地调用
				if(logicPacket->m_ID == 0 && logicPacket->m_Exter != 0)
				{
					CallServiceQueue* callQueue = (CallServiceQueue*)logicPacket->m_Exter;
					Kernel::CTEventPackage* pak = CLuaCppManager::Instance().GetEventOut()->AllocPacket(retData.length());
					if(!pak) break;
					pak->Reset();
					pak->m_Len = retData.length();
					pak->m_Buf.Push(retData.c_str(), retData.length());
					if(callQueue && SystemInfo::GetTime() <= serviceHead->m_Timespace)
						callQueue->Put(pak);
					else
						CLuaCppManager::Instance().GetEventOut()->FreePacket(pak);
				}
				else
				{
					// 远程调用
					CLuaCppManager::Instance().GetEventOut()->SendCallServiceData(reqServiceName, dstServiceName, retData.c_str(), retData.length(), serviceHead->m_Timespace);
				}
				break;
			}
		
		default:assert(false); break;
		}
	}
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_ServiceName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LuaModule->GetLuaState(), -1));
	}
	
	// 释放对象
	CLuaCppManager::Instance().GetEventOut()->FreePacket(logicPacket);
}
