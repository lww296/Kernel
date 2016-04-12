#ifndef __SOCKET_HELPER_H__
#define __SOCKET_HELPER_H__

#include "Types.h"
#include "Define.h"

/// @function 创建网络助手对象
/// 
/// @param path 配置文件路径
extern "C" _KERNEL_EXPORTS bool CreateNetHelper(const char* path);

/// @function 销毁网络助手对象
///
extern "C" _KERNEL_EXPORTS void DestoryNetHelper();

/// @function 主动获取网络操作事件
/// 
/// @param buf 缓冲区
/// @param bufSize 缓冲区大小
/// @param 返回事件结果，存在事件返回true
extern "C" _KERNEL_EXPORTS bool NetLogicEvent(void* buf, unsigned int bufSize);

/// @function 主动获取网络操作事件
/// 
/// @param logicPacket 逻辑事件包结构体
/// @param bufSize 缓冲区大小
/// @param 返回事件结果，存在事件返回true
extern "C" _KERNEL_EXPORTS bool NetLogicEventToStruct(Kernel::LogicPacket* logicPacket, unsigned int bufSize);

/// @function 获取网络操作事件
/// 
/// @param isBlock 是否阻塞获取
/// @param buf 缓冲区
/// @param bufSize 缓冲区大小
/// @param 返回事件结果，存在事件返回true
extern "C" _KERNEL_EXPORTS bool NetRunEvent(bool isBlock, void* buf, unsigned int bufSize);

/// @function 连接服务器
/// 
/// @param sessionID 编号(会话id)
/// @param ip ip地址
/// @param port 端口
/// @param type 套接字协议类型
extern "C" _KERNEL_EXPORTS uint64_t Connect(const char* ip, unsigned short port, SocketType type = SOCKET_TYPE_TCP, uint64_t sessionID = 0);

/// @function 发送数据
/// 
/// @param sessionID 会话id
/// @param data 数据
/// @param size 数据大小
/// @return 返回结果
extern "C" _KERNEL_EXPORTS bool SendData(uint64_t sessionID, const void* data, unsigned short size);

/// @function 关闭用户连接
/// 
/// @param sessionID 会话id
extern "C" _KERNEL_EXPORTS void Close(uint64_t sessionID);

/// @function 中断用户连接
/// 
/// @param sessionID 会话id
extern "C" _KERNEL_EXPORTS void Shutdown(uint64_t sessionID);

/// @function 设置定时器
/// 
/// @param timerID 定时器编号(必须大于10)
/// @param interval 定时器运行持续次数
/// @param overTimer 每轮到期时间(毫秒为单位)
/// @param param 参数
/// @param size 参数数据长度
/// @return 返回结果，成功返回true，失败返回false
extern "C" _KERNEL_EXPORTS bool SetTimerExt(unsigned int timerID, unsigned int interval, unsigned int overTime, const void* param, unsigned short size);

/// @function 移除定时器
/// 
/// @parma timerID 定时器编号
/// @return 返回结果
extern "C" _KERNEL_EXPORTS bool KillTimerExt(unsigned int timerID);

/// @function 获取选项信息
/// 
/// @param optType 选项类型
/// @param 返回是否有该选项信息
extern "C" _KERNEL_EXPORTS int64_t GetEngineOption(int optType);

#endif //__SOCKET_HELPER_H__