
#include "UserHandler.h"
#include "link/network/from_client_session.h"
#include "link/network/to_ls_session.h"
#include "link/logic/HandleMsgMacros.h"
#include "link/logic/UserLinkManager.h"
#include "link/logic/UserLink.h"

#include "link/protos/login.pb.h"

#include <glog/logging.h>

UserHandler::UserHandler()
{
}

UserHandler::~UserHandler()
{
}

void UserHandler::UpdateLogic()
{
}

void UserHandler::ListenFromClient(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &UserHandler::HandleCLServerList, this, protos::CLServerList, "服务器列表请求");
}

void UserHandler::ListenFromLs(ClientWorker* pServer)
{
	REG_MSG_TO(pServer, &UserHandler::HandleLCServerList, this, protos::LCServerList, "服务器列表回应");
}

void UserHandler::HandleCLServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLServerList);

	auto pUser = UserLinkManager::Me()->GetUserBySessionID(nSessionID);
	if (!pUser)
	{
		// 没找到用户
		LOG(INFO) << "HandleCLServerList/fail sid=" << nSessionID
			<< " not found user";
		return;
	}

	To_Ls_Session::Me()->Send1(pHandleMsg.get(), To_Ls_Meta(pUser->GetUserID()));

	LOG(INFO) << "HandleCLServerList/ok userid=" << pUser->GetUserID()
		<< " sid=" << nSessionID;
}

void UserHandler::HandleLCServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LCServerList);

	auto pUser = UserLinkManager::Me()->GetUser(pMeta->GetGUID());
	if (!pUser)
	{
		// 没找到用户
		LOG(INFO) << "HandleLCServerList/fail userid=" << pMeta->GetGUID()
			<< " not found user";
		return;
	}

	From_Client_Session::Me()->Send(pUser->GetSessionID(), pHandleMsg.get());

	LOG(INFO) << "HandleLCServerList/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " server_list_size=" << pHandleMsg->server_list_size();
}
