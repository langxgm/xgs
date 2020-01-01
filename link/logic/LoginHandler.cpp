
#include "LoginHandler.h"
#include "link/main/LinkServer.h"
#include "link/network/from_client_session.h"
#include "link/network/to_ls_session.h"
#include "link/logic/HandleMsgMacros.h"
#include "link/logic/UserLinkManager.h"
#include "link/logic/UserLink.h"

#include "link/config/SystemConfig.h"
#include "link/pbconfig/link.conf.pb.h"

#include "link/protos/login.pb.h"
#include "link/protos/login2sdk.pb.h"

#include <evpp/utility.h>
#include <evpp/tcp_conn.h>

#include <glog/logging.h>

LoginHandler::LoginHandler()
{
	UserLinkManager::InitInstance();
	UserLinkManager::Me()->Init(SystemConfig::Me()->GetLinkConfig()->networkmodule().from_c_session_num() * 5);
}

LoginHandler::~LoginHandler()
{
	UserLinkManager::DestroyInstance();
}

void LoginHandler::UpdateLogic()
{
	UserLinkManager::Me()->Update();
}

void LoginHandler::ListenFromClient(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLogin, this, protos::CLLogin, "游客登陆请求");
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLoginAccount, this, protos::CLLoginAccount, "账号登陆请求");
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLoginWeixin, this, protos::CLLoginWeixin, "登陆请求(微信平台)");
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLoginFacebook, this, protos::CLLoginFacebook, "登陆请求(Facebook平台)");

	REG_MSG_TO(pServer, &LoginHandler::HandleCLReconnLogin, this, protos::CLReconnLogin, "断线重连请求");
}

void LoginHandler::ListenFromLs(ClientWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleLCLogin, this, protos::LCLogin, "游客登陆回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleLCLoginAccount, this, protos::LCLoginAccount, "账号登陆回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleLCLoginWeixin, this, protos::LCLoginWeixin, "登陆回应(微信平台)");
	REG_MSG_TO(pServer, &LoginHandler::HandleLCLoginFacebook, this, protos::LCLoginFacebook, "登陆回应(Facebook平台)");

	REG_MSG_TO(pServer, &LoginHandler::HandleLCReconnLogin, this, protos::LCReconnLogin, "断线重连回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleL2LinkDisconnect, this, protos::L2LinkDisconnect, "断线回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleL2LinkKickPlayer, this, protos::L2LinkKickPlayer, "踢下线");
}

void LoginHandler::LoginLink(int64_t nUserID, int64_t nSessionID)
{
	auto pUser = UserLinkManager::Me()->GetUser(nUserID);
	if (!pUser)
	{
		pUser = UserLinkManager::Me()->NewUser();
		if (!pUser)
		{
			return;
		}

		pUser->Init(nUserID, nSessionID);

		if (UserLinkManager::Me()->AddUser(nUserID, pUser))
		{
			// 设置认证成功
			From_Client_Session::Me()->SetAuth(nSessionID, true);

			LOG(INFO) << "LoginLink/ok AddUser userid=" << nUserID << " sid=" << nSessionID
				<< " userNum=" << UserLinkManager::Me()->GetUserNum() << " userLimit=" << UserLinkManager::Me()->GetUserLimit();
		}
		else
		{
			LOG(WARNING) << "LoginLink/fail AddUser userid=" << nUserID << " sid=" << nSessionID << " user full"
				<< " userNum=" << UserLinkManager::Me()->GetUserNum() << " userLimit=" << UserLinkManager::Me()->GetUserLimit();
		}
	}
	else
	{
		int64_t nOldSessionID = pUser->GetSessionID();

		pUser->Reconnect(nSessionID);

		// 重置SessionIDUsers映射表
		UserLinkManager::Me()->ModifySessionID(nOldSessionID, pUser);

		// 设置认证成功
		From_Client_Session::Me()->SetAuth(nSessionID, true);

		// 顶号关闭旧连接
		From_Client_Session::Me()->CloseConn(nOldSessionID);

		LOG(INFO) << "LoginLink/ok ModifySessionID userid=" << nUserID
			<< " old_sid=" << nOldSessionID << " new_sid=" << nSessionID;
	}
}

