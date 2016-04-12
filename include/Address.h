#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include "Types.h"
#include "OperationOption.h"

namespace Kernel
{
/// @Brief 套接字地址
class _KERNEL_EXPORTS CAddress
{
public:
	explicit CAddress()
	{
		memset(&m_Addr, 0, sizeof(m_Addr));
		m_Addr.sin_family = AF_INET;
		m_Type = SOCKET_TYPE_TCP;
	}

	explicit CAddress(SocketType type)
	{
		memset(&m_Addr, 0, sizeof(m_Addr));
		m_Addr.sin_family = AF_INET;
		m_Type = type;
	}

	/// @param port 服务器监听端口
	/// 
	explicit CAddress(unsigned short port, SocketType type = SOCKET_TYPE_TCP)
	{
		memset(&m_Addr, 0, sizeof(m_Addr));
		SocketOpt::ConvertSockaddrToNet(NULL, port, &m_Addr);
		m_Type = type;
	}

	explicit CAddress(const char* addr, unsigned short port, SocketType type = SOCKET_TYPE_TCP)
	{
		memset(&m_Addr, 0, sizeof(m_Addr));
		SocketOpt::ConvertSockaddrToNet(addr, port, &m_Addr);
		m_Type = type;
	}

	explicit CAddress(const struct sockaddr_in& addr, SocketType type = SOCKET_TYPE_TCP)
	{
		m_Addr = addr;
		m_Type = type;
	}

	explicit CAddress(unsigned long ip, SocketType type = SOCKET_TYPE_TCP)
	{
		m_Addr.sin_addr.s_addr = ip;
		m_Type = type;
	}

	/// @function 获取套接字地址信息
	/// 
	/// @return 返回结果
	struct sockaddr_in& GetSockAddr() { return m_Addr; }

	/// @function 获取套接字地址信息
	/// 
	/// @return 返回结果
	const struct sockaddr_in& GetSockAddr() const { return m_Addr; }

	/// @function 设置套接字地址信息
	/// 
	/// @param addr 信息
	CAddress& Set(const CAddress* addr)
	{
		assert(addr);
		m_Addr = addr->GetSockAddr();
		m_Type = addr->GetType();
		return *this;
	}

	/// @function 设置套接字地址信息
	/// 
	/// @param addr 信息
	void SetSockAddr(const struct sockaddr_in& addr) { m_Addr = addr; }

	/// @function 设置套接字地址信息
	/// 
	/// @param addr ip或域名
	/// @param port 端口
	/// @param type 类型
	void SetSockAddr(const char* addr, unsigned short port, SocketType type = SOCKET_TYPE_TCP)
	{
		memset(&m_Addr, 0, sizeof(m_Addr));
		SocketOpt::ConvertSockaddrToNet(addr, port, &m_Addr);
		m_Type = type;
	}

	/// @function 设置套接字地址信息
	/// 
	/// @param addr 信息
	void SetSockPort(unsigned short port, bool isNet = true)
	{
		memset(&m_Addr, 0, sizeof(m_Addr));
		m_Addr.sin_family = AF_INET;
		m_Addr.sin_port = (isNet ? SocketOpt::Htons(port) : port);
		m_Addr.sin_addr.s_addr = INADDR_ANY;
	}

	/// @function 设置套接字地址信息
	/// 
	/// @note 不做任何转换
	void SetAddr(long ip, unsigned short port, SocketType type = SOCKET_TYPE_TCP)
	{
		m_Addr.sin_family = AF_INET;
		m_Addr.sin_port = port;
		m_Addr.sin_addr.s_addr = (unsigned int)ip;
		m_Type = type;
	}

	/// @function 设置套接字地址类型
	/// 
	void SetType(SocketType type) { m_Type = type; }

	/// @function 转换套接字地址信息
	/// 
	/// @return 返回字符串
	string ConvertSockAddr() const
	{
		char buf[32] = { 0 };
		SocketOpt::ConvertSockaddrToLocal(buf, sizeof(buf), m_Addr);
		return buf;
	}

	/// @function 获取ip信息
	/// 
	/// @return 返回字符串
	string GetStrIP() const
	{
		char buf[32] = { 0 };
		SocketOpt::InetNtop(AF_INET, &m_Addr.sin_addr, buf, sizeof(buf));
		return buf;
	}

	/// @function 获取ip地址
	/// 
	/// @return 返回值
	unsigned long GetIP() const
	{
		return m_Addr.sin_addr.s_addr;
	}

	/// @function 获取端口
	/// 
	/// @return 返回端口
	unsigned short GetPort(bool isHost = true) const
	{
		return isHost ? SocketOpt::Ntohs(m_Addr.sin_port) : m_Addr.sin_port;
	}

	/// @function 获取地址字符串
	/// 
	/// @return 返回字符串
	string GetAddrStr() const
	{
		char buf[128] = { 0 };
		if(strcmp(GetStrIP().c_str(), "0.0.0.0") == 0)
			snprintf(buf, sizeof(buf), "%s:%d", "127.0.0.1", GetPort());
		else
			snprintf(buf, sizeof(buf), "%s:%d", GetStrIP().c_str(), GetPort());
		return buf;
	}

	/// @function 获取地址类型
	/// 
	SocketType GetType() const { return m_Type; }

	/// @function 重置信息
	/// 
	void Reset()
	{
		m_Type = SOCKET_TYPE_TCP;
		memset(&m_Addr, 0, sizeof(m_Addr));
	}

private:
	struct sockaddr_in		m_Addr;		///<地址信息
	SocketType				m_Type;		///<地址类型

}; // end by class CAddress

} // end by namespace Kernel

#endif //__INET_ADDRESS_H__