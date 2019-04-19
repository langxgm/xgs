
#include "Player.h"

Player::Player()
{
	m_pClientInfo.reset(new PlayerClientInfo());
}

Player::~Player()
{
	m_pClientInfo.reset();
}

void Player::Init(int64_t nPlayerGUID, int64_t nGwsSessionID)
{
	m_nPlayerGUID = nPlayerGUID;
	m_nSessionID = nGwsSessionID;
}

void Player::Reconnect(int64_t nGwsSessionID)
{
	m_nSessionID = nGwsSessionID;
}

void Player::Online()
{
	m_bOnline = true;
}

void Player::Offline()
{
	m_bOnline = false;
}
