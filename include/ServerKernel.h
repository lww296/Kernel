#ifndef __SERVER_KERNEL_H__
#define __SERVER_KERNEL_H__

#include <list>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "Types.h"
#include "Address.h"
#include "IEventIn.h"
#include "IEventOut.h"
#include "IClientOut.h"
#include "IClientIn.h"
#include "IDBPool.h"

namespace Kernel
{
/// @Brief 服务器引擎
class _KERNEL_EXPORTS CServerKernel : boost::noncopyable
{
public:
	typedef boost::function<bool ()> ServiceStopCall;

	CServerKernel(string& config, IEventIn* in, IClientIn* cin);
	~CServerKernel();

	static CServerKernel& Instance();

	/// @function 初始化服务
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

	/// @function 设置服务器停止回调
	/// 
	void SetStopCall(ServiceStopCall call);

	/// @function 通知服务器可以正式关闭
	///
	/// @note 底层引擎将关闭
	void SetServerRealStop();

	/// @function 获取操作事件出接口
	/// 
	/// @param 返回接口
	IEventOut* GetEventOut();

	/// @function 获取操作事件出接口
	/// 
	/// @param 返回接口
	IClientOut* GetClientEventOut();

	/// @function 获取支持数据库列表
	/// 
	/// @return 返回list列表
	const std::list<string>& GetSupportDB();

	/// @function 获取数据库池对象
	/// 
	/// @param str 数据库池字符串
	IDBPool* GetPool(const string& str);

	/// @function 获取逻辑事件
	/// 
	/// @param data 事件数据缓冲区
	/// @param size 缓冲区大小
	/// @return 存在事件返回true
	bool RunLogicEvent(bool block, void* data, unsigned int size);
	
}; // end by class CServerKernel

} // end by namespace Kernel

#endif //__SERVER_KERNEL_H__