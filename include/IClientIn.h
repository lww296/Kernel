#ifndef __I_CLIENT_IN_H__
#define __I_CLIENT_IN_H__

#include "Types.h"
#include "Address.h"

/// @Brief 客户端事件进接口
class _KERNEL_EXPORTS IClientIn
{
public:
	virtual ~IClientIn() { }

	//////////////////////////////////////////////////////////////////////////
	/// 自定义事件

	/// @function 自己控制缓冲区
	/// @note 独立的线程事件
	/// 
	/// @param sessionID 会话id
	/// @param inBuf 缓冲区数据
	/// @param inLen 输入缓冲区数据长度
	/// @param out 输出对象(解包对象)(发送数据)
	/// @param outLen 输出对象长度
	/// @param type 控制缓冲区类型(0:接收缓存, 1:发送缓冲区)
	/// @return 返回结果,处理了完整的包返回true
	virtual bool OnSelfContorlBuf(uint64_t sessionID, void* inBuf, unsigned int inLen, void* out, unsigned int &outLen, unsigned int type = 0) = 0;

	/// @function 自定义加密
	/// @note 独立的线程事件 send thread
	/// 
	/// @param src 原数据
	/// @param srcLen 原数据长度
	/// @param dst 加密后数据
	/// @return 加密后的数据长度
	virtual unsigned int OnSelfEncrypt(const char* src, unsigned int srcLen, char* dst) = 0;

	/// @function 自定义解密
	/// @note 独立的线程事件 recv thread
	/// 
	/// @param src 原数据
	/// @param srcLen 原数据长度
	/// @param dst 解密后数据
	/// @return 解密后的数据长度
	virtual unsigned int OnSelfDecrypt(const char* src, unsigned int srcLen, char* dst) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 网络事件

	/// @function 连接服务器成功
	/// 
	///	@param sessionID 会话id
	/// @param addr 连接地址信息
	/// @return 返回结果
	virtual bool OnConnected(uint64_t sessionID, const Kernel::CAddress* addr) = 0;

	/// @function 连接服务器失败
	/// 
	///	@param sessionID 会话id
	/// @param addr 连接地址信息
	/// @return 返回结果
	virtual bool OnConnectedFail(uint64_t sessionID, const Kernel::CAddress* addr) = 0;

	/// @function 与服务器断开连接
	/// 
	/// @param sessionID 会话id
	/// @param addr 连接地址信息
	/// @return 返回结果
	virtual bool OnDisconnects(uint64_t sessionID, const Kernel::CAddress* addr) = 0;

	/// @function 用户网络消息事件读取
	/// 
	/// @param sessionID 会话id
	/// @param data 数据
	/// @param size 数据长度
	/// @param exter 扩展值
	/// @param addr 地址信息
	/// @return 返回结果, 返回false将会关闭套接字
	virtual bool OnReadEvent(uint64_t sessionID, void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 定时器事件

	/// @function 定时器事件
	///
	/// @param timerID 定时器ID
	/// @param interval 时间间隔
	/// @param overTime 到期时间(毫秒为单位)
	/// @param param 定时器参数
	/// @param paramLen 参数长度
	/// @return 返回结果, 返回false将会移除定时器
	virtual bool OnTimerEvent(unsigned int timerID, unsigned int interval, unsigned int overTime, const void* param = NULL, unsigned short paramLen = 0) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 节点事件

	/// @function 节点注册成功
	/// 
	/// @param sessionID 会话Id
	/// @param serverId 服务器id
	/// @param type 服务器类型
	/// @param addr 节点地址信息
	virtual void OnNodeRegisterSuccess(uint64_t sessionID, unsigned int serverId, ServerType type, Kernel::CAddress& addr) = 0;

#ifdef _OPEN_P2P_MODULE_
	//////////////////////////////////////////////////////////////////////////
	/// p2p 操作回调

	/// @function p2p事件
	/// 
	/// @param srcSessionID 原会话id
	/// @param srcUserID 原用户id
	/// @param desUserID 请求p2p的目标用户id
	/// @param retCode 返回代码
	virtual void OnP2PEvent(uint64_t srcSessionID, uint64_t srcUserID, uint64_t desUserID, int retCode) = 0;

#endif //_OPEN_P2P_MODULE_

};

#endif //__I_CLIENT_IN_H__