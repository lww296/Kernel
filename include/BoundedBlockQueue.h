#ifndef __BOUNDED_BLOCK_QUEUE_H__
#define __BOUNDED_BLOCK_QUEUE_H__

#include <boost/circular_buffer.hpp>	// 环形缓冲

#include "MutexLock.h"
#include "Condition.h"

namespace Kernel
{
// 队列动作
enum QUEUE_ACTION
{
	QA_BLOCK = 0,		// 阻塞等待
	QA_OVERWRITE,		// 覆盖写
	QA_BREAK,			// 退出
};

/// @Brief 有界阻塞队列(T: 数据类型，DATAL: m_TaskQueue 锁类型, PutAction: 投递动作，TakeAction: 取动作, ConditionTimeout:条件等待超时时间(毫秒)(0:永不超时)
template <typename T, typename DATAL = CMutexLock, QUEUE_ACTION PutAction = QA_OVERWRITE, QUEUE_ACTION TakeAction = QA_BLOCK, int ConditionTimeout = 0>
class CBoundedBlockQueue 
{
public:
	/// @param max 队列最大任务数
	/// 
	explicit CBoundedBlockQueue(unsigned int max = 64)
		:m_TaskMax(max)
		,m_NotEmptyCond(m_Mutex)
		,m_NotFullCond(m_Mutex)
		,m_TaskQueue(m_TaskMax)
		, m_IsRunning(true)
	{
		
	}

	/// @function 投递任务
	/// 
	/// @param t 任务对象
	void Put(const T& t)
	{
		if (!m_IsRunning) return;

		CLockGuard<DATAL> lock(m_Mutex);
		while(m_TaskQueue.full())
		{
			if (PutAction == QA_BREAK)
			{
				return;
			}
			else if (PutAction == QA_OVERWRITE)
			{
				break;
			}
			else if (PutAction == QA_BLOCK)
			{
				// 等待
				bool timeout = false;
				if (ConditionTimeout == 0)
					m_NotFullCond.Wait();
				else
					timeout = m_NotFullCond.WaitTimeOut(ConditionTimeout);
				if (timeout)
					return;
			}

			if (!m_IsRunning)
				return;
		}

		m_TaskQueue.push_back(t);
		m_NotEmptyCond.Notify();
	}

	/// @function 取出任务
	/// 
	/// @return 返回任务对象
	void Take(T& task)
	{
		if (!m_IsRunning) return;

		CLockGuard<DATAL> lock(m_Mutex);
		while(m_TaskQueue.empty())
		{
			if (TakeAction == QA_BREAK)
			{
				return;
			}
			else if (TakeAction == QA_OVERWRITE)
			{
				break;
			}
			else if (TakeAction == QA_BLOCK)
			{
				// 等待
				bool timeout = false;
				if (ConditionTimeout == 0)
					m_NotEmptyCond.Wait();
				else
					timeout = m_NotEmptyCond.WaitTimeOut(ConditionTimeout);
				if (timeout)
					return;
			}

			if (!m_IsRunning)
				return;
		}

		if(!m_TaskQueue.empty())
		{
			task = m_TaskQueue.front();
			m_TaskQueue.pop_front();
			m_NotFullCond.Notify();
		}
	}

	/// @function 队列是否为空
	/// 
	/// @return 返回结果
	bool IsEmpty()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.empty();
	}

	/// @function 队列是否满
	/// 
	/// @return 返回结果
	bool IsFull()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.full();
	}

	/// @function 获取队列当前任务数量
	/// 
	/// @return 返回数量
	size_t Size()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.size();
	}

	/// @function 获取队列容量(总大小)
	/// 
	/// @return 返回队列容量
	size_t Capacity()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.capacity();
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
	void Clear()
	{
		Stop();
		CLockGuard<DATAL> lock(m_Mutex);
		m_TaskQueue.clear();
	}

private:
	unsigned int				m_TaskMax;			/// 最大任务数量
	CCondition					m_NotEmptyCond;		/// 非空条件变量
	CCondition					m_NotFullCond;		/// 非满条件变量
	DATAL						m_Mutex;			/// 互斥量
	boost::circular_buffer<T>	m_TaskQueue;		/// 任务队列
	bool						m_IsRunning;		/// 是否运行

}; // end by class CBoundedBlockQueue

