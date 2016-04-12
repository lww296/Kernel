#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__

#include <deque>
#include <list>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include "MutexLock.h"

const static int poolChunkSize = 50;

namespace Kernel
{
/// @Brief 对象池
template <typename T, typename LOCKT = CFaskLock>
class CObjectPool : boost::noncopyable
{
public:
	CObjectPool(int chunk = poolChunkSize, bool autoAlloc = true, bool autoCheckDel = false)
		:m_AutoAlloc(autoAlloc)
		,m_AutoCheckDel(autoCheckDel)
	{
		// 调整块大小
		if(chunk > 1000)
			m_ChunkSize = 1000;
		else if(chunk <= 0)
			m_ChunkSize = 0;
		else
			m_ChunkSize = chunk;

		if(m_ChunkSize == 0)
			return;

		for(size_t i = 0; i < m_ChunkSize; ++i)
		{
			T* newObj = new T();
			if(newObj)
			{
				m_AllList.push_back(newObj);
				m_IdleQueue.push_back(newObj);
			}
		}
	}

	~CObjectPool()
	{
		Clear();
	}

	/// @function 申请分配对象
	/// 
	/// @return t 返回对象
	T* Alloc()
	{
		CLockGuard<LOCKT>	lock(m_Lock);

		// 是否自动创建分配
		if(m_AutoAlloc)
		{
			if(m_ChunkSize == 0)
				return NULL;

			try
			{
				// 分配检测
				if(m_IdleQueue.size() / 10 == 0)
				{
					for(size_t i = 0; i < m_ChunkSize; ++i)
					{
						T* newObj = new T();
						if(newObj)
						{
							m_AllList.push_back(newObj);
							m_IdleQueue.push_back(newObj);
						}
					}
				}
			}
			catch(...)
			{
				
			}
		}

		if(m_IdleQueue.size() > 0)
		{
			T* t = m_IdleQueue.front();
			m_IdleQueue.pop_front();
			return t;
		}
		return NULL;
	}

	/// @function 申请释放对象
	/// 
	/// @param obj 对象
	void Free(T* obj)
	{
		assert(obj);
		if(NULL == obj)
			return;

		{
			CLockGuard<LOCKT>	lock(m_Lock);
			m_IdleQueue.push_back(obj);
		}

		if(m_AutoCheckDel)
			Check();
	}

	/// @function 清空
	/// 
	void Clear()
	{
		try
		{
			CLockGuard<LOCKT>	lock(m_Lock);
			typename std::list<T*>::iterator it = m_AllList.begin();
			typename std::list<T*>::iterator tmp;
			for( ; it != m_AllList.end(); )
			{
				tmp = it;
				++tmp;

				T* ptr = (*it);
				m_AllList.erase(it);
				_SAFE_DELETE_(ptr);

				it = tmp;
			}

			m_IdleQueue.clear();
			m_AllList.clear();	
		}
		catch(...)
		{
			m_IdleQueue.clear();
			m_AllList.clear();	
		}
	}

	/// @function 检测资源回收
	/// @note 根据空闲队列回收
	/// 
	/// @return 返回是否有回收操作
	bool Check()
	{
		CLockGuard<LOCKT>	lock(m_Lock);
		if(m_ChunkSize == 0)
			return true;

		try
		{
			// 空闲队列大小是分配块大小的2倍以上就释放一块
			size_t num = m_IdleQueue.size() / m_ChunkSize;
			if(num > 2)
			{
				size_t size = m_ChunkSize * (num - 2);
				for(size_t i = 0; i < size; ++i)
				{
					T* ptr = m_IdleQueue.front();
					m_IdleQueue.pop_front();

					if(!ptr) continue;

					typename std::list<T*>::iterator it = m_AllList.begin();
					typename std::list<T*>::iterator tmp;
					for(; it != m_AllList.end(); )
					{
						tmp = it;
						++tmp;

						if(ptr == (*it))
						{
							m_AllList.erase(it);
							_SAFE_DELETE_(ptr);
							break;
						}

						it = tmp;
					}
				}

				return true;
			}
		}
		catch(...)
		{
			m_IdleQueue.clear();
			m_AllList.clear();
		}

		return false;
	}

	void CheckAll()
	{
		CLockGuard<LOCKT>	lock(m_Lock);
		if(m_ChunkSize == 0)
			return;

		try
		{
			// 空闲队列大小是分配块大小的2倍以上就释放一块
			size_t num = m_AllList.size() / m_ChunkSize;
			if(num > 1)
			{
				m_IdleQueue.clear();
				size_t size = m_ChunkSize * (num - 1);
				typename std::list<T*>::iterator it = m_AllList.begin();
				typename std::list<T*>::iterator tmp;
				for(unsigned int i = 0; it != m_AllList.end(); ++i)
				{
					tmp = it;
					++tmp;
					T* ptr = (*it);

					if(i >= size)
					{
						m_IdleQueue.push_back(ptr);
					}
					else
					{
						m_AllList.erase(it);
						_SAFE_DELETE_(ptr);
					}

					it = tmp;
				}
			}
		}
		catch(...)
		{
			m_IdleQueue.clear();
			m_AllList.clear();
		}
	}

