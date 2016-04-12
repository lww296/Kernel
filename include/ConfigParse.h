#ifndef __CONFIG_PARSE_H__
#define __CONFIG_PARSE_H__

#include "Types.h"
#include <boost/unordered_map.hpp>

namespace Kernel
{

/// @brief 解析.conf类型的配置文件
class _KERNEL_EXPORTS CConfigParse
{
public:
	CConfigParse(string path = "");
	~CConfigParse();

	/// @function 解析配置
	/// 
	/// @param path 配置文件路径
	/// @return 返回解析结果
	bool Parse(string path);

	/// @function 获取配置数据
	/// 
	/// @param key 配置键
	/// @return 返回配置数据
	string Get(string key);

	/// @function 获取配置数据
	/// 
	/// @param key 配置键
	/// @return 返回配置数据
	bool GetBool(string key);

	/// @function 获取配置数据
	/// 
	/// @param key 配置键
	/// @return 返回配置数据
	short GetShort(string key);

	/// @function 获取配置数据
	/// 
	/// @param key 配置键
	/// @return 返回配置数据
	int GetInt(string key);

	/// @function 获取配置数据
	/// 
	/// @param key 配置键
	/// @return 返回配置数据
	long long GetLongLong(string key);

	/// @function 获取配置数据
	/// 
	/// @param key 配置键
	/// @return 返回配置数据
	double GetDouble(string key);

	/// @function 输出
	/// 
	void Print();

private:
	string		m_ConfigPath;		/// 配置文件路径
	boost::unordered_map<string, string>	m_Datas;	/// 数据

};


}

#endif //__CONFIG_PARSE_H__