void LoginHandler::HandleCLLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLogin);

	std::string strIP;
	if (auto conn = From_Client_Session::Me()->GetConnPtr(nSessionID))
	{
		std::vector<std::string> vecString;
		evpp::StringSplit(conn->remote_addr(), ":", 0, vecString);
		if (vecString.size() > 0)
		{
			strIP = vecString[0];
		}
	}

	// 限制长度
	if (pHandleMsg->deviceid().length() > 128)
	{
		pHandleMsg->mutable_deviceid()->resize(128);
	}
	pHandleMsg->set_ip(std::move(strIP));
	pHandleMsg->set_client_sessionid(nSessionID);

	To_Ls_Session::Me()->Send1(pHandleMsg.get(), To_Ls_Meta());

	LOG(INFO) << "HandleCLLogin/ok sid=" << nSessionID
		<< " ip=" << pHandleMsg->ip();
}

void LoginHandler::HandleLCLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LCLogin);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->userid() != 0)
		{
			LoginLink(pHandleMsg->userid(), pHandleMsg->client_sessionid());
		}
	}

	int64_t nClientSessionID = pHandleMsg->client_sessionid();
	pHandleMsg->set_client_sessionid(0);

	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleLCLogin/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pHandleMsg->userid() << " sid=" << nClientSessionID;
}

void LoginHandler::HandleCLLoginAccount(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLoginAccount);

	std::string strIP;
	if (auto conn = From_Client_Session::Me()->GetConnPtr(nSessionID))
	{
		std::vector<std::string> vecString;
		evpp::StringSplit(conn->remote_addr(), ":", 0, vecString);
		if (vecString.size() > 0)
		{
			strIP = vecString[0];
		}
	}

	// 限制长度
	if (pHandleMsg->username().length() > 128)
	{
		pHandleMsg->mutable_username()->resize(128);
	}
	if (pHandleMsg->password().length() > 128)
	{
		pHandleMsg->mutable_password()->resize(128);
	}
	if (pHandleMsg->deviceid().length() > 128)
	{
		pHandleMsg->mutable_deviceid()->resize(128);
	}
	pHandleMsg->set_ip(std::move(strIP));
	pHandleMsg->set_client_sessionid(nSessionID);

	To_Ls_Session::Me()->Send1(pHandleMsg.get(), To_Ls_Meta());

	LOG(INFO) << "HandleCLLoginAccount/ok sid=" << nSessionID
		<< " ip=" << pHandleMsg->ip();
}

void LoginHandler::HandleLCLoginAccount(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LCLoginAccount);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->userid() != 0)
		{
			LoginLink(pHandleMsg->userid(), pHandleMsg->client_sessionid());
		}
	}

	int64_t nClientSessionID = pHandleMsg->client_sessionid();
	pHandleMsg->set_client_sessionid(0);

	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleLCLoginAccount/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pHandleMsg->userid() << " sid=" << nClientSessionID;
}

void LoginHandler::HandleCLLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLoginWeixin);

	std::string strIP;
	if (auto conn = From_Client_Session::Me()->GetConnPtr(nSessionID))
	{
		std::vector<std::string> vecString;
		evpp::StringSplit(conn->remote_addr(), ":", 0, vecString);
		if (vecString.size() > 0)
		{
			strIP = vecString[0];
		}
	}

	pHandleMsg->clear_route();
	pHandleMsg->mutable_route()->set_client_sessionid(nSessionID);
	pHandleMsg->mutable_route()->set_link_id(LinkServer::Me()->GetID());

	if (pHandleMsg->param().deviceid().empty())
	{
		pHandleMsg->mutable_param()->set_deviceid("sessionid:" + std::to_string(nSessionID));
	}
	if (pHandleMsg->param().deviceid().length() > 128)
	{
		pHandleMsg->mutable_param()->mutable_deviceid()->resize(128);
	}

	pHandleMsg->mutable_param()->set_ip(std::move(strIP));

	if (pHandleMsg->param().js_code().length() > 128)
	{
		pHandleMsg->mutable_param()->mutable_js_code()->resize(128);
	}

	To_Ls_Session::Me()->Send1(pHandleMsg.get(), To_Ls_Meta());

	LOG(INFO) << "HandleCLLoginWeixin/ok sid=" << nSessionID
		<< " ip=" << pHandleMsg->param().ip();
}

