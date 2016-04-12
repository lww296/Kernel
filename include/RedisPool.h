#include "Types.h"
#ifdef _OPEN_DB_MODULE_
#ifdef __REDIS_DB_MODULE__
#ifndef __REDIS_POOL_H__
#define __REDIS_POOL_H__

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>

#include "Types.h"
#include "IDBPool.h"
#include "IDBHelper.h"
#include "MutexLock.h"
#include "RedisHelper.h"

namespace Kernel
{
// 异步redis操作回调函数
typedef boost::function<void (CRedisHelper* helper, int status)> AsyncConnectRedisFunc;
typedef boost::function<void (CRedisHelper* helper, void* privdata)> AsyncCommandRedisFunc;

/// @Brief redis 连接池
class _KERNEL_EXPORTS CRedisPool : boost::noncopyable, public IDBPool
{
public:
	CRedisPool(string& name);
	~CRedisPool();

	static CRedisPool& Instance();

	/// @function 获取池名称
	/// 
	/// @return 返回名称
	virtual string& GetPoolName() /*= 0*/;

	/// @function 建立连接
	/// 
	/// @param info	 连接字符串
	/// @param dbname : 数据库名字
	/// @return 成功返回编号,失败返回-1
	virtual int Connect(const char* info, const char *dbname) /*= 0*/;

	/// @function 建立连接
	/// 
	/// @param hostname : 主机名
	/// @param port 端口
	/// @param dbname : 数据库名字
	/// @return 成功返回编号,失败返回-1
	virtual int Connect(const char* hostname, unsigned short port, const char *dbname) /*= 0*/;

	/// @function 连接数据库 
	/// @param hostname : 主机名
	/// @param user : 登录数据库用户
	/// @param passwd : 登录数据库密码
	/// @param dbname : 数据库名字
	/// @return 成功返回编号,失败返回-1
	virtual int Connect(const char *hostname, const char *user, const char *passwd, const char *dbname, unsigned short port = 0) /*= 0*/;

	/// @function 获取操作助手
	/// 
	/// @param no 池连接编号
	/// @return 返回结果
	virtual IDBHelper* GetHelper(int no) /*= 0*/;

	/// @function 获取操作空闲助手
	/// 
	/// @param dbName 数据库名称
	/// @return 返回结果
	virtual IDBHelper* GetHelper(const char* name) /*= 0*/;

	/// @function 释放助手
	/// 
	/// @param helper 助手对象
	virtual void FreeHelper(IDBHelper* helper) /*= 0*/;

	/// @function 关闭连接
	/// 
	/// @param no 池连接编号
	virtual bool Close(int no) /*= 0*/;

	/// @function 清空连接
	/// 
	/// @param 关闭池所有连接
	virtual void Clear() /*= 0*/;

	/// @function 获取助手总数
	/// 
	/// @param 返回助手总数
	virtual size_t GetHelperSize() /*= 0*/ { return m_PoolHelper.size(); }

	/// @function 获取操作空闲助手
	/// 
	/// @param dbName 数据库名称
	/// @return 返回结果
	CRedisHelper& GetRedisHelper(const char* name);

	/// @function 获取操作助手
	/// 
	/// @param no 池连接编号
	/// @return 返回结果
	CRedisHelper& GetRedisHelper(int no) /*= 0*/;

	/// @function 释放助手
	/// 
	/// @param helper 助手对象
	void FreeRedisHelper(CRedisHelper& helper);

	//////////////////////////////////////////////////////////////////////////
	// 异步操作接口
public:
#ifdef __REDIS_ASYNC_EVENT__
	/// @function 线程运行事件
	/// 
	void Run();

	/// @function 根据异步reid上下文查找helper
	/// 
	/// @return 返回助手
	CRedisHelper* GetHelper(redisAsyncContext* context);

	/// @function 设置,获取连接回调
	/// 
	/// @param helper 助手
	/// @param func 操作回调函数
	void SetAsyncConnectCall(AsyncConnectRedisFunc func) { m_AsyncConnnectFunc = func; }
	AsyncConnectRedisFunc GetAsyncConnectCall() { return m_AsyncConnnectFunc; }

	/// @function 设置,获取连接断开回调
	/// 
	/// @param helper 助手
	/// @param func 操作回调函数
	void SetAsyncDisconectCall(AsyncConnectRedisFunc func) { m_AsyncDisconnnectFunc = func; }
	AsyncConnectRedisFunc GetAsyncDisconectCall() { return m_AsyncDisconnnectFunc; }

	/// @function 设置,获取命令操作回调
	/// 
	/// @param func 操作回调函数
	void SetAsyncCommandCall(AsyncCommandRedisFunc func) { m_AsyncCommandFunc = func; }
	AsyncCommandRedisFunc GetAsyncCommandCall() { return m_AsyncCommandFunc; }

	/// @function 异步连接redis服务器
	/// 
	/// @param ip ip地址
	/// @param port 端口
	/// @return 成功返回编号,失败返回-1
	int AsyncConnect(const char* hostname, unsigned short port, const char *dbname);

	/// @function redis助手连接(已经存在helper连接调用)
	/// 
	/// @return 成功返回true, 失败返回false
	bool AsyncConnect(IDBHelper* helper);
#endif //__REDIS_ASYNC_EVENT__

private:
	typedef boost::unordered_map<int, IDBHelper*>	RedisPoolHelper;
	typedef RedisPoolHelper::const_iterator	Cit;
	typedef RedisPoolHelper::iterator			It;

	RedisPoolHelper		m_PoolHelper;		///<redis数据库连接池助手
	string				m_PoolName;			///<池名称
	unsigned int		m_HelperTotal;		///<助手个数
	CMutexLock			m_PoolMutex;		///<池互斥体

	AsyncConnectRedisFunc	m_AsyncConnnectFunc;	///<异步连接回调
	AsyncConnectRedisFunc	m_AsyncDisconnnectFunc;	///<异步断开连接回调
	AsyncCommandRedisFunc	m_AsyncCommandFunc;		///<异步命令执行回调

}; // end by CRedisPool

} // end by Kernel

#endif //__REDIS_POOL_H__
#endif //__REDIS_DB_MODULE__
#endif //_OPEN_DB_MODULE_