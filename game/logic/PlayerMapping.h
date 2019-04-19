//------------------------------------------------------------------------
// * @filename: PlayerMapping.h
// *
// * @brief: 玩家的映射表
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdint.h>

class Player;
typedef std::shared_ptr<Player> PlayerPtr;

class PlayerMapping
{
public:
	PlayerMapping();
	~PlayerMapping();

	//------------------------------------------------------------------------
	// 增加玩家
	//------------------------------------------------------------------------
	bool AddPlayer(int64_t nPlayerGUID, const PlayerPtr& pPlayer);

	//------------------------------------------------------------------------
	// 删除玩家
	//------------------------------------------------------------------------
	bool DelPlayer(int64_t nPlayerGUID);

	//------------------------------------------------------------------------
	// 获取玩家
	//------------------------------------------------------------------------
	PlayerPtr GetPlayer(int64_t nPlayerGUID);
	PlayerPtr GetPlayerByLoginKey(const std::string& strLoginKey);

	//------------------------------------------------------------------------
	// 获取玩家列表
	//------------------------------------------------------------------------
	std::map<int64_t, PlayerPtr>& GetPlayers() { return m_mapPlayers; }
	std::map<std::string, PlayerPtr>& GetLoginKeyPlayers() { return m_mapLoginKeyPlayers; }

	//------------------------------------------------------------------------
	// 获得玩家人数
	//------------------------------------------------------------------------
	uint32_t GetPlayerNum();

private:
	// 玩家列表管理<PlayerGUID, 玩家>
	std::map<int64_t, PlayerPtr> m_mapPlayers;

	// 玩家映射<login_key, 玩家>
	std::map<std::string, PlayerPtr> m_mapLoginKeyPlayers;
};
