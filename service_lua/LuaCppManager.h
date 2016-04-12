#ifndef __LUA_CPP_MANAGER_H__
#define __LUA_CPP_MANAGER_H__

#include <boost/unordered_map.hpp>

#include "LuaEventOut.h"
#include "LuaClientEventOut.h"
#include "MSSqlPool.h"
#include "MysqlPool.h"
#include "RedisPool.h"
#include "ObjectPool.h"
#include "DistributedService.h"
#include "LuaCppDefine.h"
#include "MutexLock.h"

class CLuaCppModule;

typedef Kernel::CActiveObjectMap<string, CLuaCppModule, Kernel::CMutexLock>			LuaCppModuleMap;
typedef Kernel::CActiveObjectMap<string, CDistributedService, Kernel::CMutexLock>	DistributeServiceMap;

/// @brief 模块管理
class CLuaCppManager
{
public:
	CLuaCppManager();
	~CLuaCppManager();

	static CLuaCppManager& Instance();

	/// @function 初始化
	/// 
	/// @return 成功返回true, 失败返回false
	bool Init();

	/// @function 启动
	/// 
	/// @return 成功返回true, 失败返回false
	bool Start();

	/// @function 停止
	/// 
	/// @return 成功返回true, 失败返回false
	bool Stop();

	/// @function 重新加载脚本
	/// 
	/// @return 成功返回true, 失败返回false
	bool Reload();

	/// @function 设置主逻辑服务名
	/// 
	/// @param name 服务名
	void SetMainLogic(string name) { m_MainLogicName = name; }

	/// @function 创建模块
	/// 
	/// @param serviceName 服务名
	/// @param serviceName 服务名
	/// @param sciprtRootPath 脚本跟目录
	/// @param serverRootPath 当前运行服务器脚本跟目录
	/// @param isCreate 是否创建服务队列
	/// @return 返回结果
	CLuaCppModule* CreateModule(string serviceName, string scriptRootPath, string serverRootPath, bool isCreate = false);

	/// @function 查找模块
	/// 
	/// @return 返回模块
	CLuaCppModule* FindModule(string serviceName);

	/// @function 移除模块
	/// 
	/// @param serviceName 服务名
	void DestoryModule(string serviceName);

	/// @function 清空
	/// 
	void Clear();

	/// @function 设置事件出接口
	/// 
	/// @param out 接口对象
	void SetEventOut(IEventOut* out) { m_EventOut = out; m_LuaEventOut->SetEventOut(out); }

	/// @function 获取事件出接口
	/// 
	/// @return 接口对象
	IEventOut* GetEventOut() { return m_EventOut; }

	/// @function 设置事件出接口
	/// 
	/// @param out 接口对象
	void SetClientEventOut(IClientOut* out) { m_ClientOut = out;  m_LuaClientEventOut->SetEventOut(out); }

	/// @function 获取事件出接口
	/// 
	/// @return 接口对象
	IClientOut* GetClientOut() { return m_ClientOut; }

	/// @function 获取事件出接口
	/// 
	/// @return 接口对象
	CLuaEventOut* GetLuaEventOut() { return m_LuaEventOut; }

	/// @function 获取事件出接口
	/// 
	/// @return 接口对象
	CLuaClientEventOut* GetLuaClientEventOut() { return m_LuaClientEventOut; }

	/// @function 创建服务
	/// 
	/// @param serviceName 服务名
	/// @param remoteAddr 远程服务地址(非本地服务)
	/// @param sessionID 远程服务会话id(本地服务默认0)
	bool CreateService(string serviceName, const Kernel::CAddress* remoteAddr = NULL, double sessionID = 0);

	/// @function 查找服务
	/// 
	/// @param serviceName 服务名
	/// @return 返回服务
	CDistributedService* FindService(string serviceName);
	bool IsService(string serviceName);

	/// @function 删除服务
	/// 
	/// @param serviceName 服务名
	void DeleteService(string serviceName);

	/// @functioin 向服务发送数据
	/// 
	/// @param srcService 请求服务名
	/// @param detService 目标服务名
	/// @param func 服务内函数
	/// @param session 会话
	/// @param param 参数
	/// @return 返回发送结果
	bool Send(string srcService, string detService, string func, string param);

	/// @functioin 向服务发送数据
	/// 
	/// @param srcService 请求服务名
	/// @param detService 目标服务名
	/// @param func 服务内函数
	/// @param param 参数
	/// @param timeout 超时时间(0:永不超时)
	/// @return 返回数据
	string Call(string srcService, string detService, string func, string param, int timeout);

private:
	CLuaEventOut*			m_LuaEventOut;				/// lua服务器事件出
	CLuaClientEventOut*		m_LuaClientEventOut;		/// lua客户端事件出
	IEventOut*				m_EventOut;					/// 服务器事件出接口
	IClientOut*				m_ClientOut;				/// 客户端事件出接口
	eServiceStatus			m_Status;					/// 状态
	string					m_MainLogicName;			/// 主逻辑服务名

	LuaCppModuleMap			m_LuaCppModuleMap;			/// 模块列表
	DistributeServiceMap	m_DistributedServiceMap;	/// 服务列表
};

// lua事件接口
extern CLuaEventOut& GetServerLuaOut();
extern CLuaClientEventOut& GetClientLuaOut();

#ifdef _OPEN_DB_MODULE_

#ifdef __MSSQL_DB_MODULE__
extern Kernel::CMSSqlPool& GetMSSqlPool();
#endif //__MSSQL_DB_MODULE__

#ifdef __MYSQL_DB_MODULE__
extern Kernel::CMysqlPool& GetMysqlPool();
#endif //__MYSQL_DB_MODULE__

#ifdef __REDIS_DB_MODULE__
extern Kernel::CRedisPool& GetRedisPool();
#endif //__REDIS_DB_MODULE__

#endif //_OPEN_DB_MODULE_


#endif //__LUA_CPP_MANAGER_H__