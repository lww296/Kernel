#include "Types.h"
#ifdef _OPEN_DB_MODULE_
#ifdef __REDIS_DB_MODULE__
#ifndef __REDIS_HELPER_H__
#define __REDIS_HELPER_H__

#ifdef __WIN_PLATFORM
#pragma comment(lib,"hiredis.lib")
#endif

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "IDBHelper.h"

#include "hiredis/hiredis.h"

#ifdef __REDIS_ASYNC_EVENT__
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#include "hiredis/async.h"
#endif //__REDIS_ASYNC_EVENT__

namespace Kernel
{
/// @Brief Redis数据库助手(暂不支持异步操作，请使用同步接口调用)
class _KERNEL_EXPORTS CRedisHelper : boost::noncopyable, public IDBHelper
{
public:
	CRedisHelper();
	virtual ~CRedisHelper();

	/// @function 初始化
	/// 
	/// @return 连接成功返回true, 失败返回false
	virtual bool Init(string poolName, string dbName, string addrInfo = "") /*= 0*/;

	/// @function 获取数据库名称(redis: ip:port)
	/// 
	/// @return 返回dbname
	virtual string GetDbName() /*= 0*/ { return m_DbName; }

	/// @function 获取地址信息
	/// 
	/// @return 返回助手地址信息
	virtual string GetHelperAddrInfo() /*= 0*/ { return m_AddrInfo; }

	/// @function 设置状态
	/// 
	/// @param status 状态
	virtual void SetStatus(DbHelperStatus status) /*= 0*/ { m_Status = status; }

	/// @function 获取状态
	/// 
	/// @return 返回状态
	virtual DbHelperStatus GetStatus() /*= 0*/ { return m_Status; }

	/// @function 获取助手编号
	/// 
	/// @return 返回助手编号
	virtual int GetHelperNO() /*= 0*/{ return m_HelperNO; };

	/// @function 设置助手编号
	/// 
	/// @param no 助手编号
	virtual void SetHelperNO(int no) /*= 0*/ { m_HelperNO = no; }

	/// @function 建立连接
	/// @note (Don't use for redis helper) 
	/// 
	/// @param info	 连接字符串
	/// @return 成功返回true
	virtual bool Connect(const char* info) /*= 0*/;

	/// @function 建立连接
	/// 
	/// @param hostname : 主机名
	/// @param port 端口
	/// @return 成功返回true
	virtual bool Connect(const char* hostname, unsigned short port) /*= 0*/;

	/// @function 连接数据库
	/// @note (Don't use for redis helper) 
	/// 
	/// @param hostname : 主机名
	/// @param user : 登录数据库用户
	/// @param passwd : 登录数据库密码
	/// @param dbname : 数据库名字
	/// @return 成功返回true
	virtual bool Connect(const char *hostname, const char *user, const char *passwd, const char *dbname) /*= 0*/;

	/// @function 关闭连接
	/// 
	virtual void Close() /*= 0*/;

	/// @function 执行命令
	/// 
	/// @param cmd sql语句 or 存储过程名称
	/// @param isProc 执行的是否是存储过程
	/// @param isStore 是否存储记录集
	/// @return 返回执行结果，成功返回true
	virtual bool Execute(const char* cmd, bool isProc, bool isStore) /*= 0*/;

	/// @function 获取记录集行数量
	/// 
	/// @retrn 返回行数量
	virtual long GetRows() /*= 0*/;

	/// @function 获取记录集列数量
	/// 
	/// @return 返回列数量
	virtual unsigned int GetConlums() /*= 0*/;

	/// @function 移动到下一个记录行
	/// 
	virtual void MoveNext() /*= 0*/;

	/// @function 是否是记录集尾部
	///
	/// @return 是返回true
	virtual bool IsRecordsEnd() /*= 0*/;

	/// @function 重置记录集
	/// 
	virtual void ResetRecordset() /*= 0*/;

	/// @function 返回影响的行数
	/// 
	/// @return 返回结果
	virtual unsigned long long GetAffectedRows() /*= 0*/;

	/// @function 数据偏移
	/// 
	/// @param offset 偏移
	virtual void DataSeek(unsigned long long offset) /*= 0*/;

	/// @function 列偏移
	/// 
	/// @param offset 偏移
	/// @return 返回实际偏移了多少列
	virtual unsigned int FieldSeek(unsigned int offset) /*= 0*/;

	/// @function 执行命令
	/// 
	/// @param fmtcmd 格式化参数
	/// @return 成功返回true
	bool Execute(const char* fmtcmd, ...);

	/// @function 获取字段数据
	/// 
	/// @param index 字段列索引位置
	/// @return 返回结果
	bool GetBool();
	int GetInt();
	long long GetLonglong();
	const char* GetString(char* buf, size_t bufSize);

	/// @function 获取当前结果集类型
	/// 
	/// @return 返回类型
	int GetReplyType() { if(m_RedisReply) return m_RedisReply->type; else return 0; }

	/// @function 获取当前数据长度
	/// 
	/// @return 返回长度
	int GetReplayLength() { if(m_RedisReply) return m_RedisReply->len; else return 0; }

	/// @function 获取当前结果集元素数量
	/// 
	/// @return 返回元素数量
	size_t GetReplayElements() { if(m_RedisReply) return m_RedisReply->elements; else return 0; }

	/// @function 获取当前结果集
	/// 
	/// @return 返回当前结果集
	struct redisReply* GetRedisReply() { return m_RedisReply; }

	//////////////////////////////////////////////////////////////////////////
	// 异步操作接口
public:
	/// @function 异步连接redis服务器
	/// 
	/// @param ip ip地址
	/// @param port 端口
	bool AsyncConnect(const char* ip, unsigned int port);

	/// @function 执行命令
	/// 
	/// @param isStore 是否存储记录集
	/// @param privdata 提供数据
	/// @param fmtcmd 格式化参数
	/// @return 成功返回true
	bool AsyncExecute(bool isStore, void *privdata, const char* fmtcmd, ...);

private:
	struct redisContext*	m_RedisContext;			///<redis上下文
	struct redisReply*		m_RedisReply;			///<结果集

#ifdef __REDIS_ASYNC_EVENT__
	redisAsyncContext*	m_AsyncContext;			///<异步redis上下文
#endif //__REDIS_ASYNC_EVENT__
	
	int					m_HelperNO;			///<助手编号
	string				m_PoolName;			///<池名称
	string				m_DbName;			///<db名称("0")...
	string				m_AddrInfo;			///<地址信息(ip:port)
	DbHelperStatus		m_Status;			///<助手状态
	bool				m_IsAsync;			///<是否异步操作

	friend class CRedisPool;

}; // end by CRedisHelper

} // end by Kernel

#endif //__REDIS_HELPER_H__
#endif //__REDIS_DB_MODULE__
#endif //_OPEN_DB_MODULE_