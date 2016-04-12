#ifndef __SERVICE_UNIT_H__
#define __SERVICE_UNIT_H__

#include "ServerKernel.h"
#include "AttemperEvent.h"
#include "IDBPool.h"
#include "CorrespondEvent.h"

/// @Breif 服务单元
class CServiceUnit
{
public:
	CServiceUnit(const char* path);
	~CServiceUnit();

	static CServiceUnit& Instance();

	/// @Brief 程序主入口
	/// 
	bool Run();

	/// @function 停止服务
	/// 
	bool Stop();

	/// @function 获取配置文件路径
	/// 
	/// @return 返回路径
	string GetPath() const { return m_Config; }

private:
	enum ServiceUnitState{ SERVICE_UNIT_START, SERVICE_UNIT_STOP };

	ServiceUnitState			m_State;			///<服务器单元状态
	Kernel::CServerKernel*		m_ServerKernel;		///<服务器引擎对象
	CAttemperEvent*				m_AttemperEvent;	///<调度事件
	CCorrespondEvent*			m_CorrespondEvent;	///<协调事件
	string						m_Config;			///<配置文件

};

#endif //__SERVICE_UNIT_H__