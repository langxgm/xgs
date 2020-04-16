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

class PlayerMapping;
typedef std::unique_ptr<PlayerMapping> PlayerMappingPtr;

class PlayerFactory;

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
	PlayerPtr NewPlayer(PlayerFactory* pFactory = nullptr);

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
	PlayerPtr GetPlayerByLoginKey(const std::string& strLoginKey);

	template<typename T>
	std::shared_ptr<T> GetPlayer_cast(int64_t nPlayerGUID)
	{
		return std::static_pointer_cast<T>(GetPlayer(nPlayerGUID));
	}

	template<typename T>
	std::shared_ptr<T> GetPlayerByLoginKey_cast(const std::string& strLoginKey)
	{
		return std::static_pointer_cast<T>(GetPlayerByLoginKey(strLoginKey));
	}

	//------------------------------------------------------------------------
	// 获取玩家列表
	//------------------------------------------------------------------------
	std::map<int64_t, PlayerPtr>& GetPlayers();

	//------------------------------------------------------------------------
	// 修改映射关系
	//------------------------------------------------------------------------
	void ModifyMapping(const PlayerPtr& pPlayer, const std::string& strLoginKey);

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
	// 玩家的映射表
	PlayerMappingPtr m_pPlayerMapping;

	// 玩家人数限制
	uint32_t m_nPlayerLimit = 0;

	// 心跳检测定时器
	TimeChecker m_HeartbeatTimer;
};
