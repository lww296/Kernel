#ifndef __I_DB_POOL_H__
#define __I_DB_POOL_H__

#include "Types.h"

class IDBHelper;

/// @Brief 数据库池接口
class _KERNEL_EXPORTS IDBPool
{
public:
	virtual ~IDBPool() { }

	/// @function 获取池名称
	/// 
	/// @return 返回名称
	virtual string& GetPoolName() = 0;

	/// @function 建立连接
	/// 
	/// @param addr	 连接字符串
	/// @param dbname : 数据库名字
	/// @return 成功返回编号,失败返回-1
	virtual int Connect(const char* addr, const char *dbname) = 0;

	/// @function 建立连接
	/// 
	/// @param hostname : 主机名
	/// @param port 端口
	/// @param dbname : 数据库名字
	/// @return 成功返回编号,失败返回-1
	virtual int Connect(const char* hostname, unsigned short port, const char *dbname) = 0;

	/// @function 连接数据库 
	/// @param hostname : 主机名
	/// @param user : 登录数据库用户
	/// @param passwd : 登录数据库密码
	/// @param dbname : 数据库名字
	/// @return 成功返回编号,失败返回-1
	virtual int Connect(const char *hostname, const char *user, const char *passwd, const char *dbname, unsigned short port = 0) = 0;

	/// @function 获取操作助手
	/// 
	/// @param no 池连接编号
	/// @return 返回结果
	virtual IDBHelper* GetHelper(int no) = 0;

	/// @function 获取操作空闲助手
	/// 
	/// @param dbName 数据库名称
	/// @return 返回结果
	virtual IDBHelper* GetHelper(const char* name) = 0;

	/// @function 释放助手
	/// 
	/// @param helper 助手对象
	virtual void FreeHelper(IDBHelper* helper) = 0;

	/// @function 关闭连接
	/// 
	/// @param no 池连接编号
	virtual bool Close(int no) = 0;

	/// @function 清空连接
	/// 
	/// @param 关闭池所有连接
	virtual void Clear() = 0;

	/// @function 获取助手总数
	/// 
	/// @param 返回助手总数
	virtual size_t GetHelperSize() = 0;

}; // end by IDBPool

#endif //__I_DB_POOL_H__