#ifdef __USE_C11__
/// @Brief 有界阻塞队列(T: 数据类型，DATAL: m_TaskQueue 锁类型, PutAction: 投递动作，TakeAction: 取动作, ConditionTimeout:条件等待超时时间(毫秒)(0:永不超时)
template <typename T, typename DATAL = CMutexLock, QUEUE_ACTION PutAction = QA_OVERWRITE, QUEUE_ACTION TakeAction = QA_BLOCK, int ConditionTimeout = 0>
class CLockfreeBoundedBlockQueue 
{
public:
	/// @param max 队列最大任务数
	/// 
	explicit CLockfreeBoundedBlockQueue(unsigned int max = 64)
		:m_TaskMax(max)
		,m_NotEmptyCond(m_Mutex)
		,m_NotFullCond(m_Mutex)
		,m_TaskQueue(m_TaskMax)
		, m_IsRunning(true)
	{

	}

	/// @function 投递任务
	/// 
	/// @param t 任务对象
	void Put(const T& t)
	{
		if (!m_IsRunning) return;

		CLockGuard<DATAL> lock(m_Mutex);
		while(m_TaskQueue.full())
		{
			if (PutAction == QA_BREAK)
			{
				return;
			}
			else if (PutAction == QA_OVERWRITE)
			{
				break;
			}
			else if (PutAction == QA_BLOCK)
			{
				// 等待
				bool timeout = false;
				if (ConditionTimeout == 0)
					m_NotFullCond.Wait();
				else
					timeout = m_NotFullCond.WaitTimeOut(ConditionTimeout);
				if (timeout)
					return;
			}

			if (!m_IsRunning)
				return;
		}

		m_TaskQueue.push_back(t);
		m_NotEmptyCond.Notify();
	}

	/// @function 取出任务
	/// 
	/// @return 返回任务对象
	void Take(T& task)
	{
		if (!m_IsRunning) return;

		CLockGuard<DATAL> lock(m_Mutex);
		while(m_TaskQueue.empty())
		{
			if (TakeAction == QA_BREAK)
			{
				return;
			}
			else if (TakeAction == QA_OVERWRITE)
			{
				break;
			}
			else if (TakeAction == QA_BLOCK)
			{
				// 等待
				bool timeout = false;
				if (ConditionTimeout == 0)
					m_NotEmptyCond.Wait();
				else
					timeout = m_NotEmptyCond.WaitTimeOut(ConditionTimeout);
				if (timeout)
					return;
			}

			if (!m_IsRunning)
				return;
		}

		if(!m_TaskQueue.empty())
		{
			task = m_TaskQueue.front();
			m_TaskQueue.pop_front();
			m_NotFullCond.Notify();
		}
	}

	/// @function 队列是否为空
	/// 
	/// @return 返回结果
	bool IsEmpty()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.empty();
	}

	/// @function 队列是否满
	/// 
	/// @return 返回结果
	bool IsFull()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.full();
	}

	/// @function 获取队列当前任务数量
	/// 
	/// @return 返回数量
	size_t Size()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.size();
	}

	/// @function 获取队列容量(总大小)
	/// 
	/// @return 返回队列容量
	size_t Capacity()
	{
		CLockGuard<DATAL> lock(m_Mutex);
		return m_TaskQueue.capacity();
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
	void Clear()
	{
		Stop();
		CLockGuard<DATAL> lock(m_Mutex);
		m_TaskQueue.clear();
	}

private:
	unsigned int				m_TaskMax;			/// 最大任务数量
	CCondition					m_NotEmptyCond;		/// 非空条件变量
	CCondition					m_NotFullCond;		/// 非满条件变量
	DATAL						m_Mutex;			/// 互斥量
	boost::circular_buffer<T>	m_TaskQueue;		/// 任务队列
	bool						m_IsRunning;		/// 是否运行

}; // end by class CBoundedBlockQueue

#endif //__USE_C11__

} // end by namespace Kernel

#endif //__BOUNDED_BLOCK_QUEUE_H__