void LoginHandler::HandleLCLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LCLoginWeixin);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->userid() != 0)
		{
			LoginLink(pHandleMsg->userid(), pHandleMsg->route().client_sessionid());
		}
	}

	int64_t nClientSessionID = pHandleMsg->route().client_sessionid();

	pHandleMsg->clear_route();
	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleLCLoginWeixin/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pHandleMsg->userid() << " sid=" << nClientSessionID
		<< " wx_errcode=" << pHandleMsg->wx_errcode() << " wx_errmsg=" << pHandleMsg->wx_errmsg();
}

void LoginHandler::HandleCLLoginFacebook(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLoginFacebook);

	std::string strIP;
	if (auto conn = From_Client_Session::Me()->GetConnPtr(nSessionID))
	{
		std::vector<std::string> vecString;
		evpp::StringSplit(conn->remote_addr(), ":", 0, vecString);
		if (vecString.size() > 0)
		{
			strIP = vecString[0];
		}
	}

	pHandleMsg->clear_route();
	pHandleMsg->mutable_route()->set_client_sessionid(nSessionID);
	pHandleMsg->mutable_route()->set_link_id(LinkServer::Me()->GetID());

	if (pHandleMsg->param().deviceid().empty())
	{
		pHandleMsg->mutable_param()->set_deviceid("sessionid:" + std::to_string(nSessionID));
	}
	if (pHandleMsg->param().deviceid().length() > 128)
	{
		pHandleMsg->mutable_param()->mutable_deviceid()->resize(128);
	}

	pHandleMsg->mutable_param()->set_ip(std::move(strIP));

	if (pHandleMsg->param().code().length() > 128)
	{
		pHandleMsg->mutable_param()->mutable_code()->resize(128);
	}

	To_Ls_Session::Me()->Send1(pHandleMsg.get(), To_Ls_Meta());

	LOG(INFO) << "HandleCLLoginFacebook/ok sid=" << nSessionID
		<< " ip=" << pHandleMsg->param().ip();
}

void LoginHandler::HandleLCLoginFacebook(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LCLoginFacebook);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->userid() != 0)
		{
			LoginLink(pHandleMsg->userid(), pHandleMsg->route().client_sessionid());
		}
	}

	int64_t nClientSessionID = pHandleMsg->route().client_sessionid();

	pHandleMsg->clear_route();
	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleLCLoginFacebook/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pHandleMsg->userid() << " sid=" << nClientSessionID;
}

void LoginHandler::HandleCLReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLReconnLogin);

	pHandleMsg->clear_route();
	pHandleMsg->mutable_route()->set_client_sessionid(nSessionID);
	pHandleMsg->mutable_route()->set_link_id(LinkServer::Me()->GetID());

	To_Ls_Session::Me()->Send1(pHandleMsg.get(), To_Ls_Meta());

	LOG(INFO) << "HandleCLReconnLogin/ok sid=" << nSessionID;
}

void LoginHandler::HandleLCReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LCReconnLogin);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->userid() != 0)
		{
			LoginLink(pHandleMsg->userid(), pHandleMsg->route().client_sessionid());
		}
	}

	int64_t nClientSessionID = pHandleMsg->route().client_sessionid();

	pHandleMsg->clear_route();
	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleLCReconnLogin/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pHandleMsg->userid() << " sid=" << nClientSessionID;
}

void LoginHandler::HandleL2LinkDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::L2LinkDisconnect);

	LOG(INFO) << "HandleL2LinkDisconnect/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pHandleMsg->userid();
}

void LoginHandler::HandleL2LinkKickPlayer(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::L2LinkKickPlayer);

	auto pUser = UserLinkManager::Me()->GetUser(pHandleMsg->userid());
	if (!pUser)
	{
		// 没找到用户
		DLOG(WARNING) << "HandleL2LinkKickPlayer/fail userid=" << pHandleMsg->userid()
			<< " not found user";
		return;
	}

	LOG(INFO) << "HandleL2LinkKickPlayer/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " userid=" << pUser->GetUserID() << " sid=" << pUser->GetSessionID();

	// 踢下线
	From_Client_Session::Me()->CloseConn(pUser->GetSessionID());
}
