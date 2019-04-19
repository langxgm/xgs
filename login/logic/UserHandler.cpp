
#include "UserHandler.h"
#include "login/network/from_link_session.h"
#include "login/logic/HandleMsgMacros.h"

#include "login/protos/login.pb.h"

#include <glog/logging.h>

UserHandler::UserHandler()
{

}

UserHandler::~UserHandler()
{

}

void UserHandler::PreInitDatabase()
{

}

void UserHandler::UpdateLogic()
{

}

void UserHandler::ListenFromLink(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &UserHandler::HandleCLServerList, this, protos::CLServerList, "服务器列表请求");
}

void UserHandler::HandleCLServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLServerList);

	int64_t nUserID = pMeta->GetGUID();

	protos::LCServerList send;

	auto pNew = send.add_server_list();
	pNew->set_id(1);
	pNew->set_name("gws");
	pNew->set_state(protos::LCServerList::OPEN);
	pNew->set_addr("172.16.2.87:9001");
	pNew->mutable_player_info()->set_guid(16777217);
	pNew->mutable_player_info()->set_name("player_1");
	pNew->mutable_player_info()->set_portraitid(121);

	auto pNew2 = send.add_server_list();
	pNew2->set_id(2);
	pNew2->set_name("gws");
	pNew2->set_state(protos::LCServerList::OPEN);
	pNew2->set_addr("172.16.2.87:9001");
	pNew2->mutable_player_info()->set_guid(16777218);
	pNew2->mutable_player_info()->set_name("player_2");
	pNew2->mutable_player_info()->set_portraitid(122);
	From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

	auto pNew3 = send.add_server_list();
	pNew3->set_id(3);
	pNew3->set_name("gws");
	pNew3->set_state(protos::LCServerList::OPEN);
	pNew3->set_addr("172.16.2.87:9001");
	pNew3->mutable_player_info()->set_guid(16777219);
	pNew3->mutable_player_info()->set_name("player_3");
	pNew3->mutable_player_info()->set_portraitid(123);

	From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleCLServerList/ok userid=" << nUserID
		<< " server_list_size=" << send.server_list_size();
}
