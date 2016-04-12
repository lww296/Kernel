#ifndef __BUFFER_NET_H__
#define __BUFFER_NET_H__

#include <vector>
#include <assert.h>
#include <string.h>
#include <iostream>

#include "Define.h"
#include "MutexLock.h"
#include "Types.h"

namespace Kernel
{
/// @Brief 网络缓冲
class CBuffer
{
public:
	static const size_t kInitialSize = 0xffff;

public:
	CBuffer(size_t size = kInitialSize)
		:m_Buffer(NULL)
	{
		m_BufferSize = static_cast<int>(size);//设置缓冲区大小
		m_Offset = 0;//设置数据偏移值[数据大小]为0
		m_Buffer = new char[size];
		assert(m_Buffer);
		memset(m_Buffer, 0, m_BufferSize);
		m_Out = false;
	}

	CBuffer(char* addr, size_t size)
		:m_Buffer(addr)
	{
		m_BufferSize = static_cast<int>(size);//设置缓冲区大小
		m_Offset = 0;//设置数据偏移值[数据大小]为0
		assert(m_Buffer);
		memset(m_Buffer, 0, m_BufferSize);
		m_Out = true;
	}

	~CBuffer()
	{
		m_BufferSize = 0;
		m_Offset = 0;
		if(!m_Out) _SAFE_DELETE_ARRAY_(m_Buffer);
	}

	/// @functon 获取缓冲区数据大小
	/// 
	/// @return 返回大小
	int GetDataSize()
	{
		CLockGuard<CSpinlock> lock(m_Mutex); 
		return m_Offset;
	}

	/// @functon 获取缓冲区大小
	/// 
	/// @return 返回大小
	int GetBufferSize()
	{
		CLockGuard<CSpinlock> lock(m_Mutex); 
		return m_BufferSize;
	}

	/// @functon 调整缓冲区的大小
	/// 
	/// @返回结果
	//bool ChangeBufferSize(int);

	/// @function 缓冲数据是否适合包头大小
	/// 
	/// @return 返回结果
	bool IsPacketHeadSize()
	{
		CLockGuard<CSpinlock> lock(m_Mutex);
		return static_cast<int>(sizeof(NetPacketHead)) <= m_Offset;
	}

	/// @function 缓冲区是否拥有完整的包数据[包含包头和包体]
	/// 
	/// @param order 是否要进行字节序转换
	/// @return 返回结果
	bool IsFullPacket()
	{
		CLockGuard<CSpinlock> lock(m_Mutex);

		//如果连包头大小都不满足则返回
		if (!(static_cast<int>(sizeof(NetPacketHead)) <= m_Offset))
		     return false;

		return HAS_FULL_PACKET;
	}

	/// @function 添加消息到缓冲区
	///
	/// @param buf 数据
	/// @param len 长度
	/// @param type 写入方式(首部写入: 0, 末尾写入: 1)
	/// @return 返回结果
	bool Push(const char *buf,int len, int type = 1)
	{
		if(buf == NULL || len == 0)
			return false;

		try
		{
			CLockGuard<CSpinlock> lock(m_Mutex);

			//检查缓冲区长度是否满足,不满足则重新调整缓冲区大小
			if (m_Offset + len > m_BufferSize)
				return false;

			//拷贝新数据到缓冲区末尾
			if(type == 0)
			{
				memmove(m_Buffer, m_Buffer + len, m_Offset);
				memcpy(m_Buffer, buf, len);
			}
			else
			{
				memcpy(m_Buffer + sizeof(char) * m_Offset, buf, len);
			}
			m_Offset += len;//修改数据偏移

			return true;
		}
		catch(...)
		{
			assert(false);
			return false;
		}
	}

	/// @function 添加消息到缓冲区(面向发送数据)
	///
	/// @param head 包头数据
	/// @param buf 数据
	/// @param len buf长度, 不包括head的长度
	/// @return 返回结果
	bool Push(NetPacketHead& head, const void* buf, int len)
	{
		try
		{
			CLockGuard<CSpinlock> lock(m_Mutex);

			//检查缓冲区长度是否满足,不满足则重新调整缓冲区大小
			if ((m_Offset + len + (int)sizeof(NetPacketHead)) > m_BufferSize)
				return false;

			//拷贝新数据到缓冲区末尾
			memcpy(m_Buffer + sizeof(char) * m_Offset, &head, sizeof(NetPacketHead));
			memcpy(m_Buffer + sizeof(char) * m_Offset + sizeof(NetPacketHead), buf, len);
			m_Offset += (len + sizeof(NetPacketHead));//修改数据偏移

			return true;
		}
		catch(...)
		{
			assert(false);
			return false;
		}
	}

