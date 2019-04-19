
#include "UserLink.h"
#include "UserLinkManager.h"
#include "link/network/from_client_session.h"
#include "link/network/to_ls_session.h"

#include "link/protos/login2sdk.pb.h"

#include <glog/logging.h>

UserLink::UserLink()
{

}

UserLink::~UserLink()
{

}

void UserLink::Init(int64_t nUserID, int64_t nSessionID)
{
	m_nUserID = nUserID;
	m_nSessionID = nSessionID;
}

void UserLink::Reconnect(int64_t nSessionID)
{
	m_nSessionID = nSessionID;
	m_Heartbeat.Reset();
}

void UserLink::OnDisconnect()
{
	// 通知login有人断线
	protos::Link2LDisconnect send;
	send.set_userid(GetUserID());

	To_Ls_Session::Me()->Send1(&send, To_Ls_Meta(GetUserID()));

	LOG(INFO) << "OnDisconnect/ok userid=" << GetUserID() << " sid=" << GetSessionID()
		<< " userNum=" << UserLinkManager::Me()->GetUserNum() - 1;

	UserLinkManager::Me()->DelUser(GetUserID());
}

bool UserLink::CheckHeartbeat(int64_t nNow)
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
