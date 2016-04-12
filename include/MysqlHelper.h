#include "Types.h"
#ifdef _OPEN_DB_MODULE_
#ifdef __MYSQL_DB_MODULE__
#ifndef __MYSQL_HELPER_H__
#define __MYSQL_HELPER_H__

#ifdef __WIN_PLATFORM
#pragma comment(lib,"libmysql.lib")
#endif

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

#include "Types.h"
#include "IDBHelper.h"
#include "ByteArray.h"
#include <mysql/mysql.h>

typedef struct tagMysqlExtraParameter
{
	char			m_IsNull;		// 是否空
	unsigned long	m_Length;		// 实际长度
	char			m_IsError;		// 是否有错误
}MysqlExtraParameter;

namespace Kernel
{
/// @Brief mysql数据库助手
class _KERNEL_EXPORTS CMysqlHelper : boost::noncopyable, public IDBHelper
{
public:
	CMysqlHelper();
	virtual ~CMysqlHelper();

	/// @function 初始化
	/// 
	/// @return 连接成功返回true, 失败返回false
	virtual bool Init(string poolName, string dbName, string addrInfo = "") /*= 0*/;

	/// @function 获取数据库名称
	/// 
	/// @return 返回dbname
	virtual string GetDbName() /*= 0*/ { return m_DbName; }

	/// @function 获取地址信息
	/// 
	/// @return 返回助手地址信息
	virtual string GetHelperAddrInfo() /*= 0*/ { return ""; }

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
	/// @param hostname : 主机名
	/// @param user : 登录数据库用户
	/// @param passwd : 登录数据库密码
	/// @param dbname : 数据库名字
	/// @return 成功返回true
	virtual bool Connect(const char *hostname, const char *user, const char *passwd, const char *dbname) /*= 0*/;

	/// @function 关闭连接
	/// 
	virtual void Close() /*= 0*/;

	/// @function 执行sql
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

	/// @function 行偏移
	/// 
	/// @param offset 偏移
	//virtual void RowSeek(unsigned long long offset) /*= 0*/;

	/// @function 列偏移
	/// 
	/// @param offset 偏移
	/// @return 返回实际偏移了多少列
	virtual unsigned int FieldSeek(unsigned int offset) /*= 0*/;

	/// @function 获取字段数据
	/// 
	/// @param index 字段列索引位置
	/// @return 返回结果
	const char* GetValus(unsigned int index);
	bool GetBool(unsigned int index);
	unsigned char GetByte(unsigned int index);
	short GetShort(unsigned int index);
	unsigned short GetUshort(unsigned int index);
	int GetInt(unsigned int index);
	unsigned int GetUInt(unsigned int index);
	long GetLong(unsigned int index);
	unsigned long GetULong(unsigned int index);
	long long GetLonglong(unsigned int index);
	long long GetLonglong(unsigned int index, CByteArray& bytes);
	unsigned long long GetUlonglong(unsigned int index);
	float GetFloat(unsigned int index);
	double GetDouble(unsigned int index);
	const char* GetString(unsigned int index, char* buf, size_t bufSize);
	const char* GetString(unsigned int index, CByteArray& bytes);

	/// @function 获取字段数据
	/// 
	/// @param fieldName 字段名称
	/// @return 返回结果
	bool GetBool(const char* fieldName);
	unsigned char GetByte(const char* fieldName);
	short GetShort(const char* fieldName);
	unsigned short GetUshort(const char* fieldName);
	int GetInt(const char* fieldName);
	unsigned int GetUInt(const char* fieldName);
	long GetLong(const char* fieldName);
	unsigned long GetULong(const char* fieldName);
	long long GetLonglong(const char* fieldName);
	long long GetLonglong(const char* fieldName, CByteArray& bytes);
	unsigned long long GetUlonglong(const char* fieldName);
	float GetFloat(const char* fieldName);
	double GetDouble(const char* fieldName);
	const char* GetString(const char* fieldName, char* buf, size_t bufSize);
	const char* GetString(const char* fieldName, CByteArray& bytes);

public:
	/// @function 插入参数
	/// 
	/// @param value 值
	/// @param isBindParam 绑定参数填true, 结果参数填false
	void AddParameter(int& value, bool isBindParam = true);
	void AddParameter(int64_t &value, bool isBindParam = true);
	void AddLLParameter(CByteArray& bytes, bool isBindParam = true);
	void AddParameter(char &value, bool isBindParam = true);
	void AddParameter(short &value, bool isBindParam = true);
	void AddParameter(float &value, bool isBindParam = true);
	void AddParameter(double &value, bool isBindParam = true);
	void AddParameter(string& value, bool isBindParam = true);
	void AddParameter(char* value, unsigned long valueSize, bool isBindParam = true);
	void AddParameter(CByteArray& bytes, bool isBindParam = true);
	void AddBlobParameter(unsigned char* value, unsigned long valueSize, bool isBindParam = true);
	void AddBlobParameter(CByteArray& bytes, bool isBindParam = true);
	void AddDateParameter(char* value, unsigned long valueSize, bool isBindParam = true);
	void AddDateParameter(CByteArray& bytes, bool isBindParam = true);

	/// @function 重置所有参数
	/// 
	void ResetParameter();
	
	/// @function 获取下一个结果集
	/// 
	/// @return 成功返回true
	bool MoveNextRecordset();

	/// @function 预处理语句
	/// 
	/// @param cmd 预处理语句
	/// @param cmdLen 语句长度
	/// @return 成功返回true
	bool StmtPrepare(const char* cmd, int len);

	/// @function 执行stmt语句
	/// 
	/// @param isStore 是否存储记录集
	/// @return 成功返回true
	bool ExecuteStmt(bool isStore = true);

private:
	MYSQL*					m_Mysql;				///<mysql 对象
	MYSQL_RES*				m_Recordset;			///<查询结果集
	MYSQL_ROW				m_Row;					///<行记录
	MYSQL_FIELD*			m_Field;				///<列记录
	MYSQL_STMT*				m_Stmt;					///<stmt对象
	MYSQL_BIND				m_BindParam[32];		///<绑定参数
	MysqlExtraParameter		m_BindExtraParam[32];	///<绑定扩展参数	
	MYSQL_BIND				m_ResultParam[64];		///<结果参数
	MysqlExtraParameter		m_ResultExtraParam[64];	///<结果扩展参数	
	int						m_BindParamOffset;		///<绑定参数偏移
	int						m_ResultParamOffset;	///<结果参数偏移
	bool					m_IsStmt;				///<是否以预处理的方式执行

	typedef boost::unordered_map<string, unsigned int> FieldNameMap;
	typedef FieldNameMap::const_iterator Cit;
	typedef FieldNameMap::iterator It;
	FieldNameMap			m_FieldNameMap;		///<字段名字与索引的映射表	

	int					m_HelperNO;			///<助手编号
	string				m_PoolName;			///<池名称
	string				m_DbName;			///<db名称
	DbHelperStatus		m_Status;			///<助手状态

}; // end by CMysqlHelper

} // end by Kernel

#endif //__MYSQL_HELPER_H__
#endif //__MYSQL_DB_MODULE__
#endif //_OPEN_DB_MODULE_