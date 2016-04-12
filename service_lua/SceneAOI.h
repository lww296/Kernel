#ifndef __SCENE_AOI_H__
#define __SCENE_AOI_H__

#include "ObjectPool.h"
#include "AOI.h"

typedef struct tagAOIEntry
{
	uint64_t	m_UserID;
	float		m_Pos[2];
	int			m_State;
}AOIEntry;

// 场景aoi
class CSceneAOI
{
public:
	CSceneAOI(float radius = 0);
	~CSceneAOI();

	/// @function 初始化
	/// 
	/// @return 成功返回true, 失败返回false
	bool Init(aoi_Callback enter = NULL, aoi_Callback leave = NULL);

	/// @function 启动
	/// 
	/// @return 成功返回true, 失败返回false
	bool Start();

	/// @functioni 停止
	/// 
	/// @return 成功返回true, 失败返回false
	bool Stop();

	/// @function tick
	/// 
	void Update();

	/// @function 插入aoi对象
	/// @userid 用户id
	/// @param pos 位置
	/// @return 成功返回true
	bool Insert(unsigned int userId, float pos[2], float radius = 0);

	/// @function 移除aoi对象
	/// @userid 用户id
	/// @param pos 位置
	/// @return 成功返回true
	bool Remove(unsigned int userId);

private:
	// key: userid
	Kernel::CObjectPoolMap<unsigned int, AOIEntry> m_UserAOIMap;
	aoi_space*		m_AOISpace;
	aoi_Callback	m_EnterCall;
	aoi_Callback	m_LeaveCall;

};

#endif //__SCENE_AOI_H__