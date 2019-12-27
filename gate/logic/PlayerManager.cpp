
#include "PlayerManager.h"
#include "Player.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

#include <assert.h>

PlayerManager::PlayerManager()
{

}

PlayerManager::~PlayerManager()
{
	m_mapSessionIDPlayers.clear();
	m_mapPlayers.clear();
}

void PlayerManager::Init(uint32_t nPlayerLimit)
{
	m_nPlayerLimit = nPlayerLimit;
}

void PlayerManager::Update()
{
	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_HeartbeatTimer.IsStart() == false)
	{
		m_HeartbeatTimer.Start(nNow, 5 * 1000);
	}
	if (m_HeartbeatTimer.IsTimeout(nNow))
	{
		for (auto it = m_mapPlayers.begin(); it != m_mapPlayers.end();)
		{
			auto& pPlayer = it->second;
			if (pPlayer->CheckHeartbeat(nNow))
			{
				++it;
			}
			else
			{
				auto holdPlayer = pPlayer;

				m_mapSessionIDPlayers.erase(pPlayer->GetSessionID());
				it = m_mapPlayers.erase(it);

				holdPlayer->OnDisconnect();

				LOG(INFO) << "KickPlayer heartbeat timeout guid=" << holdPlayer->GetPlayerGUID()
					<< " sid=" << holdPlayer->GetSessionID();
			}
		}
	}
}

PlayerPtr PlayerManager::NewPlayer()
{
	return PlayerPtr(new Player());
}

bool PlayerManager::AddPlayer(int64_t nPlayerGUID, const PlayerPtr& pPlayer)
{
	if (GetPlayerNum() >= GetPlayerLimit())
	{
		return false; // 人满了
	}

	assert(nPlayerGUID == pPlayer->GetPlayerGUID());

	if (m_mapPlayers.find(nPlayerGUID) == m_mapPlayers.end())
	{
		auto itSIDPlayer = m_mapSessionIDPlayers.find(pPlayer->GetSessionID());
		if (itSIDPlayer == m_mapSessionIDPlayers.end())
		{
			m_mapPlayers.insert(std::make_pair(nPlayerGUID, pPlayer));
			m_mapSessionIDPlayers.insert(std::make_pair(pPlayer->GetSessionID(), pPlayer));
			return true;
		}
	}
	return false;
}

bool PlayerManager::DelPlayer(int64_t nPlayerGUID)
{
	auto pPlayer = GetPlayer(nPlayerGUID);
	if (pPlayer)
	{
		m_mapPlayers.erase(pPlayer->GetPlayerGUID());
		m_mapSessionIDPlayers.erase(pPlayer->GetSessionID());
	}
	return false;
}

void PlayerManager::DelPlayer(const PlayerPtr& pPlayer)
{
	if (pPlayer)
	{
		m_mapPlayers.erase(pPlayer->GetPlayerGUID());
		m_mapSessionIDPlayers.erase(pPlayer->GetSessionID());
	}
}

PlayerPtr PlayerManager::GetPlayer(int64_t nPlayerGUID)
{
	auto it = m_mapPlayers.find(nPlayerGUID);
	if (it != m_mapPlayers.end())
	{
		return it->second;
	}
	return nullptr;
}

PlayerPtr PlayerManager::GetPlayerBySessionID(int64_t nSessionID)
{
	auto it = m_mapSessionIDPlayers.find(nSessionID);
	if (it != m_mapSessionIDPlayers.end())
	{
		return it->second;
	}
	return nullptr;
}

std::map<int64_t, PlayerPtr>& PlayerManager::GetPlayers()
{
	return m_mapPlayers;
}

void PlayerManager::ModifySessionID(int64_t nOldSessionID, const PlayerPtr& pPlayer)
{
	m_mapSessionIDPlayers.erase(nOldSessionID);
	m_mapSessionIDPlayers.insert(std::make_pair(pPlayer->GetSessionID(), pPlayer));
}

uint32_t PlayerManager::GetPlayerNum()
{
	return m_mapPlayers.size();
}
