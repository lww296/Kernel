#ifndef __ATTEMPER_EVENT_H__
#define __ATTEMPER_EVENT_H__

#include "IEventIn.h"
#include "IEventOut.h"
#include "IClientOut.h"

#ifdef _OPEN_LUA_MODULE_
#include "LuaCppModule.h"
#include "LuaCppManager.h"
#include "LuaCppDefine.h"
#include <boost/interprocess/ipc/message_queue.hpp>
#endif

using namespace boost::interprocess;

/// @Brief 事件进接口, 非特别声明事件来源，默认是logic thread
class CAttemperEvent : public IEventIn
{
public:
	CAttemperEvent();
	~CAttemperEvent();

	static CAttemperEvent& Instance();

	//////////////////////////////////////////////////////////////////////////
	/// 初始化事件

	/// @function 逻辑线程开始运行
	/// 
	virtual void LogicThreadInit() /*= 0*/;

	//////////////////////////////////////////////////////////////////////////
	/// 状态事件

	/// @function 服务器引擎已经启动成功
	/// 
	/// @return 成功返回true, 返回false服务器将停止
	virtual bool OnStarted() /*= 0*/;

	/// @function 服务器引擎准备停止
	/// 
	virtual void OnReadyStop() /*= 0*/;

	/// @function 服务器节点注册成功
	/// 
	/// @param sessionID 会话Id
	/// @param serverId 服务器id
	/// @param type 服务器类型
	/// @param addr 节点地址信息
	virtual void OnNodeRegisterSuccess(uint64_t sessionID, unsigned int serverId, ServerType type, Kernel::CAddress& addr) /*= 0*/;

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
	virtual bool OnSelfContorlBuf(uint64_t sessionID, void* inBuf, unsigned int inLen, void* out, unsigned int &outLen, unsigned int type = 0) /*= 0*/;

	/// @function 自定义控制事件
	/// 
	/// @param eventId 事件id
	/// @param data 数据
	/// @param size 数据大小
	/// @return 返回结果
	virtual bool OnSelfContorlEvent(unsigned int eventID, void* data, unsigned short size) /*= 0*/;

	/// @function 自定义加密
	/// @note 独立的线程事件 send thread
	/// 
	/// @param src 原数据
	/// @param srcLen 原数据长度
	/// @param dst 加密后数据
	/// @return 加密后的数据长度
	virtual unsigned int OnSelfEncrypt(const char* src, unsigned int srcLen, char* dst) /*= 0*/;

	/// @function 自定义解密
	/// @note 独立的线程事件 recv thread
	/// 
	/// @param src 原数据
	/// @param srcLen 原数据长度
	/// @param dst 解密后数据
	/// @return 解密后的数据长度
	virtual unsigned int OnSelfDecrypt(const char* src, unsigned int srcLen, char* dst) /*= 0*/;

	//////////////////////////////////////////////////////////////////////////
	/// 网络事件

	/// @function 用户连接成功
	/// 
	/// @param sessionID 会话ID
	/// @param bindID 由哪个绑定端口来的连接
	/// @param addr 连接地址信息
	/// @return 返回结果, 返回false将会关闭套接字
	virtual bool OnConnect(uint64_t sessionID, unsigned int bindID, const Kernel::CAddress* addr) /*= 0*/;

	/// @function 用户网络消息事件读取
	/// 
	/// @param sessionID 会话ID
	/// @param data 数据
	/// @param size 数据长度
	/// @param exter 扩展值
	/// @param addr 连接地址信息
	/// @return 返回结果, 返回false将会关闭套接字
	virtual bool OnReadEvent(uint64_t sessionID, void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) /*= 0*/;

	/// @function 网络关闭
	/// 
	/// @param sessionID 会话ID
	/// @param addr 连接地址信息
	virtual void OnCloseEvent(uint64_t sessionID, const Kernel::CAddress* addr)/* = 0*/;

	//////////////////////////////////////////////////////////////////////////
	/// 定时器事件

	/// @function 定时器事件
	///
	/// @param timerID 定时器ID
	/// @param interval 时间间隔(毫秒为单位)
	/// @param overTime 到期时间
	/// @param param 定时器参数数据
	/// @param paramLen 定时器参数长度
	/// @return 返回结果, 返回false将会移除定时器
	virtual bool OnTimerEvent(unsigned int timerID, unsigned int interval, unsigned int overTime, const void* param = NULL, unsigned short paramLen = 0) /*= 0*/;

	//////////////////////////////////////////////////////////////////////////
	/// 数据库事件

	/// @function 数据库操作请求事件
	/// @note 独立的线程事件 db thread
	/// 
	/// @param sessionID 会话ID(上下文)
	/// @param requestID 数据库请求ID
	/// @param data 数据
	/// @param size 大小
	/// @return 返回结果 成功返回true,失败返回false
	virtual bool OnDataBaseRequest(uint64_t sessionID, unsigned int requestID, void* data, unsigned short size) /*= 0*/;

