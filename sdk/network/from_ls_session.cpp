
#include "from_ls_session.h"
#include "sdk/logic/HandleMsgMacros.h"
#include "sdk/logic/LoginHandler.h"

#include "sdk/protos/core.pb.h"

#include <glog/logging.h>

From_Ls_Session::From_Ls_Session()
{

}

From_Ls_Session::~From_Ls_Session()
{

}

void From_Ls_Session::RegisterListen()
{
	REG_MSG(&From_Ls_Session::HandlePing, this, protos::Ping, "ping请求");

	// 消息处理者
	LoginHandler::Me()->ListenFromLs(this);
}

void From_Ls_Session::OnDisconnect(int64_t nSessionID)
{
	LOG(WARNING) << "network: disconnect name=" << "LS"
		<< " sessionid=" << nSessionID;
}

void From_Ls_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Ping);

	protos::Pong send;
	send.set_message("SDK => LS");
	Send(nSessionID, &send, *pMeta);

	LOG_FIRST_N(WARNING, 10) << "network: recv ping " << pHandleMsg->message();
	LOG_FIRST_N(WARNING, 10) << "network: send pong " << send.message();
}
