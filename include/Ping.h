#ifndef __PING_H__
#define __PING_H__

#include "Types.h"

#ifdef __WIN_PLATFORM
namespace Kernel
{
/// @brief ping й╣ож
class _KERNEL_EXPORTS CPing
{
public:
	CPing(void);
	~CPing(void);

	bool CanPing(const char* pDscAddr);
	void Ping(const char* pDscAddr, int nPackNum = 4, int nPackSize = 32);

private:
	bool Init();
	bool FInit();
	bool ConstructIcmpMessage(char* pDataBuff, int nDataSize);
	USHORT CheckSum(USHORT* pBuff, int nSize);
	bool DecodeIpPacket(char* pBuff, int nBuffSize, sockaddr_in* pAddr);
	bool PingHelper(const char* pDscAddr, int nPackNum = 4, int nPackSize = 32);

private:
	SOCKET m_socketRaw;
	bool m_bPrintInfo;

}; // end class CPing

} // end namespace Kernel

#endif //__WIN_PLATFORM
#endif //__PING_H__