	/// @function 获取池总数量
	/// 
	/// @return 返回大小
	size_t GetAllSize()
	{ 
		CLockGuard<LOCKT>	lock(m_Lock);
		return m_AllList.size(); 
	}

	/// @function 获取空闲队列大小
	/// 
	/// @return 返回大小
	size_t GetIdleSize()
	{ 
		CLockGuard<LOCKT>	lock(m_Lock);
		return m_IdleQueue.size();
	}

private:
	std::list<T*>		m_AllList;			///<所有对象
	std::deque<T*>		m_IdleQueue;		///<空闲队列
	unsigned int		m_ChunkSize;		///<块大小
	LOCKT				m_Lock;				///<锁对象
	bool				m_AutoAlloc;		///<无对象时自动创建
	bool				m_AutoCheckDel;		///<释放对象后是否自动检测删除

}; // end by class CObjectPool


/// @Brief 活动对象池列表
template <typename TKEY, typename TVAL, typename MAP_LOCK = CFaskLock, typename POOL_LOCK = CFaskLock>
class CObjectPoolMap
{
private:
	typedef boost::unordered_map<TKEY, TVAL*>	ActivePoolMap;

public:
	typedef typename ActivePoolMap::iterator MapIt;
	typedef typename ActivePoolMap::const_iterator MapCit;

public:
	CObjectPoolMap(int chunk = poolChunkSize, bool autoAlloc = true, bool autoCheckDel = false)
		:m_FreePool(chunk, autoAlloc, autoCheckDel)
	{

	}

	~CObjectPoolMap()
	{
		Clear();
	}

	/// @function 获取活动对象数量
	/// 
	/// @return 返回数量
	size_t GetActiveNum()
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		return m_ActiveMap.size();
	}

	/// @function 获取一个活动对象
	/// 
	/// @param key 活动对象key
	/// @return 返回对象
	TVAL* FindActiveObj(TKEY key)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.find(key);
 		if(it == m_ActiveMap.end())
			return NULL;

		return it->second;
	}

	/// @function 投递对象到活动列表
	/// 
	bool InsertActiveObj(TKEY key, TVAL* val)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.find(key);
 		if(it != m_ActiveMap.end())
		{
			assert(false);
			return false;
		}
		m_ActiveMap.insert(std::make_pair(key, val));
		return true;
	}

	/// @function 移除活动对象
	/// 
	/// @note 只移除不释放
	void RemoveActiveObj(TKEY key)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		m_ActiveMap.erase(key);
	}

	/// @function 删除活动对象
	/// 
	/// @note 会释放该对象
	void DelActiveObj(TKEY key)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.find(key);
		if(it != m_ActiveMap.end())
		{
			TVAL* obj = it->second;
			m_ActiveMap.erase(it);
			PostFreeObj(obj);
		}
	}

	/// @function 获取一个空闲对象
	/// 
	/// @return 返回空闲对象
	TVAL* GetFreeObj()
	{
		CLockGuard<MAP_LOCK> lock(m_FreeObjLock);
		return m_FreePool.Alloc();
	}

	/// @function 投递对象到空闲池
	/// 
	void PostFreeObj(TVAL* obj)
	{
		CLockGuard<MAP_LOCK> lock(m_FreeObjLock);
		m_FreePool.Free(obj);
	}

	MapIt Begin()
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		return m_ActiveMap.begin();
	}

	MapCit Begin() const
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		return m_ActiveMap.begin();
	}

	MapIt End()
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		return m_ActiveMap.end();
	}

	MapCit End() const
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		return m_ActiveMap.end();
	}

	/// @function 清空
	/// 
	void Clear()
	{
		m_ActiveMap.clear();
		m_FreePool.Clear();
	}

private:
	CObjectPool<TVAL, POOL_LOCK>		m_FreePool;		///<空闲池	
	ActivePoolMap		m_ActiveMap;		///<活动列表
	MAP_LOCK			m_ActiveObjLock;	///<活动对象锁
	MAP_LOCK			m_FreeObjLock;		///<空闲对象锁

}; // end class CObjectPoolMap


/// @Brief 对象池活动列表
template <typename T, typename LIST_LOCK = CFaskLock, typename POOL_LOCK = CFaskLock>
class CObjectPoolList
{
private:
	typedef std::list<T*>	ActivePoolList;

public:
	typedef typename ActivePoolList::iterator ListIt;
	typedef typename ActivePoolList::const_iterator ListCit;

public:
	CObjectPoolList(int chunk = poolChunkSize, bool autoAlloc = true, bool autoCheckDel = false)
		:m_FreePool(chunk, autoAlloc, autoCheckDel)
	{

	}

	~CObjectPoolList()
	{
		Clear();
	}

