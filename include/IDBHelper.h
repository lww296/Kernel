#ifndef __I_DB_HELPER_H__
#define __I_DB_HELPER_H__

#include "Types.h"

enum DbHelperStatus
{
	HELPER_STATUS_NULL,			// 无效状态

	HELPER_STATUS_INIT,			// 初始化
	HELPER_STATUS_CONNECTING,	// 正在连接
	HELPER_STATUS_CONNECTED,	// 已连接
	HELPER_STATUS_FREE,			// 空闲(可使用)
	HELPER_STATUS_IDLE,			// 忙
	HELPER_STAUTS_STOP,			// 停止
	
	HELPER_STATUS_MAX,		
};

/// @Brief 数据库助手接口类
class _KERNEL_EXPORTS IDBHelper
{
public:
	virtual ~IDBHelper() { }

	/// @function 初始化
	/// 
	/// @param poolName 池名称
	/// @param dbName 数据库名称
	/// @param addrInfo 地址信息
	/// @return 连接成功返回true, 失败返回false
	virtual bool Init(string poolName, string dbName, string addrInfo = "") = 0;

	/// @function 获取数据库名称
	/// 
	/// @return 返回dbname
	virtual string GetDbName() = 0;

	/// @function 获取地址信息
	/// 
	/// @return 返回助手地址信息
	virtual string GetHelperAddrInfo() = 0;

	/// @function 设置状态
	/// 
	/// @param status 状态
	virtual void SetStatus(DbHelperStatus status) = 0;

	/// @function 获取助手编号
	/// 
	/// @return 返回助手编号
	virtual int GetHelperNO() = 0;

	/// @function 设置助手编号
	/// 
	/// @param no 助手编号
	virtual void SetHelperNO(int no) = 0;

	/// @function 获取状态
	/// 
	/// @return 返回状态
	virtual DbHelperStatus GetStatus() = 0;

	/// @function 建立连接
	/// 
	/// @param info	 连接字符串
	/// @return 成功返回true
	virtual bool Connect(const char* info) = 0;

	/// @function 建立连接
	/// 
	/// @param hostname : 主机名
	/// @param port 端口
	/// @return 成功返回true
	virtual bool Connect(const char* hostname, unsigned short port) = 0;

	/// @function 连接数据库 
	/// @param hostname : 主机名
	/// @param user : 登录数据库用户
	/// @param passwd : 登录数据库密码
	/// @param dbname : 数据库名字
	/// @return 成功返回true
	virtual bool Connect(const char *hostname, const char *user, const char *passwd, const char *dbname) = 0;

	/// @function 关闭连接
	/// 
	virtual void Close() = 0;

	/// @function 执行sql
	/// 
	/// @param cmd sql语句 or 存储过程名称
	/// @param isProc 执行的是否是存储过程
	/// @param isStore 是否存储记录集
	/// @return 返回执行结果，成功返回true
	virtual bool Execute(const char* cmd, bool isProc, bool isStore) = 0;

	/// @function 获取记录集行数量
	/// 
	/// @retrn 返回行数量
	virtual long GetRows() = 0;

	/// @function 获取记录集列数量
	/// 
	/// @return 返回列数量
	virtual unsigned int GetConlums() = 0;

	/// @function 移动到下一个记录行
	/// 
	virtual void MoveNext() = 0;
	
	/// @function 是否是记录集尾部
	///
	/// @return 是返回true
	virtual bool IsRecordsEnd() = 0;

	/// @function 重置记录集
	/// 
	virtual void ResetRecordset() = 0;

	/// @function 返回影响的行数
	/// 
	/// @return 返回结果
	virtual unsigned long long GetAffectedRows() = 0;

	/// @function 数据偏移
	/// 
	/// @param offset 偏移
	virtual void DataSeek(unsigned long long offset) = 0;

	/// @function 列偏移
	/// 
	/// @param offset 偏移
	/// @return 返回实际偏移了多少列
	virtual unsigned int FieldSeek(unsigned int offset) = 0;


}; // end by IDBHelper

#endif //__I_DB_HELPER_H__
