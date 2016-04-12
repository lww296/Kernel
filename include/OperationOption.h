#ifndef __OPERATION_OPTION_H__
#define __OPERATION_OPTION_H__

#include "Types.h"

namespace Kernel
{
/// @Brief 套接字操作
namespace SocketOpt
{
	/// @function 初始化socket
	/// 
	_KERNEL_EXPORTS void InitSock();

	/// @function 清理socket
	/// 
	_KERNEL_EXPORTS void ClearSock();

	/// @function 等待
	/// @note 同步等待，内部实现是系统异步机制(select...)等待，非sleep等待
	/// 
	/// @param time 时间(毫秒为单位)
	_KERNEL_EXPORTS void Wait(unsigned int time);

	/// @function 设置套接字为非阻塞
	/// 
	/// @param sd 网络套接字 
	_KERNEL_EXPORTS void SetNonblock(int sd);

	/// @function close exec
	/// 
	/// @param sd 网络套接字
	_KERNEL_EXPORTS void SetCloseExec(int sd);

	/// @function 创建tcp套接字
	/// 
	/// @param domain 域
	/// @param isNoblock 是否设置为非阻塞
	/// @param isCloseExec 是否在调用 exec 时关闭后不可以
	/// @return 成功返回套接字, 失败返回-1
	_KERNEL_EXPORTS int CreateTcpSocket(int domain = AF_INET, bool isNonblock = true, bool isCloseExec = true);

	/// @function 创建udp套接字
	/// 
	/// @param domain 域
	/// @param isNoblock 是否设置为非阻塞
	/// @param isCloseExec 是否在调用 exec 时关闭后不可以
	/// @return 成功返回套接字, 失败返回-1
	_KERNEL_EXPORTS int CreateUdpSocket(int domain = AF_INET, bool isNonblock = true, bool isCloseExec = true);

	/// @function 连接
	/// 
	/// @param sockfd 套接字描述符
	/// @param addr 连接地址信息
	/// @return 成功返回0， 失败返回-1
	_KERNEL_EXPORTS int Connect(int sockfd, const struct sockaddr_in& addr, bool isNoblock = true);

	/// @function 连接服务器
	/// 
	/// @return 成功返回socket id, 失败返回-1
	_KERNEL_EXPORTS int ConnectServer(const char* ip, unsigned short port, bool isNonblock = true);

	/// @function 绑定
	/// 
	/// @param sockfd 套接字描述符
	/// @param localAddr 地址信息
	/// @param 成功返回true, 失败返回false
	_KERNEL_EXPORTS bool Bind(int sockfd, const struct sockaddr_in& localAddr);

	/// @function 监听
	/// 
	/// @param sockfd 套接字
	_KERNEL_EXPORTS void Listen(int sockfd);

	/// @function 接受连接
	/// 
	/// @param sockfd 套接字描述符
	/// @param addr 地址信息 
	/// @param isWSAccpet 是否是异步accept(windows环境有效)
	/// @return 成功返回套接字描述符, 失败返回-1
	_KERNEL_EXPORTS int Accept(int sockfd, struct sockaddr_in* addr, bool isWSAccpet = false);

	/// @function 接收
	/// 
	/// @param sockfd 套接字
	/// @param buf 缓冲
	/// @param count 读长度
	/// @return 返回结果, > 0: 返回读取的字节数， == 0 读到文件尾， < 0 socekt错误或socket关闭
	_KERNEL_EXPORTS int Read(int sockfd, void* buf, size_t count);

	/// @function 发送
	/// 
	/// @param sockfd 套接字
	/// @param buf 缓冲
	/// @param count 发送长度
	/// @param surplusLen 剩余数据长度
	/// @return 成功返回0，关闭或返回-1, 缓冲区已满返回1
	_KERNEL_EXPORTS int Write(int sockfd, const void* buf, size_t count);

	/// @function 发送
	/// 
	/// @param sockfd 套接字
	/// @param buf 缓冲
	/// @param count 发送长度
	/// @param surplusLen 剩余数据长度
	/// @return 成功返回0，关闭或返回-1, 缓冲区已满返回1
	_KERNEL_EXPORTS int Write(int sockfd, const void* buf, size_t count, int& surplusLen);

	/// @function 读数据
	/// 
	/// @param sockfd 套接字
	/// @param buf 缓冲
	/// @param count 读长度
	/// @param flags 操作标识
	/// @param fromAddr 接收地址信息
	/// @param fromLen 接收地址信息长度
	/// @return 返回结果, > 0: 返回读取的字节数， == 0 读到文件尾， < 0 socekt错误或socket关闭
	_KERNEL_EXPORTS int RecvFrom(int sockfd, char* buf, size_t count, unsigned int flags = 0, struct sockaddr* fromAddr = NULL, int* fromLen = NULL);

	/// @function 发送数据
	/// 
	/// @param sockfd 套接字
	/// @param buf 缓冲
	/// @param count 发送长度
	/// @param surplusLen 剩余数据长度
	/// @param flags 操作标识
	/// @param toAddr 发送地址信息
	/// @param toLen 发送地址信息长度
	/// @return 成功返回0，关闭或返回-1, 缓冲区已满返回1
	_KERNEL_EXPORTS int SendTo(int sockfd, const char* buf, size_t count, unsigned int flags = 0, const struct sockaddr* toAddr = NULL, int toLen = 0);

	/// @function 关闭套接字
	/// 
	/// @param sockfd 套接字ID
	_KERNEL_EXPORTS void Close(int sockfd);

	/// @function 中断套接字
	/// 
	/// @param sockfd 套接字ID
	_KERNEL_EXPORTS void Shutdown(int sockfd);

