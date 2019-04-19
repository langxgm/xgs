
#include "Player.h"
#include "PlayerManager.h"
#include "game/network/from_client_session.h"

#include <glog/logging.h>

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
	m_Heartbeat.Reset();
}

void Player::OnDisconnect()
{
	LOG(INFO) << "OnDisconnect/ok guid=" << GetPlayerGUID() << " sid=" << GetSessionID()
		<< " playerNum=" << PlayerManager::Me()->GetPlayerNum() - 1;

	PlayerManager::Me()->DelPlayer(GetPlayerGUID());
}

bool Player::CheckHeartbeat(int64_t nNow)
{
	// 3次没来心跳,主动断开连接
	if (m_Heartbeat.GetCount() >= 3)
	{
		m_Heartbeat.Reset();

		From_Client_Session::Me()->CloseConn(GetSessionID());

		return false;
	}
	else if (nNow - m_Heartbeat.GetLastSendTime() >= 10 * 1000)
	{
		m_Heartbeat.OnSendAction();
	}
	return true;
}

void Player::Online()
{
	m_bOnline = true;
}

void Player::Offline()
{
	m_bOnline = false;
}
