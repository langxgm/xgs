
#include "PlayerMapping.h"
#include "Player.h"

PlayerMapping::PlayerMapping()
{

}

PlayerMapping::~PlayerMapping()
{
	m_mapLoginKeyPlayers.clear();
	m_mapOpenIDPlayers.clear();
	m_mapPlayers.clear();
}

bool PlayerMapping::AddPlayer(int64_t nPlayerGUID, const PlayerPtr& pPlayer)
{
	if (m_mapPlayers.find(nPlayerGUID) == m_mapPlayers.end())
	{
		m_mapPlayers.insert(std::make_pair(pPlayer->GetPlayerGUID(), pPlayer));
		m_mapOpenIDPlayers.insert(std::make_pair(pPlayer->GetOpenID(), pPlayer));
		m_mapLoginKeyPlayers.insert(std::make_pair(pPlayer->GetLoginKey(), pPlayer));
		return true;
	}
	return false;
}

bool PlayerMapping::DelPlayer(int64_t nPlayerGUID)
{
	auto pPlayer = GetPlayer(nPlayerGUID);
	if (pPlayer)
	{
		m_mapPlayers.erase(pPlayer->GetPlayerGUID());
		m_mapOpenIDPlayers.erase(pPlayer->GetOpenID());
		m_mapLoginKeyPlayers.erase(pPlayer->GetLoginKey());
		return true;
	}
	return false;
}

PlayerPtr PlayerMapping::GetPlayer(int64_t nPlayerGUID)
{
	auto it = m_mapPlayers.find(nPlayerGUID);
	if (it != m_mapPlayers.end())
	{
		return it->second;
	}
	return nullptr;
}

PlayerPtr PlayerMapping::GetPlayerByOpenID(const std::string& strOpenID)
{
	auto it = m_mapOpenIDPlayers.find(strOpenID);
	if (it != m_mapOpenIDPlayers.end())
	{
		return it->second;
	}
	return nullptr;
}

PlayerPtr PlayerMapping::GetPlayerByLoginKey(const std::string& strLoginKey)
{
	auto it = m_mapLoginKeyPlayers.find(strLoginKey);
	if (it != m_mapLoginKeyPlayers.end())
	{
		return it->second;
	}
	return nullptr;
}

uint32_t PlayerMapping::GetPlayerNum()
{
	return m_mapPlayers.size();
}
