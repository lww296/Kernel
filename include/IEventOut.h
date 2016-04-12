#ifndef __I_EVENT_OUT_H__
#define __I_EVENT_OUT_H__

#include "Types.h"
#include "Address.h"
#include "Define.h"
#include "ThreadPackage.h"

class _KERNEL_EXPORTS IEventOut
{
public:
	virtual ~IEventOut() { }

	//////////////////////////////////////////////////////////////////////////
	/// 其它事件

	/// @function 创建绑定套接字
	/// 
	/// @param addr 连接地址信息
	/// @param bindAddr 绑定地址信息
	/// @param bindID 绑定标识(绑定多个端口时，bindID必须不一样, 0: 自动分配)
	/// @param sync 同步or异步
	/// @return 成功返回绑定标识，失败返回-1
	virtual int CreateBindSocket(Kernel::CAddress addr, int bindID = 0, bool sync = true) = 0;
	virtual int CreateBindSocket(const char* addr, unsigned short port, int bindID = 0, bool sync = true) = 0;

	/// @function 删除绑定套接字
	/// 
	/// @param bindID 绑定标识
	/// @param sync 同步or异步
	/// @return 成功返回true
	virtual bool DestoryBindSocket(unsigned int bindID = 0, bool sync = true) = 0;

	/// @function 设置用户到会话id
	/// 
	virtual void SetUserToSession(uint64_t sessionID, uint64_t userId) = 0;

	/// @function 根据会话获取用户id
	/// 
	virtual uint64_t GetSessionUserID(uint64_t sessionID) = 0;

	/// @function 分配一块内存包
	/// 
	/// @param allocSize 分配大小
	virtual Kernel::CTEventPackage* AllocPacket(unsigned int allocSize) = 0;

	/// @function 释放内存包
	/// 
	virtual void FreePacket(Kernel::CTEventPackage* obj) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 网络事件

	/// @function 发送数据
	/// 
	/// @param sessionID 会话ID
	/// @param buf 数据
	/// @param size 长度
	/// @param addr 地址信息(udp协议。必须填写此字段)
	/// @param level 数据级别
	/// @return 返回发送结果
	virtual bool Send(uint64_t sessionID, const void* buf, size_t size, const Kernel::CAddress* addr = NULL, int level = Kernel::DATA_LEVEL_NORMAL) = 0;

	/// @function 广播数据
	/// 
	/// @param buf 数据
	/// @param size 长度
	/// @param sessionIdList 指定数据发送给那些用户列表(std::list<uint64_t>)
	virtual void Broadcast(const void* buf, size_t size, void* sessionIdList = NULL) = 0;

	/// @function 关闭用户连接
	/// 
	/// @param sessionID 会话ID
	virtual void Close(uint64_t sessionID) = 0;

	/// @function 中断用户连接
	/// 
	/// @param sessionID 会话ID
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
	/// 数据库事件

	/// @function 投递数据库操作请求
	/// 
	/// @param sessionID 会话ID(上下文)
	/// @param requestID 数据库请求ID
	/// @param data 数据
	/// @param size 大小
	/// @param exter 扩展值
	/// @param 返回投递结果
	virtual bool PostDatabaseRequest(uint64_t sessionID, unsigned int requestID, const void* data, unsigned short size, uint64_t exter = 0) = 0;

	/// @function 投递数据库事件结果
	/// 
	/// @param sessionID 会话ID(上下文)
	/// @param dbID 数据库请求ID
	/// @param data 数据
	/// @param size 大小
	/// @param local 是否本地处理
	/// @param exter 扩展值
	/// @param 返回投递结果
	virtual bool PostDatabaseEvent(uint64_t sessionID, unsigned int dbID, const void* data, unsigned short size, bool IsLocal = true, uint64_t exter = 0) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 自定义事件

	/// @function 自定义控制事件
	/// 
	/// @param eventId 事件id
	/// @param data 数据
	/// @param size 数据大小
	/// @return 返回结果
	virtual bool PostSelfEvent(unsigned int eventID, const void* data, unsigned short size) = 0;

	/// @function 投递任务到线程中运行
	///
	/// @param callback 任务回调函数
	/// @param taskName 任务名称
	/// @param param 回调函数参数
	/// @param runTime 运行次数(0: 永久运行)
	/// @return 投递成功返回true
	virtual bool PostUserTask(Kernel::UserTaskCallParam callback, string taskName = "", const void* param = NULL, unsigned int runTime = 0) = 0;

	/// @function 投递任务到线程中运行
	/// @note 线程单独运行此任务(独立运行)
	///
	/// @param callback 任务回调函数
	/// @param taskName 任务名称
	/// @return 投递成功返回true
	virtual bool PostUserTask(string taskName, Kernel::UserTaskCallback callback) = 0;

	/// @function 投递任务到线程中运行
	///
	/// @param callback 任务回调函数
	/// @param taskName 任务名称
	/// @param runTime 运行次数(0: 永久运行)
	/// @return 投递成功返回true
	virtual bool PostUserTask(Kernel::UserTaskCallback callback, string taskName, unsigned int runTime) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 分布式服务

	/// @function 设置主逻辑服务名
	/// 
	/// @param serviceName 服务信息
	virtual void SetMainLogicName(string serviceName) = 0;

	/// @function 创建服务
	/// 
	/// @param serviceName 服务信息
	/// @param remoteAddr 远程服务地址
	/// @param serviceQueue 服务队列
	/// @param sessionID 远程服务会话id(本地服务默认0)
	virtual bool CreateService(string serviceName, Kernel::CAddress	remoteAddr, void* serviceQueue, uint64_t sessioinID = 0) = 0;

	/// @function 删除服务
	/// 
	/// @param serviceName 服务名
	virtual bool DeleteService(string serviceName) = 0;

	/// @function 发送数据
	/// 
	/// @param srcService 请求服务名
	/// @param dstService 目标服务名
	/// @param func 函数
	/// @param data 数据
	/// @param size 数据大小
	/// @param 返回结果
	virtual bool SendData(string srcService, string dstService, string func, const void* data, unsigned short size) = 0;

	/// @function 同步调用数据
	/// 
	/// @param srcService 请求服务名
	/// @param dstService 目标服务名
	/// @param func 函数
	/// @param data 数据
	/// @param size 数据大小
	/// @param timeout 调用超时时间(0:永不超时)
	/// @param 返回数据
	virtual string CallService(string srcService, string dstService, string func, const void* data, unsigned short size, unsigned int timeout = 3000) = 0;

	/// @function 发送调用返回数据
	/// 
	/// @param srcService 请求服务名
	/// @param dstService 目标服务名
	/// @param data 数据
	/// @param size 数据大小
	/// @param timespace 时间戳
	/// @param 返回结果
	virtual bool SendCallServiceData(string srcService, string dstService, const void* data, unsigned short size, uint64_t timespace) = 0;

	/// @function 设置验证用户会话
	/// @note 没验证的会话事件只能被投递到逻辑主线程
	/// 
	/// @param sessionID 验证会话id
	/// @param verify 是否验证
	virtual void VerifySession(uint64_t sessionID, bool verify) = 0;

};

#endif //__I_EVENT_OUT_H__