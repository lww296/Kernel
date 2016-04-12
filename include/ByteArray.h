#ifndef __BYTE_ARRAY_H__
#define __BYTE_ARRAY_H__

#include "Types.h"
#include "ServerLog.h"

using namespace Kernel;

namespace Kernel
{
/// @brief 字节数组
class _KERNEL_EXPORTS CByteArray
{
public:
	CByteArray(unsigned int size = 1024)
	{
		m_Buffer = new char[size];
		assert(m_Buffer);
		m_Size = size;
		m_Length = 0;
		m_Out = false;
	}

	CByteArray(char* buf, unsigned int size)
	{
		m_Buffer = buf;
		assert(m_Buffer);
		m_Size = size;
		m_Length = 0;
		m_Out = true;
	}

	~CByteArray()
	{
		Free();
	}

	void Reset()
	{
		memset(m_Buffer, 0, m_Size);
		m_Length = 0;
	}

	void Free()
	{
		m_Size = 0;
		m_Length = 0;
		if(!m_Out) _SAFE_DELETE_ARRAY_(m_Buffer);
	}

	unsigned char* GetByte() { return (unsigned char*)m_Buffer; }
	unsigned char GetByteVal() { return *(unsigned char*)m_Buffer; }
	bool GetBool() { return (*(unsigned char*)m_Buffer) == 1 ? true : false; }
	void SetByte(unsigned char val) { *(unsigned char*)m_Buffer = val; m_Length = sizeof(unsigned char); }
	char* GetChar() { return m_Buffer; }
	void SetCharArray(const char* val, unsigned int len) { int cpLen = 0; cpLen = len > m_Size ? m_Size : len; memcpy(m_Buffer, val, cpLen); m_Length = cpLen; }
	int GetInt() { return atoi(m_Buffer); }
	void SetInt(int val) { *(int*)m_Buffer = val; m_Length = sizeof(int); }
	long GetLong() { return atol(m_Buffer); }
	void SetLong(long val) { *(long*)m_Buffer = val;  m_Length = sizeof(long); }
	long long GetLonglong() { return atoll(m_Buffer); }
	void SetLonglong(long long val) { *(long long*)m_Buffer = val;  m_Length = sizeof(int64_t); }
	string GetLLString() { char strSessionID[32] = { 0 }; snprintf(strSessionID, sizeof(strSessionID), "%lld", atoll(m_Buffer)); return strSessionID; }
	float GetFloat() { return (float)atof(m_Buffer); }
	void SetFloat(float val) { *(float*)m_Buffer = val; m_Length = sizeof(float); }
	double GetDouble() { return atof(m_Buffer); }
	void SetDouble(double val) { *(double*)m_Buffer = val; m_Length = sizeof(double); }
	string GetString() { return m_Buffer; }
	void SetString(string val) { memcpy(m_Buffer, val.c_str(), val.length()); m_Length = val.length(); }

	unsigned int GetSize() { return m_Size; }
	void SetLength(unsigned int len) { m_Length = len; }
	unsigned int GetLength() { return m_Length; }
	string ToString()
	{
		string str(m_Buffer, m_Length);
		str.reserve(m_Length + 1);
		str.push_back('\0');
		return str;
	}

private:
	unsigned int	m_Size;		// 数组大小
	unsigned int	m_Length;	// 数据长度
	char*			m_Buffer;
	bool			m_Out;		// 是否外部buf

};

} // end namespace Kernel

// class CByteArrayManager
// {
// public:
// 	CByteArrayManager();
// 	~CByteArrayManager();
// 
// 	CByteObject* Alloc(unsigned int size);
// 
// 	void Free(CByteObject* obj);
// 
// };

#endif //__BYTE_ARRAY_H__
