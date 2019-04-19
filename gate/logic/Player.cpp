
#include "Player.h"
#include "PlayerManager.h"
#include "gate/network/from_client_session.h"
#include "gate/network/to_gs_session.h"
#include "gate/network/to_ws_session.h"

#include "gate/protos/login2game.pb.h"

#include <glog/logging.h>

Player::Player()
{

}

Player::~Player()
{

}

void Player::Init(int64_t nPlayerGUID, int64_t nSessionID)
{
	m_nPlayerGUID = nPlayerGUID;
	m_nSessionID = nSessionID;
}

void Player::Reconnect(int64_t nSessionID)
{
	m_nSessionID = nSessionID;
	m_Heartbeat.Reset();
}

void Player::OnDisconnect()
{
	// 通知game有人断线
	protos::GWS2GDisconnect send;
	send.set_guid(GetPlayerGUID());
	To_Gs_Session::Me()->Send1(&send, To_Gs_Meta(GetPlayerGUID()));

	// 通知world有人断线
	protos::GWS2WDisconnect send2w;
	send2w.set_guid(GetPlayerGUID());
	To_Ws_Session::Me()->Send1(&send2w, To_Ws_Meta(GetPlayerGUID()));

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
