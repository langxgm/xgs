
#include "from_client_session.h"
#include "to_ws_session.h"
#include "gate/logic/HandleMsgMacros.h"
#include "gate/logic/PlayerManager.h"
#include "gate/logic/Player.h"
#include "gate/logic/LoginHandler.h"

#include "gate/config/GameConfig.h"
#include "gate/pbconfig/msginfo.conf.pb.h"

#include "gate/protos/verify.pb.h"

#include <glog/logging.h>
#include <evpp/buffer.h>

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
}

void From_Client_Session::OnDisconnect(int64_t nSessionID)
{
	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
	if (pPlayer)
	{
		pPlayer->OnDisconnect();
	}
}

void From_Client_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CSPing);

	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
	if (!pPlayer)
	{
		DLOG(WARNING) << "HandlePing/fail sessionid=" << nSessionID << " ping.code=" << pHandleMsg->code() << " not logged in";
		return;
	}

	pPlayer->GetNetHeartbeat().OnRecvAction();

	protos::SCPong send;
	send.set_code(pHandleMsg->code());
	Send(nSessionID, &send);

	LOG_FIRST_N(WARNING, 10) << "network: recv ping C => S";
	LOG_FIRST_N(WARNING, 10) << "network: send pong S => C";
}
