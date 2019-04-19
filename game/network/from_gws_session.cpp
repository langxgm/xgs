
#include "from_gws_session.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/LoginHandler.h"
#include "game/logic/GameDataHandler.h"
#include "game/logic/MailHandler.h"
#include "game/logic/FeedbackHandler.h"

#include "game/protos/core.pb.h"

#include "common/logic/ServerManager.h"
#include "common/logic/ServerNodeImpl.h"
#include "common/logic/ServerNodeMacros.h"

#include <evpp/utility.h>
#include <evpp/tcp_conn.h>
#include <glog/logging.h>

class From_Gws_ServerNode : public ServerNode_Base
{
public:
	virtual ServerType GetType() override { return ServerType::Gateway; }

	virtual void DoPong() override
	{
		protos::Pong send;
		send.set_message("GS => GWS");
		From_Gws_Session::Me()->Send(GetSessionID(), &send, From_Gws_Meta());

		LOG_FIRST_N(WARNING, 10) << "network: send pong " << send.message();
	}
};

From_Gws_Session::From_Gws_Session()
{

}

From_Gws_Session::~From_Gws_Session()
{

}

void From_Gws_Session::RegisterListen()
{
	REG_MSG(&From_Gws_Session::HandlePing, this, protos::Ping, "ping请求");
	REG_MSG(&From_Gws_Session::HandleLoginAuth, this, protos::LoginAuth, "连接认证请求");

	// 消息处理者
	LoginHandler::Me()->ListenFromGws(this);
	GameDataHandler::Me()->ListenFromGws(this);
	MailHandler::Me()->ListenFromGws(this);
	FeedbackHandler::Me()->ListenFromGws(this);
}

MessagePacker* From_Gws_Session::CreateMessagePacker()
{
	return new From_Gws_MessagePacker();
}

void From_Gws_Session::Update()
{
	ServerWorker::Update();

	ServerManager::Me()->Update(ServerType::Gateway);
}

void From_Gws_Session::OnDisconnect(int64_t nSessionID)
{
	ServerNode_OnDisconnect(ServerType::Gateway);
}

void From_Gws_Session::OnMissMessage(uint32_t nMsgID, const void* pMsg, size_t nLen, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	// 注意:这里是异步调用

	// 未注册的消息
	DLOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen
		<< " guid=" << pMeta->GetGUID()
		<< " sid=" << nSessionID;
}

void From_Gws_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Ping);
	ServerNode_HandlePing(ServerType::Gateway);
}

void From_Gws_Session::HandleLoginAuth(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LoginAuth);
	ServerNode_HandleLoginAuth(ServerType::Gateway, From_Gws_ServerNode);
}

MessageMeta* From_Gws_MessagePacker::NewMeta()
{
	return new From_Gws_Meta();
}

void From_Gws_MessagePacker::PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta)
{
	MessagePacker::PackBefore(nMsgID, pMsg, pMeta);
	if (pMeta)
	{
		auto pRealMeta = static_cast<const From_Gws_Meta*>(pMeta);

		// 自动填充来时的GUID
		if (pRealMeta->Response().guid() == 0)
		{
			const_cast<From_Gws_Meta*>(pRealMeta)->MutableResponse()->set_guid(pRealMeta->Request().guid());
		}
	}
}
