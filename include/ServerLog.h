#ifndef __SERVER_LOG_H_
#define __SERVER_LOG_H_

#include <boost/noncopyable.hpp>

#include "Types.h"
#include "MutexLock.h"

namespace Kernel
{

/// @Brief 日志级别
enum LogLevel
{
	LOG_LEVEL_NONE,				// 无

	LOG_LEVEL_NORMAL,			// 常规
	LOG_LEVEL_NORMAL_SYSTEM,	// 系统常规
	LOG_LEVEL_DEBUG,			// 调试
	LOG_LEVEL_DEBUG_SYSTEM,		// 系统调试
	LOG_LEVEL_WARNING,			// 警告
	LOG_LEVEL_WARNING_SYSTEM,	// 系统警告
	LOG_LEVEL_ERROR,			// 错误
	LOG_LEVEL_ERROR_SYSTEM,		// 系统错误

	LOG_LEVEL_MAX	
};

//////////////////////////////////////////////////////////////////////////
/// 写日志直接调用的宏
#if (defined(__WIN_PLATFORM) && _MSC_VER < 1500)
#define WRITE_LOG							Kernel::WriteLogWin32
#define WRITE_LOG_NORMAL					Kernel::WriteLogWin32		
#define WRITE_LOG_NORMAL_SYSTEM				Kernel::WriteLogWin32	
#define WRITE_LOG_DEBUG						Kernel::WriteLogWin32	
#define WRITE_LOG_DEBUG_SYSTEM				Kernel::WriteLogWin32
#define WRITE_LOG_WARNING					Kernel::WriteLogWin32	
#define WRITE_LOG_WARNING_SYSTEM			Kernel::WriteLogWin32
#define WRITE_LOG_ERROR						Kernel::WriteLogWin32
#define WRITE_LOG_ERROR_SYSTEM				Kernel::WriteLogWin32
#else
#define WRITE_LOG(level, fmt, ...)			APIWriteLog(__FILE__, __LINE__, level, fmt, ##__VA_ARGS__)
#define WRITE_LOG_NORMAL(fmt, ...)			WRITE_LOG(LOG_LEVEL_NORMAL, fmt, ##__VA_ARGS__)
#define WRITE_LOG_NORMAL_SYSTEM(fmt, ...)	WRITE_LOG(LOG_LEVEL_NORMAL_SYSTEM, fmt, ##__VA_ARGS__)
#define WRITE_LOG_DEBUG(fmt, ...)			WRITE_LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define WRITE_LOG_DEBUG_SYSTEM(fmt, ...)	WRITE_LOG(LOG_LEVEL_DEBUG_SYSTEM, fmt, ##__VA_ARGS__)
#define WRITE_LOG_WARNING(fmt, ...)			WRITE_LOG(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define WRITE_LOG_WARNING_SYSTEM(fmt, ...)	WRITE_LOG(LOG_LEVEL_WARNING_SYSTEM, fmt, ##__VA_ARGS__)
#define WRITE_LOG_ERROR(fmt, ...)			WRITE_LOG(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define WRITE_LOG_ERROR_SYSTEM(fmt, ...)	WRITE_LOG(LOG_LEVEL_ERROR_SYSTEM, fmt, ##__VA_ARGS__)

// 终端输出
#define WRITE_SCREEN(fmt, ...)				fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

//////////////////////////////////////////////////////////////////////////

#define	LOG_FILE_DIR_LEN		256				// 日志文件路径最大长度
#define LOG_FILE_NAME_LEN		128				// 日志文件名长度
#define LOG_FILE_LEN_MAX		50000000		// 单个日志文件最大长度(字节为单位)50M
#define LOG_MSG_SIZE			4096			// 单条日志消息最大长度(字节为单位)
#define REOPEN_LOG_FILE_FAIL	"open logic file fail\n"
#define LOG_SINGLE_NUM			30				// 单次最大写入数量

/// @日志文件状态
enum
{
	LOG_STAT_CLOSE,				// 关闭
	LOG_STAT_OPEN,				// 打开
	LOG_STAT_WRITE,				// 可写
	LOG_STAT_NO_WRITE,			// 不可写
	LOG_STAT_PRE_CLOSE,			// 准备关闭
	LOG_STAT_DISBALE_WRITE,		// 禁止写
};

/// @日志时间格式
enum
{
	LOGTIME_FMT_YMD,			// 简单时间(年-月-日)
	LOGTIME_FMT_YMDHMS,			// 复杂时间(年-月-日 时:分:秒)
	LOGTIME_FMT_YMDHMSM,		// 更精确时间(年-月-日 时:分:秒 毫妙)
	LOGTIME_FMT_YMDHMSU,		// 更精确时间(年-月-日 时:分:秒 微妙)
};

/// @Brief 日志文件信息
typedef struct LogFileInfo
{		
	int				m_Fd;							///<文件描述符
	uint64_t		m_CurLogNum;					///<当前日志文件大小
	short			m_CurLogStatus;					///<当前文件状态(0:关闭, 1:打开)
	char			m_LogDir[LOG_FILE_DIR_LEN];		///<日志文件目录
	char			m_LogName[LOG_FILE_NAME_LEN];	///<日志文件名
	char			m_LogPath[LOG_FILE_DIR_LEN + LOG_FILE_NAME_LEN];	///<日志文件路径
}LogFileInfo;

/// @Brief 服务器日志类
class _KERNEL_EXPORTS CServerLog : boost::noncopyable
{
public:
	/// @function 构造函数
	/// 
	/// @param anync 是否异步记录日志
	/// @param lowerLevel 记录日志的最低级别，低于此基本将不记录日志
	CServerLog(bool anync = false, int lowerLevel = Kernel::LOG_LEVEL_NORMAL, int timeFmt = Kernel::LOGTIME_FMT_YMDHMS);
	~CServerLog();

