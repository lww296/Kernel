#ifndef __SOCKET_HELPER_SERVER_H__
#define __SOCKET_HELPER_SERVER_H__

#include "Types.h"
#include "Define.h"

// 带服务器事件的网络助手

//////////////////////////////////////////////////////////////////////////

/// @function 创建网络助手对象
/// 
/// @param path 配置文件路径
extern "C" _KERNEL_EXPORTS bool CreateNetService(const char* path);

/// @function 销毁网络助手对象
///
extern "C" _KERNEL_EXPORTS void DestoryNetService();

/// @function 获取网络操作事件
/// 
/// @param isBlock 是否阻塞获取
/// @param buf 缓冲区
/// @param bufSize 缓冲区大小
/// @param 返回事件结果，存在事件返回true
extern "C" _KERNEL_EXPORTS bool RunLogicEvent(bool isBlock, void* buf, unsigned int bufSize);

/// @function 主动获取网络操作事件
/// 
/// @param logicPacket 逻辑事件包结构体
/// @param bufSize 缓冲区大小
/// @param 返回事件结果，存在事件返回true
extern "C" _KERNEL_EXPORTS bool RunLogicEventToStruct(Kernel::LogicPacket* logicPacket, unsigned int bufSize);

//////////////////////////////////////////////////////////////////////////
/// server

/// @function 创建绑定套接字
/// 
/// @param addr 连接地址信息
/// @param bindAddr 绑定地址信息
/// @param bindID 绑定标识(绑定多个端口时，bindID必须不一样, 0: 自动分配)
/// @param sync 同步or异步
/// @return 成功返回绑定标识，失败返回-1
extern "C" _KERNEL_EXPORTS int CreateBindSocket(const char* addr, unsigned short port, int bindID, bool sync);

/// @function 删除绑定套接字
/// 
/// @param bindID 绑定标识
/// @param sync 同步or异步
/// @return 成功返回true
extern "C" _KERNEL_EXPORTS bool DestoryBindSocket(unsigned int bindID, bool sync);

/// @function 设置用户到会话id
/// 
extern "C" _KERNEL_EXPORTS void SetUserToSession(uint64_t sessionID, uint64_t userId);

/// @function 根据会话获取用户id
/// 
extern "C" _KERNEL_EXPORTS uint64_t GetSessionUserID(uint64_t sessionID);

/// @function 发送数据
/// 
/// @param sessionID 会话ID
/// @param buf 数据
/// @param size 长度
/// @param addr 地址信息(udp协议。必须填写此字段)
/// @param level 数据级别
/// @return 返回发送结果
extern "C" _KERNEL_EXPORTS bool SendS(uint64_t sessionID, const void* buf, size_t size, const Kernel::CAddress* addr, int level);

/// @function 广播数据
/// 
/// @param buf 数据
/// @param size 长度
extern "C" _KERNEL_EXPORTS void BroadcastS(const void* buf, size_t size);

/// @function 关闭用户连接
/// 
/// @param sessionID 会话ID
extern "C" _KERNEL_EXPORTS void CloseS(uint64_t sessionID);

/// @function 中断用户连接
/// 
/// @param sessionID 会话ID
extern "C" _KERNEL_EXPORTS void ShutdownS(uint64_t sessionID);

/// @function 设置定时器
/// 
/// @param timerID 定时器编号(必须大于10)
/// @param interval 定时器运行持续次数
/// @param overTimer 每轮到期时间(毫秒为单位)
/// @param param 参数
/// @param size 参数数据长度
/// @return 返回结果，成功返回true，失败返回false
extern "C" _KERNEL_EXPORTS bool SetTimerS(unsigned int timerID, unsigned int interval, unsigned int overTime, void* param, unsigned short size);

/// @function 移除定时器
/// 
/// @parma timerID 定时器编号
/// @return 返回结果
extern "C" _KERNEL_EXPORTS bool KillTimerS(unsigned int timerID);

