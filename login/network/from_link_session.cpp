
#include "from_link_session.h"
#include "login/logic/HandleMsgMacros.h"
#include "login/logic/LoginHandler.h"
#include "login/logic/UserHandler.h"

#include "login/protos/core.pb.h"

#include <glog/logging.h>

From_Link_Session::From_Link_Session()
{

}

From_Link_Session::~From_Link_Session()
{

}

void From_Link_Session::RegisterListen()
{
	REG_MSG(&From_Link_Session::HandlePing, this, protos::Ping, "ping请求");

	// 消息处理者
	LoginHandler::Me()->ListenFromLink(this);
	UserHandler::Me()->ListenFromLink(this);
}

MessagePacker* From_Link_Session::CreateMessagePacker()
{
	return new From_Link_MessagePacker();
}

void From_Link_Session::OnDisconnect(int64_t nSessionID)
{
	LOG(WARNING) << "network: disconnect name=" << "LINK"
		<< " sessionid=" << nSessionID;
}

void From_Link_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Ping);

	protos::Pong send;
	send.set_message("LS => LINK");
	Send(nSessionID, &send, *pMeta);

	LOG_FIRST_N(WARNING, 10) << "network: recv ping " << pHandleMsg->message();
	LOG_FIRST_N(WARNING, 10) << "network: send pong " << send.message();
}

MessageMeta* From_Link_MessagePacker::NewMeta()
{
	return new From_Link_Meta();
}

void From_Link_MessagePacker::PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta)
{
	MessagePacker::PackBefore(nMsgID, pMsg, pMeta);
	if (pMeta)
	{
		auto pRealMeta = static_cast<const From_Link_Meta*>(pMeta);

		// 自动填充来时的userid
		if (pRealMeta->Response().userid() == 0)
		{
			const_cast<From_Link_Meta*>(pRealMeta)->MutableResponse()->set_userid(pRealMeta->Request().userid());
		}
	}
}