	/// @function 数据事件
	/// 
	/// @param sessionID 会话ID(上下文)
	/// @param dbID 数据库请求ID
	/// @param data 数据
	/// @param size 大小
	/// @return 返回结果 成功返回true,失败返回false
	virtual bool OnDatabaseEvent(uint64_t sessionID, unsigned int dbID, void* data, unsigned short size) /*= 0*/;

	//////////////////////////////////////////////////////////////////////////
	/// 作为web服务器时候的事件

	/// @function web服务器事件
	/// @note 独立的线程事件 read thread
	/// 
	/// @param httpReq web请求
	/// @param httpRes web回复
	/// @return 返回结果
	virtual bool OnHttpServerEvent(Kernel::CHttpRequest* httpReq, Kernel::CHttpResponse* httRes) /*= 0*/;

	//////////////////////////////////////////////////////////////////////////
	/// 分布式服务

	/// @function 本地创建服务结果
	/// 
	/// @param serviceName 服务信息
	/// @param result 创建服务返回结果
	virtual void LocalCreateService(string serviceName, int result) /*= 0*/;

	/// @function 本地删除服务结果
	/// 
	/// @param result 删除服务返回结果
	virtual void LocalDeleteService(string serviceName, int result) /*= 0*/;

	/// @function 查询服务
	/// @note 选择服务队列
	///
	/// @param sessionID 会话id
	/// @param data 数据
	/// @param size 长度
	/// @return 返回投递服务队列(默认是逻辑主线程)
	virtual string QueryServiceQueue(uint64_t sessionID, const void* data, unsigned short size) /*= 0*/;

	/// @function 远程请求创建服务
	/// 
	/// @param serviceName 服务信息
	/// @param result 创建服务返回结果
	virtual void RemoteCreateService(string serviceName, uint64_t sessioinID, const Kernel::CAddress* addr) /*= 0*/;

	/// @function 远程请求删除服务
	/// 
	/// @param result 删除服务返回结果
	virtual void RemoteDeleteService(string serviceName, bool result) /*= 0*/;

	/// @function 服务发送数据事件
	/// 
	/// @param sessionID 会话id
	/// @param func 执行函数
	/// @param data 数据
	/// @param len 长度
	virtual void ServiceSendData(uint64_t sessionID, string func, const void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) /*= 0*/;

	/// @function 服务调用数据事件
	/// 
	/// @param sessionID 会话id
	/// @param func 执行函数
	/// @param data 数据
	/// @param len 长度
	/// @return 返回结果数据
	virtual string ServiceCallData(uint64_t sessionID, string func, const void* data, unsigned short size, uint64_t exter, const Kernel::CAddress* addr) /*= 0*/;

public:
	/// @function 设置事件出接口
	/// 
	/// @param out 接口对象
	void SetEventOut(IEventOut* out) { m_EventOut = out; }

	/// @function 获取事件出接口
	/// 
	/// @return 接口对象
	IEventOut* GetEventOut() { return m_EventOut; }

	/// @function 设置事件出接口
	/// 
	/// @param out 接口对象
	void SetClientOut(IClientOut* out) { m_ClientOut = out; }

	/// @function 获取事件出接口
	/// 
	/// @return 接口对象
	IClientOut* GetClientOut() { return m_ClientOut; }

	/// @function 初始化
	/// 
	/// @return 返回结果
	bool Init();

	/// @function 发送数据
	/// 
	/// @param sessionID 会话ID
	/// @param cmd 命令
	/// @param buf 数据
	/// @param size 长度
	/// @return 返回发送结果
	bool Send(uint64_t sessionID, unsigned short cmd, const char* buf, unsigned int size);

#ifdef _OPEN_LUA_MODULE_

	/// @function 检测reload脚本
	/// 
	void ReloadScriptTask();

	/// @function 获取主逻辑服务名
	/// 
	/// @return 返回服务名
	string GetMainLogic() const { return m_MainLogicName; }

#endif 
	
private:
	IEventOut*					m_EventOut;			///<事件出
	IClientOut*					m_ClientOut;

public:
#ifdef _OPEN_LUA_MODULE_
	CLuaCppManager*				m_LuaCppManager;	/// lua cpp 模块管理
	CLuaCppModule*				m_LogicModule;		/// lua 逻辑操作模块
	CLuaCppModule*				m_DBModule;			/// lua db操作模块
	//message_queue				m_ReloadScriptMQ;	/// 重新加载脚本消息队列
	string						m_MainLogicName;	/// 主逻辑名称
#endif

};


#endif //__ATTEMPER_EVENT_H__