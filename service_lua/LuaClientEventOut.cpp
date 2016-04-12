#include "LuaClientEventOut.h"

static CLuaClientEventOut* g_LuaClientEventOut = NULL;

//////////////////////////////////////////////////////////////////////////
CLuaClientEventOut::CLuaClientEventOut(IClientOut* out)
	:m_EventOut(out)
{
	g_LuaClientEventOut = this;
}

//////////////////////////////////////////////////////////////////////////
CLuaClientEventOut::~CLuaClientEventOut()
{
	g_LuaClientEventOut = NULL;
}

//////////////////////////////////////////////////////////////////////////
CLuaClientEventOut& CLuaClientEventOut::Instance()
{
	assert(g_LuaClientEventOut);
	return *g_LuaClientEventOut;
}

//////////////////////////////////////////////////////////////////////////
double CLuaClientEventOut::Connect(const char* ip, unsigned short port, SocketType type, double sessionID /*= 0*/)
{
	uint64_t id = (uint64_t)sessionID;
	return (double)m_EventOut->Connect(ip, port, type, id);
}

//////////////////////////////////////////////////////////////////////////
double CLuaClientEventOut::Connect(const Kernel::CAddress* serverAddr, double sessionID /*= 0*/)
{
	uint64_t id = (uint64_t)sessionID;
	return (double)m_EventOut->Connect(serverAddr, id);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::SendData(double sessionID, const char* data, unsigned short size)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->SendData(id, (const void*)data, size);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::SendData(double sessionID, const char* data, unsigned short size, const Kernel::CAddress* addr /*= NULL*/)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->SendData(id, (const void*)data, size, addr);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::SendData(double sessionID, const char* data, unsigned short size, const Kernel::CAddress* addr /*= NULL*/, int level /*= Kernel::DATA_LEVEL_NORMAL*/)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->SendData(id, (const void*)data, size, addr, level);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::SendData(double sessionID, const char* data, unsigned short size, SocketType type)
{
	uint64_t id = (uint64_t)sessionID;
	return m_EventOut->SendData(id, (const void*)data, size, type);
}
//////////////////////////////////////////////////////////////////////////
void CLuaClientEventOut::Close(double sessionID)
{
	uint64_t id = (uint64_t)sessionID;
	m_EventOut->Close(id);
}

//////////////////////////////////////////////////////////////////////////
void CLuaClientEventOut::Shutdown(double sessionID)
{
	uint64_t id = (uint64_t)sessionID;
	m_EventOut->Shutdown(id);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime)
{
	return m_EventOut->SetTimer(timerID, interval, overTime);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime, const char* param /*= NULL*/, unsigned short size /*= 0*/)
{
	return m_EventOut->SetTimer(timerID, interval, overTime, (void*)param, size);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::KillTimer(unsigned int timerID)
{
	return m_EventOut->KillTimer(timerID);
}

//////////////////////////////////////////////////////////////////////////
void CLuaClientEventOut::SetUserToSession(double sessionID, double userId)
{
	uint64_t id1 = (uint64_t)sessionID;
	uint64_t id2 = (uint64_t)userId;
	m_EventOut->SetUserToSession(id1, id2);
}

//////////////////////////////////////////////////////////////////////////
double CLuaClientEventOut::GetSessionUserID(double sessionID)
{
	uint64_t id = (uint64_t)sessionID;
	return (double)m_EventOut->GetSessionUserID(id);
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::RequestP2P(double srcSessionID, double srcUserID, double desUserID, const char* helpHoldIP, unsigned short helpHoldPort, SocketType helpHoldType /*= SOCKET_TYPE_TCP*/)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CLuaClientEventOut::RequestP2P(double srcSessionID, double srcUserID, double desUserID, const Kernel::CAddress* helpHoldAddr)
{
	return false;
}