	/// @function 得到缓冲区内容
	///
	/// @return 返回缓冲
	const char *GetBufferContents()
	{
		CLockGuard<CSpinlock> lock(m_Mutex);

		return m_Buffer;
	}

	/// @function 得到缓冲区内容
	///
	/// @return 返回缓冲
	char *GetBuffer()
	{
		CLockGuard<CSpinlock> lock(m_Mutex);
		return m_Buffer;
	}

	/// @function 缓冲区复位
	///
	/// @note 清空缓冲区数据,但并未释放缓冲区
	void Reset()
	{
		CLockGuard<CSpinlock> lock(m_Mutex);

		if (m_Offset>0)
		{
			m_Offset = 0;
			memset(m_Buffer, 0, m_BufferSize);
		}
	}

	/// @function 获取并移除缓冲区首部的第一个数据包
	/// 
	/// @param outBuf 输出缓冲
	/// @param size 输出缓冲大小
	/// @param isSave 是否保存缓存首包
	void Poll(void *outBuf, unsigned short size, bool isSave = true)
	{
		try
		{
			CLockGuard<CSpinlock> lock(m_Mutex);

			if(m_Offset == 0)
				return;

			if ((static_cast<int>(sizeof(NetPacketHead)) <= m_Offset) && HAS_FULL_PACKET)
			{
				unsigned short len = FIRST_PACKET_LEN;
				if(isSave)
				{
					// 获取首包
					assert(outBuf);
					unsigned short copyLen = (len > size ? size : len);
					memcpy(outBuf, m_Buffer + 0, copyLen);
				}

				// 移除首包
				memmove(m_Buffer, m_Buffer + len * sizeof(char), m_Offset - len);
				m_Offset -= len;
				assert(m_Offset >= 0);
			}
		}
		catch(...)
		{
			assert(false);
			return;
		}
	}

	/// @function 获取并移除缓冲区指定长度数据包
	/// 
	/// @param outBuf 输出缓冲
	/// @param size 输出缓冲大小
	/// @param moveSize 移除缓冲数据大小
	/// @param isSave 是否保存数据包
	/// @return 返回实际获取或移除的大小
	int Poll(void *outBuf, unsigned short size, unsigned short moveSize, bool isSave = true)
	{
		try
		{
			CLockGuard<CSpinlock> lock(m_Mutex);

			if(m_Offset == 0)
				return 0;

			// 调整大小
			int len = 0;
			if(isSave) len = (size >= moveSize ? moveSize : size);
			else len = moveSize;
			len = (len >= m_Offset ? m_Offset : len);

			if(isSave)
			{
				// 获取包
				assert(outBuf);
				memcpy(outBuf, m_Buffer, len);
			}

			if(m_Offset - len == 0)
			{
				m_Offset = 0;
				memset(m_Buffer, 0, m_BufferSize);
			}
			else
			{
				// 移除包
				memmove(m_Buffer, m_Buffer + len * sizeof(char), m_Offset - len);
				m_Offset -= len;
			}

			assert(m_Offset >= 0);
			return len;
		}
		catch(...)
		{
			assert(false);
			return 0;
		}
	}

	/// @function 调整缓冲区
	/// @note 以版本号和包类型作为调整标识
	///
	/// @param version 版本
	/// @param type 类型
	void AdjustBuf(unsigned char version = 1, unsigned char type = 1)
	{
		CLockGuard<CSpinlock> lock(m_Mutex);

		if(m_BufferSize < 2)
			return;

		while((m_Buffer[0] != version || m_Buffer[1] != type) && m_Offset > 0)
		{
			// 偏移
			memcpy(m_Buffer, m_Buffer + sizeof(char), m_Offset - sizeof(char));
			m_Offset -= sizeof(char);
		}
	}

	/// @function 输出缓冲
	/// 
	void Print()
	{
		for(int i = 0; i < m_Offset;)
		{
			char buf[4096] = {};
			int num = m_Offset - i > (int)sizeof(buf) ? sizeof(buf) : (m_Offset - i);

			memcpy(buf, m_Buffer + i, num);
			std::cout << buf << std::endl;
			i += num;
		}
	}

private:
	char*			m_Buffer;					///<缓冲
	int				m_BufferSize;				///<缓冲区总大小
	int				m_Offset;					///<缓冲区数据大小
	bool			m_Out;						///<是否外部buf
	CSpinlock		m_Mutex;					///<互斥体

}; // end by class Kernel

} // end by namespace Kernel

#endif  // __BUFFER_NET_H__
