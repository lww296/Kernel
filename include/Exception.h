#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>

#include "Types.h"

namespace Kernel
{
/// @Brief 异常
class _KERNEL_EXPORTS CException : std::exception
{
public:
	explicit CException(const char* msg);
	explicit CException(const string& msg);
	virtual ~CException() throw();
	virtual const char* What() const throw();
	const char* StackTrace() const throw();

private:
	void GetStackTrace();

	string		m_Message;		///<异常消息
	string		m_Stack;		///<栈信息

}; // end by class CExcepion

} // end by namespace Kernel

#endif //__EXCEPTION_H__