	/// @function 转换为网络端口
	///
	/// @param port 端口
	/// @return 返回结果
	_KERNEL_EXPORTS unsigned short Htons(unsigned short port);
	
	/// @function 转换为本地端口
	/// 
	/// @param port 端口
	/// @return 返回结果
	_KERNEL_EXPORTS unsigned short Ntohs(unsigned short port);

	/// @function 点分十进制字符串转换为整数
	/// 
	/// @param af ipv4(AF_INET) or ipv6(AF_INET6)
	/// @param src 源ip
	/// @param dst struct in_addr
	_KERNEL_EXPORTS void InetPton(int af, const char* src, void* dst);

	/// @function 整数转换为点分十进制字符串
	/// 
	/// @param af ipv4(AF_INET) or ipv6(AF_INET6)
	/// @param src struct in_addr
	/// @param dst 目的
	_KERNEL_EXPORTS void InetNtop(int af, const void* src, char* dst, size_t size);

	/// @function 转换套接字地址到网络字节序
	/// 
	/// @parma addr IP地址 或者 域名
	/// @param port 端口
	/// @param addr_in 输出套接字地址信息
	_KERNEL_EXPORTS void ConvertSockaddrToNet(const char* addr, unsigned short port, struct sockaddr_in* addr_in, SocketType sockType = SOCKET_TYPE_TCP);
	
	/// @function 转换套接字地址为本地字节序
	/// @note 转换后为字符串
	/// 
	/// @param buf 字符缓冲
	/// @param size 字符串大小
	/// @parma addr 套接字地址信息
	_KERNEL_EXPORTS void ConvertSockaddrToLocal(char* buf, size_t size, const struct sockaddr_in& addr);

	/// @function 获取本地地址信息
	/// 
	/// @param sockfd
	/// @param 返回本地地址信息
	_KERNEL_EXPORTS struct sockaddr_in GetLocalAddr(int sockfd);

	/// @function 获取对端地址信息
	/// 
	/// @param sockfd
	/// @param 返回本地地址信息
	_KERNEL_EXPORTS struct sockaddr_in GetPeerAddr(int sockfd);

	/// @function 获取套接字错误码
	/// 
	/// @return 返回错误码
	_KERNEL_EXPORTS int GetSocketError(int sockfd);

	/// @function 获取错误码
	/// 
	/// @return 返回错误码
	_KERNEL_EXPORTS int GetError();

} // end by namespace SocketOpt

/// @Brief 系统信息
namespace SystemInfo
{
	/// @function 获取当前进程Id
	/// 
	_KERNEL_EXPORTS int GetPID();

	/// @function 获取当前执行程序名称
	/// 
	_KERNEL_EXPORTS void GetAppName(char*name, int nameSize);

	/// @function 获取当前可执行程序路径
	/// 
	_KERNEL_EXPORTS void GetAppPath(char* path, int pathSize);

	/// @function 获取系统子版本号
	/// 
	/// @return 返回系统子版本号
	_KERNEL_EXPORTS int GetMinorVersion();

	/// @function 获取系统主版本号
	/// 
	/// @return 返回系统主版本号
	_KERNEL_EXPORTS int GetMajorVersion();

	/// @function 随机种子
	/// 
	_KERNEL_EXPORTS void RandSeed();

	/// @function 获取处理器核心数量
	/// 
	_KERNEL_EXPORTS unsigned int GetCoreNum();

	/// @function 获取毫秒为单位的系统时间
	/// 
	_KERNEL_EXPORTS unsigned long GetTick();

	/// @function 获取本地时间结构
	///
	_KERNEL_EXPORTS struct tm* GetLocalTime(struct tm* tmptr);

	/// @function 获取时间
	/// 
	/// @param flag (0: 简单时间(只包含年-月-日), 1: 复杂时间(包含 年-月-日 时:分:秒),
	///		2: 更精确时间(年-月-日 时:分:秒 毫妙) 3: 更精确时间(年-月-日 时:分:秒 微妙))
	/// @param retTime 返回时间buf
	/// @param retTimeSize buf大小
	/// @return 返回北京时间
	_KERNEL_EXPORTS const char *GetLocalTime(int flag, char* retTime, int retTimeSize);

	/// @function 获取时间(UTC毫秒时间)
	/// 
	/// @return 返回utc毫秒时间
	_KERNEL_EXPORTS uint64_t GetTime();

} // end by SystemInfo

/// @Brief 引擎操作选项
namespace EngineOption
{
	/// @function 获取选项信息
	/// 
	/// @param optType 选项类型
	/// @param out 选项信息
	/// @param outLen 信息长度
	/// @param 返回是否有该选项信息
	_KERNEL_EXPORTS int64_t GetOption(int optType);
	_KERNEL_EXPORTS bool GetOption(int optType, char* out, int* outLen);
	_KERNEL_EXPORTS int64_t GetOption(int optType, int64_t param1, int64_t param2 = 0);
	_KERNEL_EXPORTS int64_t GetOption(int optType, const char* in, int inLen, char* out, int* outLen);

	/// @function 设置选项信息
	/// 
	/// @param optType 选项类型
	/// @param value 选项信息
	/// @param valueLen 信息长度
	/// @param 返回选项是否设置成功
	_KERNEL_EXPORTS bool SetOption(int optType, const char* value, int valueLen);
	_KERNEL_EXPORTS bool SetOption(int optType, int64_t param1, int64_t param2 = 0);
	_KERNEL_EXPORTS bool SetOption(int optType, const char* param1, const char* param2 = NULL);

} // end namespace EngineOption

} // end by namespace Kernel

#endif //__OPERATION_OPTION_H__