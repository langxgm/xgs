
#include "from_client_session.h"
#include "link/logic/HandleMsgMacros.h"
#include "link/logic/UserLinkManager.h"
#include "link/logic/UserLink.h"
#include "link/logic/LoginHandler.h"
#include "link/logic/UserHandler.h"

#include "link/protos/verify.pb.h"

#include <glog/logging.h>

From_Client_Session::From_Client_Session()
{

}

From_Client_Session::~From_Client_Session()
{

}

void From_Client_Session::RegisterListen()
{
	REG_MSG(&From_Client_Session::HandlePing, this, protos::CSPing, "ping请求");

	// 消息处理者
	LoginHandler::Me()->ListenFromClient(this);
	UserHandler::Me()->ListenFromClient(this);
}

void From_Client_Session::OnDisconnect(int64_t nSessionID)
{
	auto pUser = UserLinkManager::Me()->GetUserBySessionID(nSessionID);
	if (pUser)
	{
		pUser->OnDisconnect();
	}
}

void From_Client_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CSPing);

	auto pUser = UserLinkManager::Me()->GetUserBySessionID(nSessionID);
	if (!pUser)
	{
		DLOG(WARNING) << "HandlePing/fail sessionid=" << nSessionID << " ping.code=" << pHandleMsg->code() << " not logged in";
		return;
	}

	pUser->GetNetHeartbeat().OnRecvAction();

	protos::SCPong send;
	send.set_code(pHandleMsg->code());
	Send(nSessionID, &send);

	LOG_FIRST_N(WARNING, 10) << "network: recv ping C => S";
	LOG_FIRST_N(WARNING, 10) << "network: send pong S => C";
}
