#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <boost/noncopyable.hpp>

#include "Types.h"
#include "MutexLock.h"

namespace Kernel
{
/// @Brief 条件变量
class CCondition : boost::noncopyable
{
public:
	explicit CCondition(CMutexLock& mutex)
		:m_Mutex(mutex)
	{
#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_
		m_Condition = CreateEvent(NULL, FALSE, FALSE, NULL); 
#else
		InitializeConditionVariable(&m_Condition);
#endif //_WIN32_SERVER_
#else
		pthread_cond_init(&m_Condition, NULL);
#endif
	}

	~CCondition()
	{
#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_
		CloseHandle(m_Condition);   
#endif //_WIN32_SERVER_
#else
		pthread_cond_destroy(&m_Condition);
#endif
	}

	/// @function 条件等待
	/// 
	void Wait()
	{
#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_
		WaitForSingleObject(m_Condition, INFINITE);
#else
		SleepConditionVariableCS(&m_Condition, m_Mutex.GetMutex(), INFINITE);
#endif //_WIN32_SERVER_
#else
		pthread_cond_wait(&m_Condition, m_Mutex.GetMutex());
#endif
	}

	/// @function 通知
	/// 
	void Notify()
	{
#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_ 
		SetEvent(m_Condition);
#else
		WakeConditionVariable(&m_Condition);
#endif //_WIN32_SERVER_
#else
		pthread_cond_signal(&m_Condition);
#endif
	}

	/// @function 通知所有
	/// 
	/// @param num
	void NotifyAll(int num = 1)
	{
#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_
		num = num == 0 ? 1 : num;
		for(int i = 0; i < num; ++i)
			SetEvent(m_Condition);
#else
		WakeAllConditionVariable(&m_Condition);
#endif //_WIN32_SERVICE_
#else
		pthread_cond_broadcast(&m_Condition);
#endif
	}

	/// @function 超时等待
	/// 
	/// @seconds 等待毫秒
	/// @return 超时返回 true
	bool WaitTimeOut(int ms)
	{
#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_
		return WaitForSingleObject(m_Condition, ms) == WAIT_TIMEOUT;
#else
		return !SleepConditionVariableCS(&m_Condition, m_Mutex.GetMutex(), ms);
#endif //_WIN32_SERVER_
#else
		struct timespec abstime;
#ifdef __LINUX_PLATFORM
		clock_gettime(CLOCK_REALTIME, &abstime);
#else
        clock_gettime(0, &abstime);
#endif //__LINUX_PLATFORM
		abstime.tv_sec += (ms / 1000);
		abstime.tv_nsec += (ms % 1000 * 1000 * 1000);
		return ETIMEDOUT == pthread_cond_timedwait(&m_Condition, m_Mutex.GetMutex(), &abstime);
#endif
	}
    
#ifdef __MACOS_PLATFORM
    int clock_gettime(int /*clk_id*/, struct timespec* t)
    {
        struct timeval now;
        int rv = gettimeofday(&now, NULL);
        if (rv) return rv;
        t->tv_sec = now.tv_sec;
        t->tv_nsec = now.tv_usec * 1000;
        return 0;
    }
#endif // __MACOS_PLATFORM

private:
	CMutexLock&			m_Mutex;			///<互斥量

#ifdef __WIN_PLATFORM
#ifdef _WIN32_SERVER_
	HANDLE				m_Condition;		///<非空句柄
#else
	CONDITION_VARIABLE	m_Condition;		///<条件变量
#endif //_WIN32_SERVICE_
#else
	pthread_cond_t		m_Condition;		///<条件变量	
#endif

}; // end by class CCondition

} // end by namespace Kernel

#endif //__CONDITION_H__