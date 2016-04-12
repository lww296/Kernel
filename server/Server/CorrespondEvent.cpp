#include "CorrespondEvent.h"
#include "Buffer.h"
#include "ServerLog.h"
#include "AttemperEvent.h"

using namespace Kernel;

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
CCorrespondEvent::CCorrespondEvent()
	:m_ClientOut(NULL)
	,m_LogicModule(NULL)
{

}

//////////////////////////////////////////////////////////////////////////
CCorrespondEvent::~CCorrespondEvent()
{
	m_ClientOut = NULL;
	m_LogicModule = NULL;
}

//////////////////////////////////////////////////////////////////////////
bool CCorrespondEvent::OnSelfContorlBuf(uint64_t sessionID, void* inBuf, unsigned int inLen, void* out, unsigned int &outLen, unsigned int type /*= 0*/) /*= 0*/
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
unsigned int CCorrespondEvent::OnSelfEncrypt(const char* src, unsigned int srcLen, char* dst) /*= 0*/
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
unsigned int CCorrespondEvent::OnSelfDecrypt(const char* src, unsigned int srcLen, char* dst) /*= 0*/
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool CCorrespondEvent::OnConnected(uint64_t sessionID, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnConnectedC", (double)sessionID, addr);
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
bool CCorrespondEvent::OnConnectedFail(uint64_t sessionID, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnConnectedFailC", (double)sessionID, addr);
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
bool CCorrespondEvent::OnDisconnects(uint64_t sessionID, const Kernel::CAddress* addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnDisconnectsC", (double)sessionID, addr);
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
bool CCorrespondEvent::OnReadEvent(uint64_t sessionID, void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr /*= NULL*/) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnReadEventC", (double)sessionID, string((char*)data, size), size, (unsigned int)exter, addr);
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
bool CCorrespondEvent::OnTimerEvent(unsigned int timerID, unsigned int interval, unsigned int overTime, const void* param /*= NULL*/, unsigned short paramLen /*= 0*/) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		return luabind::call_function<bool>(m_LogicModule->GetLuaState(), "OnTimerEventC", timerID, interval, overTime, string((char*)param, paramLen), paramLen);
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
void CCorrespondEvent::OnNodeRegisterSuccess(uint64_t sessionID, unsigned int serverId, ServerType type, Kernel::CAddress& addr) /*= 0*/
{
#ifdef _OPEN_LUA_MODULE_
	try 
	{ 
		luabind::call_function<void>(m_LogicModule->GetLuaState(), "OnNodeRegisterSuccessC", (double)sessionID, serverId, type, addr);
	} 
	catch(luabind::error& e)
	{
		WRITE_LOG_ERROR("%s module %s", m_MainLogicName.c_str(), e.what());
		WRITE_LOG_ERROR(lua_tostring(m_LogicModule->GetLuaState(), -1));
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
bool CCorrespondEvent::Init()
{
#ifdef _OPEN_LUA_MODULE_
	m_MainLogicName = CAttemperEvent::Instance().GetMainLogic();
	m_LogicModule = CLuaCppManager::Instance().FindModule(m_MainLogicName);
	assert(m_LogicModule);
#endif
	return true;
}