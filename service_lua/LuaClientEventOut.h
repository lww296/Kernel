#ifndef _LUA_CLIENT_EVENT_OUT_H__
#define _LUA_CLIENT_EVENT_OUT_H__

#include "IClientOut.h"

/// @Brief lua客户端事件出接口
class CLuaClientEventOut
{
public:
	CLuaClientEventOut(IClientOut* out);
	~CLuaClientEventOut();

	static CLuaClientEventOut& Instance();

	//////////////////////////////////////////////////////////////////////////
	/// 网络事件

	/// @function 连接服务器
	/// 
	/// @param sessionID 编号(会话id)
	/// @param ip ip地址
	/// @param port 端口
	/// @param type 套接字类型
	/// @return 返回会话id
	double Connect(const char* ip, unsigned short port, SocketType type, double sessionID = 0);

	/// @function 连接服务器
	/// 
	/// @param sessionID 编号(会话id)
	/// @param serverAddr 服务器地址信息
	/// @param type 套接字协议类型
	/// @return 返回会话id
	double Connect(const Kernel::CAddress* serverAddr, double sessionID = 0);

	/// @function 发送数据
	/// 
	/// @param sessionID 会话id
	/// @param data 数据
	/// @param size 数据大小
	/// @param addr 地址信息(如果为NULL，默认使用tcp)(如果你是udp协议，必须填写此参数)
	/// @return 返回结果
	bool SendData(double sessionID, const char* buf, unsigned short size);
	bool SendData(double sessionID, const char* buf, unsigned short size, const Kernel::CAddress* addr);
	bool SendData(double sessionID, const char* data, unsigned short size, const Kernel::CAddress* addr = NULL, int level = Kernel::DATA_LEVEL_NORMAL);

	/// @function 发送数据
	/// 
	/// @param sessionID 会话id
	/// @param data 数据
	/// @para size 数据大小
	/// @return 返回结果
	bool SendData(double sessionID, const char* data, unsigned short size, SocketType type);

	/// @function 关闭用户连接
	/// 
	/// @param sessionID 会话id
	void Close(double sessionID);

	/// @function 中断用户连接
	/// 
	/// @param sessionID 会话id
	void Shutdown(double sessionID);

	//////////////////////////////////////////////////////////////////////////
	/// 定时器事件

	/// @function 设置定时器
	/// 
	/// @param timerID 定时器编号(必须大于10)
	/// @param interval 定时器运行持续次数
	/// @param overTimer 每轮到期时间(毫秒为单位)
	/// @param param 参数
	/// @param size 参数数据长度
	/// @return 返回结果，成功返回true，失败返回false
	bool SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime);
	bool SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime, const char* param = NULL, unsigned short size = 0);

	/// @function 移除定时器
	/// 
	/// @parma timerID 定时器编号
	/// @return 返回结果
	bool KillTimer(unsigned int timerID);

	//////////////////////////////////////////////////////////////////////////
	/// 其他事件

	/// @function 设置用户到会话id
	/// 
	void SetUserToSession(double sessionID, double userId);

	/// @function 根据会话获取用户id
	/// 
	double GetSessionUserID(double sessionID);

	/// @function p2p请求
	/// 
	/// @param srcSessionID 原会话id
	/// @param srcUserID 原用户id
	/// @param desUserID 请求p2p的目标用户id
	/// @param helpHoldIP 协助p2p服务器地址
	/// @param helpHoldPort 端口
	/// @param type 套接字类型
	/// @return 返回结果，成功返回true，失败返回false
	bool RequestP2P(double srcSessionID, double srcUserID, double desUserID, const char* helpHoldIP, unsigned short helpHoldPort, SocketType helpHoldType = SOCKET_TYPE_TCP);
	bool RequestP2P(double srcSessionID, double srcUserID, double desUserID, const Kernel::CAddress* helpHoldAddr);

	/// @function 设置事件出接口
	/// 
	/// @param out 接口对象
	void SetEventOut(IClientOut* out) { m_EventOut = out; }

private:
	IClientOut*			m_EventOut;		///<服务器事件出接口

};


#endif //_LUA_CLIENT_EVENT_OUT_H__