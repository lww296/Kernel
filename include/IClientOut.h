#ifndef __I_CLIENT_OUT_H__
#define __I_CLIENT_OUT_H__

#include "Types.h"
#include "Define.h"
#include "Address.h"

/// @Brief 客户端事件出接口
class _KERNEL_EXPORTS IClientOut
{
public:
	virtual ~IClientOut() { }

	//////////////////////////////////////////////////////////////////////////
	/// 网络事件
    
    /// @function 连接服务器
	/// 
	/// @param sessionID 编号(会话id)
	/// @param ip ip地址
	/// @param port 端口
	/// @param type 套接字协议类型
	/// @param sessionID 会话id, 如果是0，系统默认分配
	/// @return 返回会话id, 返回0表示失败
	virtual uint64_t Connect(const char* ip, unsigned short port, SocketType type = SOCKET_TYPE_TCP, uint64_t sessionID = 0) = 0;

	/// @function 连接服务器
	/// 
	/// @param sessionID 编号(会话id)
	/// @param serverAddr 服务器地址信息
	/// @param type 套接字协议类型
	/// @param sessionID 会话id, 如果是0，系统默认分配
	/// @return 返回会话id, 返回0表示失败
	virtual uint64_t Connect(const Kernel::CAddress* serverAddr, uint64_t sessionID = 0) = 0;

	/// @function 同步连接服务器
	/// 
	/// @param sessionID 编号(会话id)
	/// @param addr 服务器地址信息
	/// @param port 端口
	/// @param isUserCall 是否使用回调 如果成功(同步回调到 OnConnected),失败(同步回调到 OnConnectedFail)
	/// @param isAddrMgr 是否添加到客户端会话管理器里面管理
	/// @param 连接返回结果(isAddrMgr: true, 成功返回0)(isAddrMgr:false, 成功返回套接字id)
	/// @param sessionID 会话id, 如果是0，系统默认分配
	virtual int SyncConnect(const char* addr, int port, bool isUserCall = true, bool isAddrClientMgr = true, uint64_t sessionID = 0) = 0;

	/// @function 发送数据
	/// 
	/// @param sessionID 会话id
	/// @param data 数据
	/// @param size 数据大小
	/// @param addr 地址信息(如果为NULL，默认使用tcp)(如果你是udp协议，必须填写此参数)
	/// @param level 数据级别
	/// @return 返回结果
	virtual bool SendData(uint64_t sessionID, const void* data, unsigned short size, const Kernel::CAddress* addr = NULL, int level = Kernel::DATA_LEVEL_NORMAL) = 0;

	/// @function 发送数据
	/// @note 你不知道CAddress，你使用的又是udp协议。用此函数
	/// 
	/// @param sessionID 会话id
	/// @param data 数据
	/// @param size 数据大小
	/// @param type 发送类型(tcp or udp)必须与sessionId协议类型一致
	/// @param level 数据级别
	/// @return 返回结果
	virtual bool SendData(uint64_t sessionID, const void* data, unsigned short size, SocketType type, int level = Kernel::DATA_LEVEL_NORMAL) = 0;

	/// @function 关闭用户连接
	/// 
	/// @param sessionID 会话id
	/// @param type 套接字类型
	virtual void Close(uint64_t sessionID, SocketType type = SOCKET_TYPE_TCP) = 0;

	/// @function 中断用户连接
	/// 
	/// @param sessionID 会话id
	virtual void Shutdown(uint64_t sessionID) = 0;

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
	virtual bool SetTimer(unsigned int timerID, unsigned int interval, unsigned int overTime, void* param = NULL, unsigned short size = 0) = 0;

	/// @function 移除定时器
	/// 
	/// @parma timerID 定时器编号
	/// @return 返回结果
	virtual bool KillTimer(unsigned int timerID) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 其他事件

	/// @function 设置用户到会话id
	/// 
	virtual void SetUserToSession(uint64_t sessionID, uint64_t userId) = 0;

	/// @function 根据会话获取用户id
	/// 
	virtual uint64_t GetSessionUserID(uint64_t sessionID) = 0;

#ifdef _OPEN_P2P_MODULE_
	//////////////////////////////////////////////////////////////////////////
	/// p2p 事件

	/// @function p2p请求
	/// 
	/// @param srcSessionID 原会话id
	/// @param srcUserID 原用户id
	/// @param desUserID 请求p2p的目标用户id
	/// @param helpHoldIP 协助p2p服务器地址
	/// @param helpHoldPort 端口
	/// @param type 套接字类型
	/// @return 返回结果，成功返回true，失败返回false
	virtual bool RequestP2P(uint64_t srcSessionID, uint64_t srcUserID, uint64_t desUserID, const char* helpHoldIP, unsigned short helpHoldPort, SocketType helpHoldType = SOCKET_TYPE_UDT) = 0;
	virtual bool RequestP2P(uint64_t srcSessionID, uint64_t srcUserID, uint64_t desUserID, const Kernel::CAddress* helpHoldAddr) = 0;

#endif //_OPEN_P2P_MODULE_

};

#endif //__I_CLIENT_OUT_H__