/// @function 投递数据库操作请求
/// 
/// @param sessionID 会话ID(上下文)
/// @param requestID 数据库请求ID
/// @param data 数据
/// @param size 大小
/// @param exter 扩展值
/// @param 返回投递结果
extern "C" _KERNEL_EXPORTS bool PostDatabaseRequest(uint64_t sessionID, unsigned int requestID, const void* data, unsigned short size, uint64_t exter);

/// @function 投递数据库事件结果
/// 
/// @param sessionID 会话ID(上下文)
/// @param dbID 数据库请求ID
/// @param data 数据
/// @param size 大小
/// @param local 是否本地处理
/// @param exter 扩展值
/// @param 返回投递结果
extern "C" _KERNEL_EXPORTS bool PostDatabaseEvent(uint64_t sessionID, unsigned int dbID, const void* data, unsigned short size, bool IsLocal, uint64_t exter);

/// @function 自定义控制事件
/// 
/// @param eventId 事件id
/// @param data 数据
/// @param size 数据大小
/// @return 返回结果
extern "C" _KERNEL_EXPORTS bool PostSelfEvent(unsigned int eventID, const void* data, unsigned short size);

/// @function 投递任务到线程中运行
///
/// @param callback 任务回调函数
/// @param taskName 任务名称
/// @param param 回调函数参数
/// @param runTime 运行次数(0: 永久运行)
/// @return 投递成功返回true
extern "C" _KERNEL_EXPORTS bool PostUserTask(Kernel::UserTaskCallParam callback, string taskName, const void* param, unsigned int runTime);

/// @function 投递任务到线程中运行
/// @note 线程单独运行此任务(独立运行)
/// @note 引擎启动后才在线程中独立运行。如果引擎没启动将投递到任务队列
///
/// @param callback 任务回调函数
/// @param taskName 任务名称
/// @return 投递成功返回true
extern "C" _KERNEL_EXPORTS bool PostUserTask1(string taskName, Kernel::UserTaskCallback callback);

/// @function 投递任务到线程中运行
///
/// @param callback 任务回调函数
/// @param taskName 任务名称
/// @param runTime 运行次数(0: 永久运行)
/// @return 投递成功返回true
extern "C" _KERNEL_EXPORTS bool PostUserTask2(Kernel::UserTaskCallback callback, string taskName, unsigned int runTime);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// client

/// @function 连接服务器
/// 
/// @param sessionID 编号(会话id)
/// @param ip ip地址
/// @param port 端口
/// @param type 套接字协议类型
extern "C" _KERNEL_EXPORTS uint64_t ConnectC(const char* ip, unsigned short port, SocketType type = SOCKET_TYPE_TCP, uint64_t sessionID = 0);

/// @function 发送数据
/// 
/// @param sessionID 会话id
/// @param data 数据
/// @param size 数据大小
/// @return 返回结果
extern "C" _KERNEL_EXPORTS bool SendDataC(uint64_t sessionID, const void* data, unsigned short size);

/// @function 关闭用户连接
/// 
/// @param sessionID 会话id
extern "C" _KERNEL_EXPORTS void CloseC(uint64_t sessionID);

/// @function 中断用户连接
/// 
/// @param sessionID 会话id
extern "C" _KERNEL_EXPORTS void ShutdownC(uint64_t sessionID);

/// @function 设置定时器
/// 
/// @param timerID 定时器编号(必须大于10)
/// @param interval 定时器运行持续次数
/// @param overTimer 每轮到期时间(毫秒为单位)
/// @param param 参数
/// @param size 参数数据长度
/// @return 返回结果，成功返回true，失败返回false
extern "C" _KERNEL_EXPORTS bool SetTimerC(unsigned int timerID, unsigned int interval, unsigned int overTime, const void* param, unsigned short size);

/// @function 移除定时器
/// 
/// @parma timerID 定时器编号
/// @return 返回结果
extern "C" _KERNEL_EXPORTS bool KillTimerC(unsigned int timerID);

#endif //__SOCKET_HELPER_SERVER_H__