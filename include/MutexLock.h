#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__

#include <boost/noncopyable.hpp>
#include "Types.h"

#if (defined(__WIN_PLATFORM) && (_MSC_VER < 1500))
#else
#if _MSC_VER >= 1900
#include <atomic>
#else
#include <boost/atomic/atomic.hpp>
#endif
#endif

namespace Kernel
{
/// @Breif 互斥锁
class _KERNEL_EXPORTS CMutexLock : boost::noncopyable
{
public:
	CMutexLock()
	{
#ifdef __WIN_PLATFORM
		InitializeCriticalSection(&m_Mutex);
#else
		int ret = pthread_mutex_init(&m_Mutex, NULL);
		assert(ret == 0);
		(void) ret;
#endif
	}

	~CMutexLock()
	{
#ifdef __WIN_PLATFORM
		DeleteCriticalSection(&m_Mutex);
#else
		int ret = pthread_mutex_destroy(&m_Mutex);
		assert(ret == 0);
		(void) ret;
#endif
	}

	/// @function 加锁
	/// 
	/// @return 返回结果
	void Lock()
	{
#ifdef __WIN_PLATFORM
		EnterCriticalSection(&m_Mutex);
#else
		pthread_mutex_lock(&m_Mutex);
#endif
		
	}

	/// @function 解锁
	/// 
	/// @return 返回结果
	void Unlock()
	{
#ifdef __WIN_PLATFORM
		LeaveCriticalSection(&m_Mutex);
#else
		pthread_mutex_unlock(&m_Mutex);
#endif
	}

	/// @function 尝试加锁
	/// 
	/// @return 返回结果
	void TryLock()
	{
#ifndef __WIN_PLATFORM
		pthread_mutex_trylock(&m_Mutex);
#endif
	}

	/// @function 获取互斥变量
	/// 
	/// @return 返回互斥变量
#ifdef __WIN_PLATFORM
	CRITICAL_SECTION* GetMutex() { return &m_Mutex; }
#else
	pthread_mutex_t* GetMutex() { return &m_Mutex; }
#endif

private:
#ifdef __WIN_PLATFORM
	CRITICAL_SECTION 	m_Mutex;		///<互斥变量
#else
	pthread_mutex_t		m_Mutex;		///<互斥变量
#endif
}; // end by class CMutexLock

/// @Brief 自旋锁(原子锁)
class _KERNEL_EXPORTS CSpinlock : boost::noncopyable
{
public:
	CSpinlock()
	{
#if (defined(__WIN_PLATFORM) && (_MSC_VER < 1500))
#else
		m_State = Unlocked;
#endif
	}

	void Lock()
	{
#if (defined(__WIN_PLATFORM) && (_MSC_VER < 1500))
		m_Lock.Lock();
#else
#if _MSC_VER >= 1900
		while (m_State.exchange(Locked, std::memory_order_acquire) == Locked) { /* busy-wait */ }
#else
		while(m_State.exchange(Locked, boost::memory_order_acquire) == Locked) { /* busy-wait */ }
#endif
#endif
	}
	void Unlock()
	{
#if (defined(__WIN_PLATFORM) && (_MSC_VER < 1500))
		m_Lock.Unlock();
#else
#if _MSC_VER >= 1900
		m_State.store(Unlocked, std::memory_order_release);
#else
		m_State.store(Unlocked, boost::memory_order_release);
#endif
#endif	
	}


private:
#if (defined(__WIN_PLATFORM) && (_MSC_VER < 1500))
	CMutexLock		m_Lock;
#else
#if _MSC_VER >= 1900
	typedef enum { Locked, Unlocked } LockState;
	std::atomic<LockState>	m_State;
#else
	typedef enum { Locked, Unlocked } LockState;
	boost::atomic<LockState>	m_State;
#endif
#endif

}; // end by class CSpinlock

/// @Breif 读写锁,默认加写锁
class _KERNEL_EXPORTS CRWLock : boost::noncopyable
{
public:
	CRWLock()
	{
#ifdef __WIN_PLATFORM
#ifndef _WIN32_SERVER_
		InitializeSRWLock(&m_Mutex);
#endif //_WIN32_SERVER_
#else
		pthread_rwlock_init(&m_Mutex, NULL);
#endif //
	}

