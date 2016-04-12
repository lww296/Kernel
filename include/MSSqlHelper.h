#include "Types.h"
#ifdef _OPEN_DB_MODULE_
#ifdef __MSSQL_DB_MODULE__
#ifndef __MSSQL_HELPER_H__
#define __MSSQL_HELPER_H__

#import "c:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

#include "Types.h"
#include "IDBHelper.h"
#include "ByteArray.h"

namespace Kernel
{
/// @Brief sql server数据库助手
class _KERNEL_EXPORTS CMSSqlHelper : boost::noncopyable, public IDBHelper
{
public:
	CMSSqlHelper();
	virtual ~CMSSqlHelper();

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
	bool GetBool(unsigned int index);
	const char* GetValus(unsigned int index);
	unsigned char GetByte(unsigned int index);
	short GetShort(unsigned int index);
	unsigned short GetUshort(unsigned int index);
	int GetInt(unsigned int index);
	unsigned int GetUInt(unsigned int index);
	long GetLong(unsigned int index);
	unsigned long GetULong(unsigned int index);
	long long GetLonglong(unsigned int index);
	unsigned long long GetUlonglong(unsigned int index);
	long long GetLonglong(unsigned int index, CByteArray& bytes);
	float GetFloat(unsigned int index);
	double GetDouble(unsigned int index);
	const char* GetString(unsigned int index, char* buf, size_t bufSize);
	const char* GetString(unsigned int index, CByteArray& bytes);
	unsigned char* GetBlob(unsigned int index, unsigned char* buf, size_t bufSize);
	unsigned char* GetBlob(unsigned int index, CByteArray& bytes);
	const char* GetTime(unsigned int index, char* buf, size_t bufSize);
	const char* GetTime(unsigned int index, CByteArray& bytes);

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
	unsigned long long GetUlonglong(const char* fieldName);
	long long GetLonglong(const char* fieldName, CByteArray& bytes);
	float GetFloat(const char* fieldName);
	double GetDouble(const char* fieldName);
	const char* GetString(const char* fieldName, char* buf, size_t bufSize);
	const char* GetString(const char* fieldName, CByteArray& bytes);
	unsigned char* GetBlob(const char* fieldName, unsigned char* buf, size_t bufSize);
	unsigned char* GetBlob(const char* fieldName, CByteArray& bytes);
	const char* GetTime(const char* fieldName, char* buf, size_t bufSize);
	const char* GetTime(const char* fieldName, CByteArray& bytes);

public:
	/// @function 插入参数
	/// 
	/// @param name 表字段名
	/// @param value 值
	void AddParameter(const char* name, bool value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, char value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, unsigned char value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, int value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, unsigned int value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, long value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, unsigned long value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, __int64 value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, unsigned __int64 value, ParameterDirectionEnum direction = adParamInput);
	void AddLLParameter(const char* name, CByteArray& bytes, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, short value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, unsigned short value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, float value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, double value, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, const char* value, unsigned int valueLen, ParameterDirectionEnum direction = adParamInput);
	void AddParameter(const char* name, string value, ParameterDirectionEnum direction = adParamInput);
	void AddBlobParameter(const char* name, const unsigned char* value, unsigned int valueLen, ParameterDirectionEnum direction = adParamInput);
	void AddDateParameter(const char* name, const char* value, unsigned long valueLen, ParameterDirectionEnum direction = adParamInput);
	void AddOutParameter(const char* name, char* value, unsigned int size, ParameterDirectionEnum direction = adParamInputOutput);
	void AddOutParameter(const char* name, CByteArray& bytes, ParameterDirectionEnum direction = adParamInputOutput);

	/// @function 获取参数数据
	/// 
	/// @param name 参数名称
	/// @param value 值
	void GetParameter(const char* name, bool& value);
	void GetParameter(const char* name, char& value);
	void GetParameter(const char* name, unsigned char& value);
	void GetParameter(const char* name, short& value);
	void GetParameter(const char* name, unsigned short& value);
	void GetParameter(const char* name, int& value);
	void GetParameter(const char* name, unsigned int& value);
	void GetParameter(const char* name, long& value);
	void GetParameter(const char* name, unsigned long& value);
	void GetParameter(const char* name, float& value);
	void GetParameter(const char* name, double& value);
	void GetParameter(const char* name, __int64& value);
	void GetLLParameter(const char* name, CByteArray& bytes);
	void GetParameter(const char* name, char* value, unsigned int valueSize);
	void GetParameter(const char* name, CByteArray& bytes);
	
	/// @function 添加返回值参数
	/// 
	/// @param name 返回字段名称
	void AddReturnParameter(const char* name = "Return");

	/// @function 重置所有参数
	/// 
	void ResetParameter();

	/// @function 获取返回值
	///
	/// @return 返回结果
	long GetReturnValue();

	/// @function 是否连接错误
	/// 
	/// @return 返回结果
	bool IsConnectError();

	/// @function 重新连接
	/// 
	/// @return 返回重连结果
	bool TryReConnect(bool focusConnect,  _com_error* comError);

	/// @function 移动到开头
	/// 
	void MoveFirst();

	/// @function 获取下一个结果集
	/// 
	/// @return 成功返回true
	bool MoveNextRecordset();

private:
	_ConnectionPtr		m_Connection;			///<连接对象
	_CommandPtr			m_Command;				///<命令对象 
	_RecordsetPtr		m_Recordset;			///<记录集对象
	string				m_ConnectStr;			///<连接字符串

	unsigned long				m_ConnectCount;					//重试次数
	unsigned long				m_ConnectErrorTime;				//错误时间
	const unsigned long			m_ResumeConnectCount;			//恢复次数
	const unsigned long			m_ResumeConnectTime;			//恢复时间

	int					m_HelperNO;			///<助手编号
	string				m_PoolName;			///<池名称
	string				m_DbName;			///<db名称
	DbHelperStatus		m_Status;			///<助手状态

	friend class CMSSqlPool;

}; // end by CMSSqlHelper

} // end by Kernel

#endif //__MSSQL_HELPER_H__
#endif //__MSSQL_DB_MODULE__
#endif //_OPEN_DB_MODULE_