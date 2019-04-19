
#include "from_ws_session.h"
#include "sdk/logic/HandleMsgMacros.h"

#include "sdk/protos/core.pb.h"

#include <glog/logging.h>

From_Ws_Session::From_Ws_Session()
{

}

From_Ws_Session::~From_Ws_Session()
{

}

void From_Ws_Session::RegisterListen()
{
	REG_MSG(&From_Ws_Session::HandlePing, this, protos::Ping, "ping请求");

	// 消息处理者
}

void From_Ws_Session::OnDisconnect(int64_t nSessionID)
{
	LOG(WARNING) << "network: disconnect name=" << "WS"
		<< " sessionid=" << nSessionID;
}

void From_Ws_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Ping);

	protos::Pong send;
	send.set_message("SDK => WS");
	Send(nSessionID, &send, *pMeta);

	LOG_FIRST_N(WARNING, 10) << "network: recv ping " << pHandleMsg->message();
	LOG_FIRST_N(WARNING, 10) << "network: send pong " << send.message();
}