	/// @function 获取活动发送包数量
	/// 
	/// @return 返回数量
	size_t GetActiveNum()
	{
		CLockGuard<LIST_LOCK> lock(m_ActiveObjLock);
		return m_ActiveList.size();
	}

	/// @function 获取一个活动对象
	/// 
	/// @param remove 获取后是否从列表中移除
	/// @return 返回发送包
	T* GetActiveObj(bool remove = true)
	{
		CLockGuard<LIST_LOCK> lock(m_ActiveObjLock);
		if(remove)
		{
			T* pak = m_ActiveList.front();
			m_ActiveList.pop_front();
			return pak;
		}
		else
		{
			return m_ActiveList.front();
		}
		return NULL;
	}

	/// @function 投递对象到活动列表
	/// 
	void PostActiveObj(T* obj)
	{
		if(obj)
		{
			CLockGuard<LIST_LOCK> lock(m_ActiveObjLock);
			m_ActiveList.push_back(obj);
		}
	}

	/// @function 获取一个空闲对象
	/// 
	/// @return 返回空闲对象
	T* GetFreeObj()
	{
		CLockGuard<LIST_LOCK> lock(m_FreeObjLock);
		T* obj = m_FreePool.Alloc();
		return obj;
	}

	/// @function 投递对象到空闲池
	/// 
	void PostFreeObj(T* obj)
	{
		if(obj)
		{
			CLockGuard<LIST_LOCK> lock(m_FreeObjLock);
			m_FreePool.Free(obj);
		}
	}

	ListIt Begin()
	{
		return m_ActiveList.begin();
	}

	ListCit Begin() const
	{
		return m_ActiveList.begin();
	}

	ListIt End()
	{
		return m_ActiveList.end();
	}

	ListCit End() const
	{
		return m_ActiveList.end();
	}

	/// @function 清空
	/// 
	void Clear()
	{
		m_ActiveList.clear();
		m_FreePool.Clear();
	}

private:
	CObjectPool<T, POOL_LOCK>		m_FreePool;		///<空闲池	
	ActivePoolList		m_ActiveList;		///<活动列表
	LIST_LOCK			m_ActiveObjLock;	///<活动对象锁
	LIST_LOCK			m_FreeObjLock;		///<空闲对象锁

}; // end class CObjectPoolList


/// @Brief 活动对象列表
template <typename TKEY, typename TVAL, typename MAP_LOCK = CFaskLock>
class CActiveObjectMap
{
private:
	typedef boost::unordered_map<TKEY, TVAL*>	ActivePoolMap;

public:
	typedef typename ActivePoolMap::iterator MapIt;
	typedef typename ActivePoolMap::const_iterator MapCit;

public:
	CActiveObjectMap()
	{

	}

	~CActiveObjectMap()
	{
		Clear();
	}

	/// @function 获取活动对象数量
	/// 
	/// @return 返回数量
	size_t GetActiveNum()
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		return m_ActiveMap.size();
	}

	/// @function 获取一个活动对象
	/// 
	/// @param key 活动对象key
	/// @return 返回对象
	TVAL* FindActiveObj(TKEY key)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.find(key);
		if(it == m_ActiveMap.end())
			return NULL;

		return it->second;
	}

	/// @function 投递对象到活动列表
	/// 
	void InsertActiveObj(TKEY key, TVAL* val)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.find(key);
		if(it != m_ActiveMap.end())
		{
			assert(false);
			return;
		}
		m_ActiveMap.insert(std::make_pair(key, val));
	}

	/// @function 移除活动对象
	/// 
	/// @note 只移除不释放
	void RemoveActiveObj(TKEY key)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		m_ActiveMap.erase(key);
	}

	/// @function 删除活动对象
	/// 
	/// @note 会释放该对象
	void DelActiveObj(TKEY key)
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.find(key);
		if(it != m_ActiveMap.end())
		{
			_SAFE_DELETE_(it->second);
			m_ActiveMap.erase(it);
		}
	}

	MapIt Begin()
	{
		return m_ActiveMap.begin();
	}

	MapCit Begin() const
	{
		return m_ActiveMap.begin();
	}

	MapIt End()
	{
		return m_ActiveMap.end();
	}

	MapCit End() const
	{
		return m_ActiveMap.end();
	}

	/// @function 清空
	/// 
	void Clear()
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		m_ActiveMap.clear();
	}

	/// @function 释放
	/// 
	void Release()
	{
		CLockGuard<MAP_LOCK> lock(m_ActiveObjLock);
		typename ActivePoolMap::iterator it = m_ActiveMap.begin();
		typename ActivePoolMap::iterator tmp;
		for(; it != m_ActiveMap.end(); )
		{
			tmp = it;
			++tmp;
			_SAFE_DELETE_(it->second);
			it = tmp;
		}
		m_ActiveMap.clear();
	}

private:	
	ActivePoolMap		m_ActiveMap;		///<活动列表
	MAP_LOCK			m_ActiveObjLock;	///<活动对象锁

}; // end class CActiveObjectMap

} // end namespace Kernel

#endif //__OBJECT_POOL_H__