	~CRWLock()
	{
#ifdef __WIN_PLATFORM
#else
		pthread_rwlock_destroy(&m_Mutex);
#endif //
	}

	/// @function 加锁(写锁)
	/// 
	/// @return 返回结果
	void Lock()
	{
#ifdef __WIN_PLATFORM
#ifndef _WIN32_SERVER_
		AcquireSRWLockExclusive(&m_Mutex);
#endif //_WIN32_SERVER_
#else
		pthread_rwlock_wrlock(&m_Mutex);
#endif

	}

	/// @function 解锁(写锁)
	/// 
	/// @return 返回结果
	void Unlock()
	{
#ifdef __WIN_PLATFORM
#ifndef _WIN32_SERVER_
		ReleaseSRWLockExclusive(&m_Mutex);
#endif //_WIN32_SERVER_
#else
		pthread_rwlock_unlock(&m_Mutex);
#endif
	}

	/// @function 加锁(读锁)
	/// 
	/// @return 返回结果
	void LockR()
	{
#ifdef __WIN_PLATFORM
#ifndef _WIN32_SERVER_
		AcquireSRWLockShared(&m_Mutex);
#endif //_WIN32_SERVER_
#else
		pthread_rwlock_rdlock(&m_Mutex);
#endif

	}

	/// @function 解锁(读锁)
	/// 
	/// @return 返回结果
	void UnlockR()
	{
#ifdef __WIN_PLATFORM
#ifndef _WIN32_SERVER_
		ReleaseSRWLockShared(&m_Mutex);
#endif //_WIN32_SERVER_
#else
		pthread_rwlock_unlock(&m_Mutex);
#endif
	}

	/// @function 尝试加锁(加写锁)
	/// 
	void TryLock()
	{
#ifndef __WIN_PLATFORM
		pthread_rwlock_trywrlock(&m_Mutex);
#endif
	}

	/// @function 尝试加锁(加读锁)
	/// 
	void TryLockR()
	{
#ifndef __WIN_PLATFORM
		pthread_rwlock_tryrdlock(&m_Mutex);
#endif
	}

private:
#ifdef __WIN_PLATFORM
#ifndef _WIN32_SERVER_
	SRWLOCK			 	m_Mutex;		/// 读写锁
#endif //_WIN32_SERVER_
#else
	pthread_rwlock_t	m_Mutex;		/// 读写锁
#endif

};

/// @Brief 假锁
class _KERNEL_EXPORTS CFaskLock : boost::noncopyable
{
public:
	/// @function 加锁
	/// 
	void Lock() { }

	/// @function 解锁
	/// 
	void Unlock() { }

}; // end by class CFaskLock

/// @Brief自动守护锁
template <typename T>
class _KERNEL_EXPORTS CLockGuard : boost::noncopyable
{
public:
	explicit CLockGuard(T& mutex)
		:m_MutexLock(mutex)
	{
		m_MutexLock.Lock();
	}

	~CLockGuard()
	{
		m_MutexLock.Unlock();
	}

private:
	T&		m_MutexLock;

}; //end by class CLockGuard

/// @Brief自动守护锁(读写锁)
template <>
class _KERNEL_EXPORTS CLockGuard<CRWLock> : boost::noncopyable
{
public:
	explicit CLockGuard(CRWLock& mutex, bool rlock)
		:m_MutexLock(mutex)
		,m_ShareLock(rlock)
	{
		if(rlock)
			m_MutexLock.LockR();
		else
			m_MutexLock.Lock();
	}

	~CLockGuard()
	{
		if(m_ShareLock)
			m_MutexLock.UnlockR();
		else
			m_MutexLock.Unlock();
	}

private:
	CRWLock&	m_MutexLock;
	bool		m_ShareLock;

}; //end by class CLockGuard

typedef class CLockGuard<CSpinlock> CSpinLockGuard;		/// 自动守护自旋锁
typedef class CLockGuard<CMutexLock> CMutexLockGuard;	/// 自动守护互斥锁
typedef class CLockGuard<CFaskLock> CFaskLockGuard;		/// 自动守护假锁
typedef class CLockGuard<CRWLock> CRWLockGuard;			/// 自动守护读写锁

}	//end by namespace Kernel

#endif //__MUTEX_LOCK_H__
