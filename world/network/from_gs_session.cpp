
#include "from_gs_session.h"
#include "world/logic/HandleMsgMacros.h"
#include "world/logic/LoginHandler.h"

#include "world/protos/core.pb.h"

#include "common/logic/t_server_cache.h"
#include "common/logic/ServerManager.h"
#include "common/logic/ServerNodeImpl.h"
#include "common/logic/ServerNodeMacros.h"

#include <evpp/utility.h>
#include <evpp/tcp_conn.h>
#include <glog/logging.h>

class From_Gs_ServerNode : public ServerNode_Base
{
public:
	virtual ServerType GetType() override { return ServerType::Game; }

	virtual void DoPong() override
	{
		protos::Pong send;
		send.set_message("WS => GS");
		From_Gs_Session::Me()->Send(GetSessionID(), &send, From_Gs_Meta());

		LOG_FIRST_N(WARNING, 10) << "network: send pong " << send.message();
	}
};

void From_Gs_Session::RegisterListen()
{
	REG_MSG(&From_Gs_Session::HandlePing, this, protos::Ping, "ping请求");
	REG_MSG(&From_Gs_Session::HandleLoginAuth, this, protos::LoginAuth, "连接认证请求");
	REG_MSG(&From_Gs_Session::HandleRegisterService, this, protos::RegisterService, "注册服务请求");
	REG_MSG(&From_Gs_Session::HandleUnregisterService, this, protos::UnregisterService, "注销服务请求");

	// 消息处理者
	LoginHandler::Me()->ListenFromGs(this);
}

MessagePacker* From_Gs_Session::CreateMessagePacker()
{
	return new From_Gs_MessagePacker();
}

void From_Gs_Session::Update()
{
	ServerWorker::Update();

	ServerManager::Me()->Update(ServerType::Game);
}

void From_Gs_Session::OnDisconnect(int64_t nSessionID)
{
	ServerNode_OnDisconnect(ServerType::Game);
}

void From_Gs_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Ping);
	ServerNode_HandlePing(ServerType::Game);
}

void From_Gs_Session::HandleLoginAuth(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LoginAuth);
	ServerNode_HandleLoginAuth(ServerType::Game, From_Gs_ServerNode);

	std::string strUrl = "(not found t_server info)";
	auto pDBServer = dbcache::t_server_cache::Me()->GetServer(dbcache::ServerType::Game, pNode->GetID());
	if (pDBServer)
	{
		strUrl = pDBServer->strUrl;
	}

	LOG(WARNING) << "network: from server auth sessionid=" << nSessionID
		<< " url=" << strUrl;
}

void From_Gs_Session::HandleRegisterService(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::RegisterService);

	auto pNode = ServerManager::Me()->GetServerBySessionID(ServerType::Game, nSessionID);
	if (!pNode)
	{
		return;
	}

	for (auto& it : pHandleMsg->services())
	{
		auto& sp = it.second;

		std::vector<int64_t> vecParam;
		vecParam.push_back(sp.p1());
		vecParam.push_back(sp.p2());
		vecParam.push_back(sp.p3());
		pNode->AddService(it.first, vecParam);
	}

	protos::RegisterServiceResult send;
	Send(nSessionID, &send, *pMeta);

	LOG(WARNING) << "network: from server register service msg: " << pHandleMsg->ShortDebugString()
		<< " , id=" << pNode->GetID()
		<< " sessionid=" << nSessionID;
}

void From_Gs_Session::HandleUnregisterService(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::UnregisterService);

	auto pNode = ServerManager::Me()->GetServerBySessionID(ServerType::Game, nSessionID);
	if (!pNode)
	{
		return;
	}

	for (auto& strName : pHandleMsg->service_names())
	{
		pNode->DelService(strName);
	}

	protos::UnregisterServiceResult send;
	Send(nSessionID, &send, *pMeta);

	LOG(WARNING) << "network: from server unregister service msg: " << pHandleMsg->ShortDebugString()
		<< " , id=" << pNode->GetID()
		<< " sessionid=" << nSessionID;
}

MessageMeta* From_Gs_MessagePacker::NewMeta()
{
	return new From_Gs_Meta();
}

void From_Gs_MessagePacker::PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta)
{
	MessagePacker::PackBefore(nMsgID, pMsg, pMeta);
	if (pMeta)
	{
		auto pRealMeta = static_cast<const From_Gs_Meta*>(pMeta);

		// 自动填充来时的GUID
		if (pRealMeta->Response().guid() == 0)
		{
			const_cast<From_Gs_Meta*>(pRealMeta)->MutableResponse()->set_guid(pRealMeta->Request().guid());
		}
	}
}
