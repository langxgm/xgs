
#include "PlayerManager.h"
#include "PlayerMapping.h"
#include "PlayerFactory.h"
#include "Player.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

#include <assert.h>

PlayerManager::PlayerManager()
{

}

PlayerManager::~PlayerManager()
{
	m_pPlayerMapping.reset();
}

void PlayerManager::Init(uint32_t nPlayerLimit)
{
	m_pPlayerMapping.reset(new PlayerMapping());

	m_nPlayerLimit = nPlayerLimit;
}

void PlayerManager::Update()
{
#if 0
	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_HeartbeatTimer.IsStart() == false)
	{
		m_HeartbeatTimer.Start(nNow, 5 * 1000);
	}
	if (m_HeartbeatTimer.IsTimeout(nNow))
	{
		auto& rPlayers = m_pPlayerMapping->GetPlayers();
		auto& rLoginKeyPlayers = m_pPlayerMapping->GetLoginKeyPlayers();
		for (auto it = rPlayers.begin(); it != rPlayers.end();)
		{
			auto& pPlayer = it->second;
			if (pPlayer->CheckHeartbeat(nNow))
			{
				++it;
			}
			else
			{
				auto holdPlayer = pPlayer;

				rLoginKeyPlayers.erase(pPlayer->GetLoginKey());
				it = rPlayers.erase(it);

				holdPlayer->OnDisconnect();

				LOG(INFO) << "KickPlayer heartbeat timeout guid=" << holdPlayer->GetPlayerGUID();
			}
		}
	}
#endif
}

PlayerPtr PlayerManager::NewPlayer(PlayerFactory* pFactory /*= nullptr*/)
{
	if (pFactory)
	{
		return PlayerPtr(static_cast<Player*>(pFactory->NewPlayer()));
	}
	return PlayerPtr(new Player());
}

bool PlayerManager::AddPlayer(int64_t nPlayerGUID, const PlayerPtr& pPlayer)
{
	if (GetPlayerNum() >= GetPlayerLimit())
	{
		return false; // 人满了
	}
	assert(nPlayerGUID == pPlayer->GetPlayerGUID());
	return m_pPlayerMapping->AddPlayer(nPlayerGUID, pPlayer);
}

bool PlayerManager::DelPlayer(int64_t nPlayerGUID)
{
	return m_pPlayerMapping->DelPlayer(nPlayerGUID);
}

void PlayerManager::DelPlayer(const PlayerPtr& pPlayer)
{
	m_pPlayerMapping->DelPlayer(pPlayer->GetPlayerGUID());
}

PlayerPtr PlayerManager::GetPlayer(int64_t nPlayerGUID)
{
	return m_pPlayerMapping->GetPlayer(nPlayerGUID);
}

PlayerPtr PlayerManager::GetPlayerByLoginKey(const std::string& strLoginKey)
{
	return m_pPlayerMapping->GetPlayerByLoginKey(strLoginKey);
}

std::map<int64_t, PlayerPtr>& PlayerManager::GetPlayers()
{
	return m_pPlayerMapping->GetPlayers();
}

void PlayerManager::ModifyMapping(const PlayerPtr& pPlayer, const std::string& strLoginKey)
{
	m_pPlayerMapping->DelPlayer(pPlayer->GetPlayerGUID());

	pPlayer->SetLoginKey(strLoginKey);

	m_pPlayerMapping->AddPlayer(pPlayer->GetPlayerGUID(), pPlayer);
}

uint32_t PlayerManager::GetPlayerNum()
{
	return m_pPlayerMapping->GetPlayerNum();
}
