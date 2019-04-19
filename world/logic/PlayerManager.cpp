
#include "PlayerManager.h"
#include "PlayerMapping.h"
#include "PlayerConcreteFactory.h"
#include "Player.h"

#include <assert.h>

PlayerManager::PlayerManager()
{

}

PlayerManager::~PlayerManager()
{
	m_pPlayerFactory.reset();
	m_pPlayerMapping.reset();
}

void PlayerManager::Init(uint32_t nPlayerLimit)
{
	m_pPlayerMapping.reset(new PlayerMapping());
	m_pPlayerFactory.reset(CreatePlayerFactory());

	m_nPlayerLimit = nPlayerLimit;
}

PlayerFactory* PlayerManager::CreatePlayerFactory()
{
	return new PlayerDefaultFactory();
}

PlayerFactory* PlayerManager::GetPlayerFactory()
{
	return m_pPlayerFactory.get();
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

PlayerPtr PlayerManager::GetPlayer(int64_t nPlayerGUID)
{
	return m_pPlayerMapping->GetPlayer(nPlayerGUID);
}

PlayerPtr PlayerManager::GetPlayerByOpenID(const std::string& strOpenID)
{
	return m_pPlayerMapping->GetPlayerByOpenID(strOpenID);
}

PlayerPtr PlayerManager::GetPlayerByLoginKey(const std::string& strLoginKey)
{
	return m_pPlayerMapping->GetPlayerByLoginKey(strLoginKey);
}

void PlayerManager::ModifyMapping(const PlayerPtr& pPlayer, const std::string& strOpenID,
	const std::string& strSessionKey, const std::string& strLoginKey)
{
	m_pPlayerMapping->DelPlayer(pPlayer->GetPlayerGUID());

	pPlayer->SetOpenID(strOpenID);
	pPlayer->SetSessionKey(strSessionKey);

	pPlayer->SetLoginKey(strLoginKey);

	m_pPlayerMapping->AddPlayer(pPlayer->GetPlayerGUID(), pPlayer);
}

uint32_t PlayerManager::GetPlayerNum()
{
	return m_pPlayerMapping->GetPlayerNum();
}
