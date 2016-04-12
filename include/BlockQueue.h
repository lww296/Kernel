#ifndef __BLOCK_QUEUE_H__
#define __BLOCK_QUEUE_H__

#include <deque>

#include "Types.h"
#include "MutexLock.h"
#include "Condition.h"

#ifdef __USE_C11__
#include <boost/lockfree/queue.hpp>
#include <boost/function.hpp>
#else
#define CLockfreeBlockQueue	CBlockQueue
#endif //__USE_C11__

namespace Kernel
{
/// @无界队列(T: 数据类型，DATAL: m_TaskQueue 锁类型, IsNonblock: 是否非阻塞, ConditionTimeout:条件等待超时时间(毫秒)(0:永不超时)
template <typename T, typename DATAL = CMutexLock, bool IsNonblock = false, int ConditionTimeout = 0>
class CBlockQueue
{
public:
	CBlockQueue(int waittime = ConditionTimeout)
		:m_TaskMutex()
		,m_NullMutex()
		,m_PutMutex()
		,m_TakeMutex()
		,m_NotEmptyCond(m_NullMutex)
		,m_TaskQueue()
		,m_IsRunning(true)
		,m_WaitTime(waittime)
	{

	}

	/// @function 投递任务
	/// 
	/// @param t 任务对象
	void Put(const T& t)
	{
		if(!m_IsRunning) return;

		CLockGuard<DATAL> lock(m_PutMutex);
		if(m_IsRunning)
		{
			CLockGuard<DATAL> tasklock(m_TaskMutex);
			m_TaskQueue.push_back(t);
			m_NotEmptyCond.NotifyAll();
		}
	}

	/// @function 取出任务
	/// 
	/// @return 返回任务对象
	void Take(T& task)
	{
		if(!m_IsRunning) return;

		CLockGuard<DATAL> lock(m_TakeMutex);
		while(Size() == 0 && m_IsRunning)
		{
			if(IsNonblock)
				return;

			// 等待
			bool timeout = false;
			if(m_WaitTime == 0)
				m_NotEmptyCond.Wait();	
			else
				timeout = m_NotEmptyCond.WaitTimeOut(m_WaitTime);

			if(!m_IsRunning)
				return;

			CLockGuard<DATAL> tasklock(m_TaskMutex);
			if(!m_TaskQueue.empty() && m_IsRunning)
			{
				task = m_TaskQueue.front();
				m_TaskQueue.pop_front();
				return;
			}
			
			if(timeout)
				return;
		}

		if(m_IsRunning)
		{
			CLockGuard<DATAL> tasklock(m_TaskMutex);
			if(!m_TaskQueue.empty() && m_IsRunning)
			{
				task = m_TaskQueue.front();
				m_TaskQueue.pop_front();
			}
		}	
	}

	/// @function 设置条件等待超时时间
	/// 
	/// @parma timeout 超时时间
	void SetTimeout(int timeout) { m_WaitTime = timeout; }

	/// @function 获取队列当前任务数量
	/// 
	/// @return 返回数量
	size_t Size()
	{
		CLockGuard<DATAL> lock(m_TaskMutex);
		return m_TaskQueue.size();
	}

	/// 重新运行
	/// 
	void Restart()
	{
		m_IsRunning = true;
	}

	/// @function 停止
	///
	void Stop()
	{
		m_IsRunning = false;
		m_NotEmptyCond.NotifyAll((int)Size());
	}

	/// @function 清空
	/// 
	/// @param stop 清空的时候是否停止服务
	void Clear(bool stop = true)
	{
		if(stop)
			Stop();
		else
			m_NotEmptyCond.NotifyAll((int)Size());

		CLockGuard<DATAL> lock(m_TaskMutex);
		m_TaskQueue.clear();
	}

private:
	DATAL				m_TaskMutex;		/// 任务互斥体
	DATAL				m_NullMutex;		/// 条件等待锁
	DATAL				m_PutMutex;			/// 生产者互斥量
	DATAL				m_TakeMutex;		/// 消费者互斥量
	CCondition			m_NotEmptyCond;		/// 非空条件变量
	std::deque<T>		m_TaskQueue;		/// 任务队列
	bool				m_IsRunning;		/// 是否运行
	int					m_WaitTime;			/// 条件等待时间

}; // end by class CBlockQueue

#ifdef __USE_C11__
/// @无界队列(T: 数据类型，DATAL: m_TaskQueue 锁类型, IsNonblock: 是否非阻塞, ConditionTimeout:条件等待超时时间(毫秒)(0:永不超时)
template <typename T, typename DATAL = CMutexLock, bool IsNonblock = false, int ConditionTimeout = 0>
class CLockfreeBlockQueue
{
public:
	CLockfreeBlockQueue(int waittime = ConditionTimeout)
		:m_NullMutex()
		,m_NotEmptyCond(m_NullMutex)
		,m_TaskQueue(50)
		,m_IsRunning(true)
		,m_Count(0)
		,m_WaitTime(waittime)
	{

	}

	/// @function 投递任务
	/// 
	/// @param t 任务对象
	void Put(const T& t)
	{
		if(!m_IsRunning)
			return;

		while(!m_TaskQueue.push(t));
		++m_Count;
		m_NotEmptyCond.NotifyAll();
	}

	/// @function 取出任务
	/// 
	/// @return 返回任务对象
	void Take(T& task)
	{
		while(m_IsRunning)
		{
			if(m_TaskQueue.empty())
			{
				if(IsNonblock)
					break;

				// 等待
				bool timeout = false;
				if(m_WaitTime == 0)
					m_NotEmptyCond.Wait();	
				else
					timeout = m_NotEmptyCond.WaitTimeOut(m_WaitTime);

				if(!m_IsRunning || timeout)
					break;
			}

			if(m_TaskQueue.pop(task))
			{
				--m_Count;
				break;
			}
		}	
	}

	/// @function 设置条件等待超时时间
	/// 
	/// @parma timeout 超时时间
	void SetTimeout(int timeout) { m_WaitTime = timeout; }

	/// @function 获取队列当前任务数量
	/// 
	/// @return 返回数量
	size_t Size()
	{
		return m_Count;
	}

	/// 重新运行
	/// 
	void Restart()
	{
		m_IsRunning = true;
	}

	/// @function 停止
	///
	void Stop()
	{
		m_IsRunning = false;
		m_NotEmptyCond.NotifyAll((int)Size());
	}

	/// @function 清空
	/// 
	/// @param stop 清空的时候是否停止服务
	void Clear(bool stop = true)
	{
		if(stop)
			Stop();
		else
			m_NotEmptyCond.NotifyAll((int)Size());

		boost::function<void(T&)> func = [this](T& /*task*/) mutable -> void {
			--m_Count;
		};
		m_TaskQueue.consume_all(func);
		m_Count = 0;
	}

private:
	DATAL				m_NullMutex;		/// 条件等待锁
	CCondition			m_NotEmptyCond;		/// 非空条件变量
	boost::atomic_bool	m_IsRunning;		/// 是否运行
	boost::atomic_int	m_Count;			/// 队列任务数量
	boost::atomic_uint	m_WaitTime;			/// 条件等待时间
	boost::lockfree::queue<T, boost::lockfree::fixed_sized<false> > m_TaskQueue;	/// 任务队列

}; // end by class CLockfreeBlockQueue

#endif //__USE_C11__

} // end by namespace Kernel

#endif //__BLOCK_QUEUE_H__