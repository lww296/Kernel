#include "LuaEventOut.h"

static CLuaEventOut* g_LuaEventOut = NULL;

//////////////////////////////////////////////////////////////////////////
CLuaEventOut::CLuaEventOut(IEventOut* out)
	:m_EventOut(out)
{
	g_LuaEventOut = this;
}

//////////////////////////////////////////////////////////////////////////
CLuaEventOut::~CLuaEventOut()
{
	g_LuaEventOut = NULL;
}

//////////////////////////////////////////////////////////////////////////
CLuaEventOut& CLuaEventOut::Instance()
{
	assert(g_LuaEventOut);
	return *g_LuaEventOut;
}

//////////////////////////////////////////////////////////////////////////
int CLuaEventOut::CreateBindSocket(Kernel::CAddress addr, int bindID /*= 0*/, bool sync /*= true*/)
{
	return m_EventOut->CreateBindSocket(addr, bindID, sync);
}

//////////////////////////////////////////////////////////////////////////
int CLuaEventOut::CreateBindSocket(const char* addr, unsigned short port, int bindID /*= 0*/, bool sync /*= true*/)
{
	return m_EventOut->CreateBindSocket(addr, port, bindID, sync);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::DestoryBindSocket(unsigned int bindID /*= 0*/, bool sync /*= true*/)
{
	return m_EventOut->DestoryBindSocket(bindID, sync);
}

//////////////////////////////////////////////////////////////////////////
void CLuaEventOut::SetUserToSession(double sessionID, double userId)
{
	uint64_t id1 = (uint64_t)sessionID;
	uint64_t id2 = (uint64_t)userId;
	m_EventOut->SetUserToSession(id1, id2);
}

//////////////////////////////////////////////////////////////////////////
double CLuaEventOut::GetSessionUserID(double sessionID)
{
	uint64_t id = (uint64_t)sessionID;
	return (double)m_EventOut->GetSessionUserID(id);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::Send(double sessionID, const char* buf, unsigned int size)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->Send(id, (void*)buf, size, NULL, Kernel::DATA_LEVEL_NORMAL);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::Send(double sessionID, const char* buf, unsigned int size, const Kernel::CAddress* addr)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->Send(id, (void*)buf, size, addr, Kernel::DATA_LEVEL_NORMAL);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::Send(double sessionID, const char* buf, unsigned int size, const Kernel::CAddress* addr /*= NULL*/, int level /*= Kernel::DATA_LEVEL_NORMAL*/)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->Send(id, (void*)buf, size, addr, level);
}

//////////////////////////////////////////////////////////////////////////
void CLuaEventOut::Broadcast(const char* buf, unsigned int size)
{
	return m_EventOut->Broadcast((void*)buf, size);
}

//////////////////////////////////////////////////////////////////////////
void CLuaEventOut::Broadcast(const char* buf, unsigned int size, const std::list<double>& sessionIdList)
{
	std::list<uint64_t> idlist;
	for(std::list<double>::const_iterator it = sessionIdList.begin(); it != sessionIdList.end(); ++it)
	{
		uint64_t val = (uint64_t)*it;
		idlist.push_back(val);
	}
	return m_EventOut->Broadcast((void*)buf, size, &idlist);
}

//////////////////////////////////////////////////////////////////////////
void CLuaEventOut::Close(double sessionID)
{
	uint64_t id = (uint64_t)sessionID;
	m_EventOut->Close(id);
}

//////////////////////////////////////////////////////////////////////////
void CLuaEventOut::Shutdown(double sessionID)
{
	uint64_t id = (uint64_t)sessionID;
	m_EventOut->Shutdown(id);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime)
{
	return m_EventOut->SetTimer(timerID, interval, overTime, NULL, 0);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime, const char* param /*= NULL*/, unsigned short size /*= 0*/)
{
	return m_EventOut->SetTimer(timerID, interval, overTime, (void*)param, size);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::KillTimer(unsigned int timerID)
{
	return m_EventOut->KillTimer(timerID);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::PostDatabaseRequest(double sessionID, unsigned int requestID, const char* data, unsigned short size, double exter /*= 0*/)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->PostDatabaseRequest(id, requestID, (void*)data, size, (uint64_t)exter);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::PostDatabaseEvent(double sessionID, unsigned int dbID, const char* data, unsigned short size, bool IsLocal /*= true*/, double exter /*= 0*/)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->PostDatabaseEvent(id, dbID, (void*)data, size, IsLocal, (uint64_t)exter);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaEventOut::PostSelfEvent(unsigned int eventID, const char* data, unsigned short size)
{
	return m_EventOut->PostSelfEvent(eventID, (void*)data, size);
}