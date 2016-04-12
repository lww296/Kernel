#ifndef __CLIENT_KERNEL_H__
#define __CLIENT_KERNEL_H__

#include <boost/noncopyable.hpp>
#include "Types.h"
#include "IClientOut.h"
#include "IClientIn.h"

class CClientEventOut;

namespace Kernel
{
/// @Brief 客户端服务
class _KERNEL_EXPORTS CClientKernel : boost::noncopyable
{
public:
	CClientKernel(string& config, IClientIn* in);
	~CClientKernel();

	static CClientKernel& Instance();

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

	/// @function 获取是否运行
	///
	/// @return 返回结果
	bool IsRunning();

	/// @function 获取操作事件出接口
	/// 
	/// @param 返回接口
	IClientOut* GetEventOut();

	/// @function 运行事件
	/// 
	/// @param block 是否阻塞等待事件
	void RunEvent(bool block = true);

	/// @function 获取逻辑运行事件
	/// 
	/// @param data 事件数据缓冲区
	/// @param size 缓冲区大小
	/// @return 存在事件返回true
	bool RunEvent(bool block, void* data, unsigned int size);

}; // end by class CClientKernel

} // end by namespace Kernel

#endif //__CLIENT_MODULE_H__