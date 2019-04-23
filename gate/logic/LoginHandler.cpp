
#include "LoginHandler.h"
#include "gate/main/GateServer.h"
#include "gate/network/from_client_session.h"
#include "gate/network/to_gs_session.h"
#include "gate/network/to_ws_session.h"
#include "gate/logic/HandleMsgMacros.h"
#include "gate/logic/PlayerManager.h"
#include "gate/logic/Player.h"

#include "gate/config/SystemConfig.h"
#include "gate/pbconfig/gate.conf.pb.h"

#include "gate/protos/login2game.pb.h"

#include <evpp/tcp_conn.h>

#include <glog/logging.h>

LoginHandler::LoginHandler()
{
	PlayerManager::InitInstance();
	PlayerManager::Me()->Init(SystemConfig::Me()->GetGateConfig()->networkmodule().from_c_session_num() * 5);
}

LoginHandler::~LoginHandler()
{
	PlayerManager::DestroyInstance();
}

void LoginHandler::UpdateLogic()
{
	PlayerManager::Me()->Update();
}

void LoginHandler::ListenFromClient(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleCGLogin, this, protos::CGLogin, "登陆请求");

	REG_MSG_TO(pServer, &LoginHandler::HandleCGReconnLogin, this, protos::CGReconnLogin, "断线重连请求");
}

void LoginHandler::ListenFromGs(ClientWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleGCLogin, this, protos::GCLogin, "登陆回应");

	REG_MSG_TO(pServer, &LoginHandler::HandleGCReconnLogin, this, protos::GCReconnLogin, "断线重连回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleG2GWSDisconnect, this, protos::G2GWSDisconnect, "断线回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleG2GWSKickPlayer, this, protos::G2GWSKickPlayer, "踢下线");
}

void LoginHandler::ListenFromWs(ClientWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleW2GWSReconnLogin, this, protos::W2GWSReconnLogin, "断线重连回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleW2GWSDisconnect, this, protos::W2GWSDisconnect, "断线回应");
	REG_MSG_TO(pServer, &LoginHandler::HandleW2GWSKickPlayer, this, protos::W2GWSKickPlayer, "踢下线");
}

void LoginHandler::LoginGate(int64_t nPlayerGUID, int64_t nSessionID)
{
	auto pPlayer = PlayerManager::Me()->GetPlayer(nPlayerGUID);
	if (!pPlayer)
	{
		pPlayer = PlayerManager::Me()->NewPlayer();
		if (!pPlayer)
		{
			return;
		}

		pPlayer->Init(nPlayerGUID, nSessionID);

		if (PlayerManager::Me()->AddPlayer(nPlayerGUID, pPlayer))
		{
			// 设置认证成功
			From_Client_Session::Me()->SetAuth(nSessionID, true);

			LOG(INFO) << "LoginGate/ok AddPlayer guid=" << nPlayerGUID << " sid=" << nSessionID
				<< " playerNum=" << PlayerManager::Me()->GetPlayerNum() << " playerLimit=" << PlayerManager::Me()->GetPlayerLimit();
		}
		else
		{
			LOG(WARNING) << "LoginGate/fail AddPlayer guid=" << nPlayerGUID << " sid=" << nSessionID << " player full"
				<< " playerNum=" << PlayerManager::Me()->GetPlayerNum() << " playerLimit=" << PlayerManager::Me()->GetPlayerLimit();
		}
	}
	else
	{
		int64_t nOldSessionID = pPlayer->GetSessionID();

		pPlayer->Reconnect(nSessionID);

		// 重置SessionIDPlayers映射表
		PlayerManager::Me()->ModifySessionID(nOldSessionID, pPlayer);

		// 设置认证成功
		From_Client_Session::Me()->SetAuth(nSessionID, true);

		// 顶号关闭旧连接
		From_Client_Session::Me()->CloseConn(nOldSessionID);

		LOG(INFO) << "LoginGate/ok ModifySessionID guid=" << nPlayerGUID
			<< " old_sid=" << nOldSessionID << " new_sid=" << nSessionID;
	}
}

