#include "Types.h"
#ifdef _OPEN_DB_MODULE_
#ifdef __MSSQL_DB_MODULE__
#ifndef __MSSQL_POOL_H__
#define __MSSQL_POOL_H__

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

#include "Types.h"
#include "IDBPool.h"
#include "IDBHelper.h"
#include "MutexLock.h"

namespace Kernel
{
class CMSSqlHelper;

/// @Brief mssql 连接池
class _KERNEL_EXPORTS CMSSqlPool : boost::noncopyable, public IDBPool
{
public:
	CMSSqlPool(string& name);
	~CMSSqlPool();

	static CMSSqlPool& Instance();

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
	virtual int Connect(const char *hostname, const char *user, const char *passwd, const char *dbname, unsigned short port = 1433) /*= 0*/;

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
	CMSSqlHelper& GetMSHelper(const char* name);

	/// @function 获取操作助手
	/// 
	/// @param no 池连接编号
	/// @return 返回结果
	CMSSqlHelper& GetMSHelper(int no) /*= 0*/;

	/// @function 释放助手
	/// 
	/// @param helper 助手对象
	void FreeMSHelper(CMSSqlHelper& helper);

private:
	typedef boost::unordered_map<int, IDBHelper*>	MSSqlPoolHelper;
	typedef MSSqlPoolHelper::const_iterator	Cit;
	typedef MSSqlPoolHelper::iterator			It;

	MSSqlPoolHelper		m_PoolHelper;		///<mongodb数据库连接池助手
	string				m_PoolName;			///<池名称
	unsigned int		m_HelperTotal;		///<助手个数
	CMutexLock			m_PoolMutex;		///<池互斥体

}; // end by CMSSqlPool

} // end by Kernel

#endif //__MSSQL_POOL_H__
#endif //__MSSQL_DB_MODULE__
#endif //_OPEN_DB_MODULE_