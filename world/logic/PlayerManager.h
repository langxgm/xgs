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

#include <string>
#include <memory>
#include <stdint.h>

class Player;
typedef std::shared_ptr<Player> PlayerPtr;
typedef std::unique_ptr<Player> PlayerUPtr;

class PlayerMapping;
typedef std::unique_ptr<PlayerMapping> PlayerMappingPtr;

class PlayerFactory;
typedef std::unique_ptr<PlayerFactory> PlayerFactoryPtr;

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
	// 创建玩家的工厂
	//------------------------------------------------------------------------
	virtual PlayerFactory* CreatePlayerFactory();

	//------------------------------------------------------------------------
	// 获得玩家工厂
	//------------------------------------------------------------------------
	PlayerFactory* GetPlayerFactory();

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
	PlayerPtr GetPlayerByOpenID(const std::string& strOpenID);
	PlayerPtr GetPlayerByLoginKey(const std::string& strLoginKey);

	template<typename T>
	std::shared_ptr<T> GetPlayer_cast(int64_t nPlayerGUID)
	{
		return std::static_pointer_cast<T>(GetPlayer(nPlayerGUID));
	}

	template<typename T>
	std::shared_ptr<T> GetPlayerByOpenID_cast(const std::string& strOpenID)
	{
		return std::static_pointer_cast<T>(GetPlayerByOpenID(strOpenID));
	}

	template<typename T>
	std::shared_ptr<T> GetPlayerByLoginKey_cast(const std::string& strLoginKey)
	{
		return std::static_pointer_cast<T>(GetPlayerByLoginKey(strLoginKey));
	}

	//------------------------------------------------------------------------
	// 修改映射关系
	//------------------------------------------------------------------------
	void ModifyMapping(const PlayerPtr& pPlayer, const std::string& strOpenID,
		const std::string& strSessionKey, const std::string& strLoginKey);

public:
	//------------------------------------------------------------------------
	// 获取玩家数量
	//------------------------------------------------------------------------
	uint32_t GetPlayerNum();

	//------------------------------------------------------------------------
	// 获取玩家数量上限
	//------------------------------------------------------------------------
	uint32_t GetPlayerLimit() { return m_nPlayerLimit; }

public:
	// 玩家的映射表
	PlayerMappingPtr m_pPlayerMapping;

	// 创建玩家的工厂
	PlayerFactoryPtr m_pPlayerFactory;

	// 玩家人数限制
	uint32_t m_nPlayerLimit = 0;
};
