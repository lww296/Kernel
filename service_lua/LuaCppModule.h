#include "Types.h"
#ifdef _OPEN_LUA_MODULE_
#ifndef __LUA_CPP_MODULE_H__
#define __LUA_CPP_MODULE_H__

#include "LuaCppDefine.h"
#include "Address.h"

#ifdef __WIN_PLATFORM
#ifdef _DEBUG
#pragma comment(lib, "lua5.1.lib")
#else
#pragma comment(lib, "lua51.lib")
#endif
#pragma comment(lib, "luabind.lib")
#endif

// lua module 支持
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "luabind/luabind.hpp"
#include "luabind/function.hpp"
#include "luabind/class.hpp"
#include "luabind/object.hpp"
#include "luabind/adopt_policy.hpp"
#include "luabind/container_policy.hpp"
#include "luabind/copy_policy.hpp"
#include "luabind/dependency_policy.hpp"
#include "luabind/discard_result_policy.hpp"
#include "luabind/iterator_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/raw_policy.hpp"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/get_main_thread.hpp"
//#include "luabind/operator.hpp"

// for stl
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <stack>

// lua 错误回调函数
extern int PcallbackErrFun(lua_State* L);

using namespace luabind;
using namespace std;

// lua 模块(note: 你想使用lua模块逻辑，请先安装lua)
class CLuaCppModule
{
public:
	/// @param serviceName 服务名
	/// @param sciprtRootPath 脚本跟目录
	/// @param serverRootPath 当前运行服务器脚本跟目录
	CLuaCppModule(string serviceName, string sciprtRootPath, string serverRootPath);
	~CLuaCppModule();

	/// @function 获取lua虚拟机对象
	/// 
	/// @return 返回lua虚拟机对象
	lua_State* GetLuaState() { return L; }

	/// @function 获取脚本路径
	/// 
	string GetServerPath() const { return m_ServerRootPath; }

	/// @function 初始化
	/// @note 从脚本文件路径中，加载LoadModule.lua, ServerEvent.lua
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

	/// @function 本地创建服务结果
	/// 
	/// @param serviceName 服务信息
	/// @param result 创建服务返回结果
	void LocalCreateService(string serviceName, int result) /*= 0*/;

	/// @function 本地删除服务结果
	/// 
	/// @param result 删除服务返回结果
	void LocalDeleteService(string serviceName, int result) /*= 0*/;

	/// @function 远程请求创建服务
	/// 
	/// @param serviceName 服务信息
	/// @param result 创建服务返回结果
	void RemoteCreateService(string serviceName, uint64_t sessioinID, const Kernel::CAddress* addr) /*= 0*/;

	/// @function 远程请求删除服务
	/// 
	/// @param result 删除服务返回结果
	void RemoteDeleteService(string serviceName, bool result) /*= 0*/;

	/// @function 加载lua文件
	/// 
	bool LoadFile(const char* fileName);

	/// @function c++注册类，函数到lua
	/// 
	bool RegisterAll();

	/// @funtion 添加lua包路径
	/// 
	/// @param path 包路径
	/// @param is_cpath 是否是库环境
	void AddPackagePath(string path, bool is_cpath = false);

	/// @function 设置虚拟机运行环境
	///
	/// @param key 键
	/// @param val 值
	void SetEnv(string key, string val);

	/// @functioin 获取环境设置
	/// 
	string GetEnv(string key);

	/// @function 转换stl vector list stack queue set 成lua表
	/// @note 顺序索引
	/// 
	/// @return 返回对象
	template<class T>
	luabind::object& ConvertIndexTable(T stl, luabind::object &obj)
	{
		obj = luabind::newtable(L);
		typename T::iterator it = stl.begin();
		for(int i = 1; it != stl.end(); ++it, ++i)
		{
			obj[i] = *it;
		}
		return obj;
	}

	/// @function 转换stl map成lua表
	/// @note k,v索引
	/// 
	/// @return 返回对象
	template<class T>
	luabind::object& ConvertMapTable(T stl, luabind::object &obj)
	{
		obj = luabind::newtable(L);
		typename T::iterator it = stl.begin();
		for( ; it != stl.end(); ++it)
		{
			obj[it->first] = it->second;
		}
		return obj;
	}

private:
	lua_State*		L;					/// lua虚拟机对象
	string			m_ServiceName;		/// 服务名
	string			m_ScriptRootPath;	/// 脚本跟目录
	string			m_ServerRootPath;	/// 服务器跟目录
	eServiceStatus	m_Status;			/// 状态

};

#endif // __LUA_CPP_MODULE_H__
#endif // _OPEN_LUA_MODULE_