#ifndef __DISTRIBUTED_SERVICE_H__
#define __DISTRIBUTED_SERVICE_H__

#include "Types.h"
#include "BlockQueue.h"
#include "Address.h"
#include "LuaCppModule.h"
#include "LuaCppDefine.h"
#include "ThreadPackage.h"
#include "MutexLock.h"

typedef Kernel::CLockfreeBlockQueue<Kernel::CTEventPackage*>	DistributedQueue;		/// 服务队列
typedef Kernel::CLockfreeBlockQueue<Kernel::CTEventPackage*, Kernel::CMutexLock, false, 3000> CallServiceQueue;	/// 服务调用队列

// 分布式服务
class CDistributedService
{
public:
	CDistributedService(string serviceName, const Kernel::CAddress* remoteAddr = NULL, bool isCreate = false);
	~CDistributedService();

	/// @functin 初始化
	/// 
	/// @return 成功返回true, 失败返回false
	bool Init();

	/// @function 启动服务器
	/// 
	/// @return 成功返回true, 失败返回false
	bool Start();

	/// @function 停止服务器
	/// 
	/// @return 成功返回true, 失败返回false
	bool Stop();

	/// @function 是否是本地服务
	/// 
	bool IsLocalService() { return m_LoaclService; }

	/// @function 设置lua模块
	/// 
	void SetLuaMoudle(CLuaCppModule* moudle) { m_LuaModule = moudle; }

	/// @function 获取服务队列
	/// 
	/// @return 返回队列
	DistributedQueue* GetQueue() { return m_ServiceQueue; }

	/// @functioin 向服务发送数据
	/// 
	/// @param serviceName 服务名
	/// @param func 服务内函数
	/// @param session 会话
	/// @param param 参数
	/// @return 返回发送结果
	bool Send(string serviceName, string func, string param);

	/// @functioin 向服务发送数据
	/// 
	/// @param serviceName 服务名
	/// @param func 服务内函数
	/// @param param 参数
	/// @param timeout 超时时间(0:永不超时)
	/// @return 返回数据
	string Call(string serviceName, string func, string param, int timeout = 3000);

	/// @function 运行
	/// 
	void Run();

protected:
	bool					m_IsRunning;			/// 是否运行
	bool					m_LoaclService;			/// 是否本地服务
	bool					m_IsCreateQueue;		/// 是否创建服务队列
	string					m_ServiceName;			/// 服务名称
	DistributedQueue*		m_ServiceQueue;			/// 服务队列
	Kernel::CAddress		m_RemoteAddr;			/// 远程服务地址
	uint64_t				m_SessionID;			/// 远程服务会话

	CLuaCppModule*			m_LuaModule;			/// lua模块
	eServiceStatus			m_Status;				/// 状态
	CallServiceQueue		m_CallWait;				/// 服务call调用等待

}; // end class CDistributedService

#endif //__DISTRIBUTE_SERVICE_H__