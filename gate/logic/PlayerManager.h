//------------------------------------------------------------------------
// * @filename: PlayerManager.h
// *
// * @brief: 玩家管理
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xbase/TimeChecker.h"

#include <map>
#include <memory>
#include <stdint.h>

class Player;
typedef std::shared_ptr<Player> PlayerPtr;

class PlayerManager : public Singleton<PlayerManager>
{
protected:
	friend class Singleton<PlayerManager>;
	PlayerManager();
	virtual ~PlayerManager();
public:
	//------------------------------------------------------------------------
	// 初始化
	//------------------------------------------------------------------------
	virtual void Init(uint32_t nPlayerLimit);

	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	void Update();

	//------------------------------------------------------------------------
	// 创建玩家
	//------------------------------------------------------------------------
	PlayerPtr NewPlayer();

	//------------------------------------------------------------------------
	// 增加玩家
	//------------------------------------------------------------------------
	bool AddPlayer(int64_t nPlayerGUID, const PlayerPtr& pPlayer);

	//------------------------------------------------------------------------
	// 删除玩家
	//------------------------------------------------------------------------
	bool DelPlayer(int64_t nPlayerGUID);
	void DelPlayer(const PlayerPtr& pPlayer);

	//------------------------------------------------------------------------
	// 获取玩家
	//------------------------------------------------------------------------
	PlayerPtr GetPlayer(int64_t nPlayerGUID);
	PlayerPtr GetPlayerBySessionID(int64_t nSessionID);

	//------------------------------------------------------------------------
	// 修改SessionID
	//------------------------------------------------------------------------
	void ModifySessionID(int64_t nOldSessionID, const PlayerPtr& pPlayer);

public:
	//------------------------------------------------------------------------
	// 获取玩家数量
	//------------------------------------------------------------------------
	uint32_t GetPlayerNum();

	//------------------------------------------------------------------------
	// 获取玩家数量上限
	//------------------------------------------------------------------------
	uint32_t GetPlayerLimit() { return m_nPlayerLimit; }

private:
	// 玩家列表管理<PlayerGUID, 玩家>
	std::map<int64_t, PlayerPtr> m_mapPlayers;

	// 玩家映射<SessionID, 玩家>
	std::map<int64_t, PlayerPtr> m_mapSessionIDPlayers;

	// 玩家人数限制
	uint32_t m_nPlayerLimit = 0;

	// 心跳检测定时器
	TimeChecker m_HeartbeatTimer;
};