void LoginHandler::HandleCGLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGLogin);

	std::string strIP;
	auto conn = From_Client_Session::Me()->GetConnPtr(nSessionID);
	if (conn)
	{
		strIP = conn->remote_addr();
	}

	pHandleMsg->clear_route();
	pHandleMsg->mutable_route()->set_client_sessionid(nSessionID);
	pHandleMsg->mutable_route()->set_gws_id(GateServer::Me()->GetID());
	if (pHandleMsg->param().deviceid().empty())
	{
		pHandleMsg->mutable_param()->set_deviceid("sessionid:" + std::to_string(nSessionID));
	}
	if (pHandleMsg->param().deviceid().length() > 128)
	{
		pHandleMsg->mutable_param()->mutable_deviceid()->resize(128);
	}
	pHandleMsg->mutable_param()->set_ip(std::move(strIP));

	if (pHandleMsg->param().user_token().length() > 64)
	{
		pHandleMsg->mutable_param()->mutable_user_token()->resize(64);
	}

	To_Gs_Session::Me()->Send1(pHandleMsg.get(), To_Gs_Meta());

	LOG(INFO) << "HandleCGLogin/ok sid=" << nSessionID
		<< " ip=" << pHandleMsg->param().ip();
}

void LoginHandler::HandleGCLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::GCLogin);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->guid() != 0)
		{
			LoginGate(pHandleMsg->guid(), pHandleMsg->route().client_sessionid());
		}
	}

	int64_t nClientSessionID = pHandleMsg->route().client_sessionid();

	pHandleMsg->clear_route();
	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleGCLogin/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pHandleMsg->guid() << " sid=" << nClientSessionID;
}

void LoginHandler::HandleCGReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGReconnLogin);

	pHandleMsg->clear_route();
	pHandleMsg->mutable_route()->set_client_sessionid(nSessionID);
	pHandleMsg->mutable_route()->set_gws_id(GateServer::Me()->GetID());

	To_Gs_Session::Me()->Send1(pHandleMsg.get(), To_Gs_Meta());

	LOG(INFO) << "HandleCGReconnLogin/ok sid=" << nSessionID;
}

void LoginHandler::HandleGCReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::GCReconnLogin);

	if (pHandleMsg->error() == 0)
	{
		if (pHandleMsg->guid() != 0)
		{
			LoginGate(pHandleMsg->guid(), pHandleMsg->route().client_sessionid());

			// 通知world
			protos::GWS2WReconnLogin send2w;
			send2w.set_guid(pHandleMsg->guid());
			To_Ws_Session::Me()->Send1(&send2w, To_Ws_Meta(pHandleMsg->guid()));
		}
	}

	int64_t nClientSessionID = pHandleMsg->route().client_sessionid();

	pHandleMsg->clear_route();
	From_Client_Session::Me()->Send(nClientSessionID, pHandleMsg.get());

	LOG(INFO) << "HandleGCReconnLogin/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pHandleMsg->guid() << " sid=" << nClientSessionID;
}

void LoginHandler::HandleW2GWSReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::W2GWSReconnLogin);

	LOG(INFO) << "HandleW2GWSReconnLogin/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pHandleMsg->guid();
}

void LoginHandler::HandleG2GWSDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::G2GWSDisconnect);

	LOG(INFO) << "HandleG2GWSDisconnect/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pHandleMsg->guid();
}

void LoginHandler::HandleW2GWSDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::W2GWSDisconnect);

	LOG(INFO) << "HandleW2GWSDisconnect/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pHandleMsg->guid();
}

void LoginHandler::HandleG2GWSKickPlayer(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::G2GWSKickPlayer);

	auto pPlayer = PlayerManager::Me()->GetPlayer(pHandleMsg->guid());
	if (!pPlayer)
	{
		// 没找到玩家
		DLOG(WARNING) << "HandleG2GWSKickPlayer/fail guid=" << pHandleMsg->guid()
			<< " not found player";
		return;
	}

	LOG(INFO) << "HandleG2GWSKickPlayer/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID();

	// 踢下线
	From_Client_Session::Me()->CloseConn(pPlayer->GetSessionID());
}

void LoginHandler::HandleW2GWSKickPlayer(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::W2GWSKickPlayer);

	auto pPlayer = PlayerManager::Me()->GetPlayer(pHandleMsg->guid());
	if (!pPlayer)
	{
		// 没找到玩家
		DLOG(WARNING) << "HandleW2GWSKickPlayer/fail guid=" << pHandleMsg->guid()
			<< " not found player";
		return;
	}

	LOG(INFO) << "HandleW2GWSKickPlayer/ok error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg()
		<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID();

	// 踢下线
	From_Client_Session::Me()->CloseConn(pPlayer->GetSessionID());
}
