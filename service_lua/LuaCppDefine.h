#ifndef __LUA_CPP_DEFINE_H__
#define __LUA_CPP_DEFINE_H__

//////////////////////////////////////////////////////////////////////////
/// lua服务名定义

// 主逻辑
#define MAIN_LOGIC			"main_logic"

// 数据库逻辑
#define DB_LOGIC			"db_logic"


//////////////////////////////////////////////////////////////////////////
/// lua 环境变量设置 key

/// 脚本跟目录
#define SCRIPT_ROOT_PATH	"script_root_path"	

/// 当前运行服务器脚本跟目录
#define SERVER_ROOT_PATH	"server_root_path"

// 服务名
#define SERVICE_NAME		"service_name"


//////////////////////////////////////////////////////////////////////////


// 服务状态
enum eServiceStatus
{
	SERVICE_STATUS_NULL,		// 无状态
	SERVICE_STATUS_INIT,		// 初始化
	SERVICE_STATUS_START,		// 开始
	SERVICE_STATUS_STOP			// 停止
};



#endif //__LUA_CPP_DEFINE_H__