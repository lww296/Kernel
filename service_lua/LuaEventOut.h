#ifndef _LUA_EVENT_OUT_H__
#define _LUA_EVENT_OUT_H__

#include "IEventOut.h"
#include <list>

/// @brief lua调用事件出
class CLuaEventOut
{
public:
	CLuaEventOut(IEventOut* out);
	~CLuaEventOut();

	static CLuaEventOut& Instance();

	//////////////////////////////////////////////////////////////////////////
	/// 其它事件

	/// @function 创建绑定套接字
	/// 
	/// @param addr 连接地址信息
	/// @param bindAddr 绑定地址信息
	/// @param bindID 绑定标识(绑定多个端口时，bindID必须不一样, 0: 自动分配)
	/// @param sync 同步or异步
	/// @return 成功返回绑定标识，失败返回-1
	int CreateBindSocket(Kernel::CAddress addr, int bindID = 0, bool sync = true);
	int CreateBindSocket(const char* addr, unsigned short port, int bindID = 0, bool sync = true);

	/// @function 删除绑定套接字
	/// 
	/// @param bindID 绑定标识
	/// @param sync 同步or异步
	/// @return 成功返回true
	bool DestoryBindSocket(unsigned int bindID = 0, bool sync = true);

	/// @function 设置用户到会话id
	/// 
	void SetUserToSession(double sessionID, double userId);

	/// @function 根据会话获取用户id
	/// 
	double GetSessionUserID(double sessionID);

	//////////////////////////////////////////////////////////////////////////
	/// 网络事件

	/// @function 发送数据
	/// 
	/// @param sessionID 会话ID
	/// @param buf 数据
	/// @param size 长度
	/// @return 返回发送结果
	bool Send(double sessionID, const char* buf, unsigned int size);
	bool Send(double sessionID, const char* buf, unsigned int size, const Kernel::CAddress* addr);
	bool Send(double sessionID, const char* buf, unsigned int size, const Kernel::CAddress* addr = NULL, int level = Kernel::DATA_LEVEL_NORMAL);

	/// @function 广播数据
	/// 
	/// @param buf 数据
	/// @param size 长度
	/// @param sessionIdList 发送数据给指定的一批用户(std::list<uint64_t>)
	void Broadcast(const char* buf, unsigned int size);
	void Broadcast(const char* buf, unsigned int size, const std::list<double>& sessionIdList);

	/// @function 关闭用户连接
	/// 
	/// @param sessionID 会话ID
	void Close(double sessionID);

	/// @function 中断用户连接
	/// 
	/// @param sessionID 会话ID
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
	/// 数据库事件

	/// @function 投递数据库操作请求
	/// 
	/// @param sessionID 会话ID(上下文)
	/// @param requestID 数据库请求ID
	/// @param data 数据
	/// @param size 大小
	/// @param exter 扩展值
	/// @param 返回投递结果
	bool PostDatabaseRequest(double sessionID, unsigned int requestID, const char* data, unsigned short size, double exter = 0);

	/// @function 投递数据库事件结果
	/// 
	/// @param sessionID 会话ID(上下文)
	/// @param dbID 数据库请求ID
	/// @param data 数据
	/// @param size 大小
	/// @param local 是否本地处理
	/// @param exter 扩展值
	/// @param 返回投递结果
	bool PostDatabaseEvent(double sessionID, unsigned int dbID, const char* data, unsigned short size, bool IsLocal = true, double exter = 0);

	//////////////////////////////////////////////////////////////////////////
	/// 自定义事件

	/// @function 自定义控制事件
	/// 
	/// @param eventId 事件id
	/// @param data 数据
	/// @param size 数据大小
	/// @return 返回结果
	bool PostSelfEvent(unsigned int eventID, const char* data, unsigned short size);

	/// @function 设置事件出接口
	/// 
	/// @param out 接口对象
	void SetEventOut(IEventOut* out) { m_EventOut = out; }

private:
	IEventOut*			m_EventOut;		///<服务器事件出接口

};

#endif //_LUA_EVENT_OUT_H__