#ifndef __THREAD_PACKAGE_H__
#define __THREAD_PACKAGE_H__

#include "Types.h"
#include "Buffer.h"
#include "Address.h"
#include "Define.h"

namespace Kernel
{
/// @Brief 线程包
class CThreadPackage
{
public:
	CThreadPackage(int type, bool ispool = true, unsigned int size = 0)
		:m_BufSize(size)
		,m_Pool(ispool)
		,m_ThreadEventType(type)
	{
		m_Len = 0;
		m_ID = 0;
		m_Exter = 0;
	}

	virtual ~CThreadPackage()
	{

	}

	/// @function 重置数据
	/// 
	virtual void Reset()
	{
		m_ThreadEventType = 0;
		m_Type = 0;
		m_Len = 0;
		m_ID = 0;
		m_Exter = 0;
	}

	/// @function 获取缓冲区大小
	/// 
	/// @return 返回缓冲区大小
	unsigned int GetBufSize() { return m_BufSize; }

	/// @function 是否池中分配的对象
	/// 
	bool IsPool() { return m_Pool; }

private:
	unsigned int		m_BufSize;			/// 事件缓冲大小
	bool				m_Pool;				/// 是否内存池中分配的

public:
	uint64_t			m_ThreadEventType;	/// 线程事件类型
	uint64_t			m_Type;				/// 逻辑事件类型
	unsigned short		m_Len;				/// 包体长度
	uint64_t			m_ID;				/// ID
	uint64_t			m_Exter;			/// 扩展字段

}; // end class CThreadPackage

/// @brief 带缓冲区的线程包
template <int type, unsigned int ThreadDataSize = THREAD_DATA_SIZE_2048>
class CThreadPackageBuf : public CThreadPackage
{
public:
	CThreadPackageBuf(unsigned size = ThreadDataSize, bool pool = true)
		:CThreadPackage(type, pool, size)
		,m_Buf(size)
	{

	}

	virtual ~CThreadPackageBuf()
	{

	}

	/// @function 重置数据
	/// 
	virtual void Reset()
	{
		m_Buf.Reset();
		m_AddrInfo.Reset();
		CThreadPackage::Reset();
	}

	/// @function 缓冲区
	/// 
	char* Buffer() { return m_Buf.GetBuffer(); }
	void* BufferVoid() { return m_Buf.GetBuffer(); }
	string GetString() { return m_Buf.GetBuffer(); }

public:
	CAddress		m_AddrInfo;				///<地址信息
	CBuffer			m_Buf;					///<缓冲区

}; // end class CThreadPackageBuf

typedef class CThreadPackageBuf<0, DATA_BUF_MAX>			CTEventBufPackage65536;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_32768>	CTEventBufPackage32768;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_16384>	CTEventBufPackage16384;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_8192>	CTEventBufPackage8192;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_4096>	CTEventBufPackage4096;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_2048>	CTEventBufPackage2048;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_1024>	CTEventBufPackage1024;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_512>	CTEventBufPackage512;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_256>	CTEventBufPackage256;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_128>	CTEventBufPackage128;
typedef class CThreadPackageBuf<0, THREAD_DATA_SIZE_64>		CTEventBufPackage64;
typedef class CThreadPackageBuf<0, 1>	CTEventPackage;

} // end Kernel


#endif //__THREAD_PACKAGE_H__