	static CServerLog& Instance();

	/// @funtion 初始化日志服务
	/// 
	/// @param dir 日志文件目录
	/// @param filename 日志文件名
	/// @param flag ( O_RDONLY | O_WRONLY | O_RDWR | O_CREAT | O_APPEND | O_TRUNC )
	/// @param mode 权限
	bool InitLog(const char* dir, const char* filename = NULL, int flag = O_RDWR | O_CREAT, int mode = 0644);

	/// @function 停止日志服务
	/// 
	void StopLogServer();

	/// @function 写日志
	///
	/// @param file 文件函数
	/// @param line 所在行
	/// @param fmt 格式化参数
	/// @param isOutScreen 是否输出到终端
	bool WriteLog(const char* file, int line, int level, const char* text, bool isOutScreen = false);

	/// @function 设置是否异步写日志
	/// 
	/// @param async 
	void SetIsAsync(bool async);

	/// @function 设置记录日志最低级别
	/// 
	/// @param async 
	void SetLowerLevel(unsigned int level);

	/// @function 将日志回写文件
	/// 
	/// @param data 日志数据
	void WriteToFile(unsigned int num = LOG_SINGLE_NUM);

	/// @function 是否投递写日志事件
	/// 
	bool IsPostWriteLog();

private:

	/// @function 屏幕输出
	/// 
	/// @param fmt 格式化参数
	void ScreenOutput(const char* fmt, ...);

	/// @function 真正的写日志，直接写日志到文件
	///
	/// @param data 数据
	/// @param size 大小
	void RealWriteLog(const char* data, unsigned int size);

	/// @function 打开日志文件
	/// 
	/// @param path 日志文件路径
	/// @param flag ( _O_RDONLY | _O_WRONLY | _O_RDWR | _O_CREAT | _O_APPEND | _O_TRUNC )
	/// @param mode 权限 
	int OpenLogFile(const char *path, int flag, int mode);

	/// @function 获取时间
	/// 
	/// @param flag (0: 简单时间(只包含年-月-日), 1: 复杂时间(包含 年-月-日 时:分:秒),
	///		2: 更精确时间(年-月-日 时:分:秒 毫妙) 3: 更精确时间(年-月-日 时:分:秒 微妙))
	/// @param retTime 返回时间buf
	/// @param retTimeSize buf大小
	/// @return 返回北京时间
	const char *GetLocalTime(int flag, char* retTime, int retTimeSize);

	/// @function 关闭日志文件
	/// 
	void CloseLogFile();

	/// @function 获取日志文件名
	/// 
	/// @param isSelect 是否查找新的文件
	void GetLogFileName(char* name, int nameSize, bool isSelect = false);

private:
		
	Kernel::LogFileInfo		m_LogInfo;			///<日志文件信息
	CMutexLock				m_Mutex;			///<互斥量
	unsigned int			m_LowerLevel;		///<记录日志最低级别
	bool					m_IsAnyncWrite;		///<是否异步写日志
	unsigned int			m_LogTimeFmt;		///<日志时间格式
	CMutexLock				m_PostLogMutex;		///<投递写日志事件
	bool					m_IsPostLog;		///<是否投递写日志
	string					m_AppName;			///<应用名称
	
}; // end by class CServerLog

//////////////////////////////////////////////////////////////////////////

/// @funtion 初始化日志服务
/// 
/// @param dir 日志文件目录
/// @param filename 日志文件名
/// @param flag ( _O_RDONLY | _O_WRONLY | _O_RDWR | _O_CREAT | _O_APPEND | _O_TRUNC )
/// @param mode 权限
extern bool _KERNEL_EXPORTS APIInitLog(const char* dir, const char* filename, int flag, int mode = 0644);

/// @function 写日志
///
/// @param fmt 格式化参数
/// @parma file 文件函数
/// @param line 行
extern bool _KERNEL_EXPORTS APIWriteLog(const char* file, int line, int level, const char* fmt, ...);

/// @function 写日志
///
/// @param fmt 格式化参数
/// @parma file 文件函数
/// @param line 行
extern bool _KERNEL_EXPORTS WriteLog(const char* file, int line, int level, const char* text);

/// @function 写日志
///
/// @param fmt 格式化参数
extern bool _KERNEL_EXPORTS WriteLogWin32(const char* fmt, ...);

//////////////////////////////////////////////////////////////////////////


} // end by namespace Kernel

#endif //__SERVER_LOG_H_