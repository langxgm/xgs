
#include "to_gs_session.h"
#include "gate/main/GateServer.h"
#include "gate/logic/HandleMsgMacros.h"
#include "gate/logic/LoginHandler.h"

#include "gate/protos/core.pb.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

To_Gs_Session::To_Gs_Session()
{

}

To_Gs_Session::~To_Gs_Session()
{

}

void To_Gs_Session::RegisterListen()
{
	REG_MSG(&To_Gs_Session::HandlePong, this, protos::Pong, "ping回应");
	REG_MSG(&To_Gs_Session::HandleLoginAuthResult, this, protos::LoginAuthResult, "连接认证回应");

	// 消息处理者
	LoginHandler::Me()->ListenFromGs(this);
}

MessagePacker* To_Gs_Session::CreateMessagePacker()
{
	return new To_Gs_MessagePacker();
}

void To_Gs_Session::Update()
{
	ClientWorker::Update();

	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_PingTimer.IsTimeout(nNow))
	{
		m_PingTimer.Stop();

		SendPing();
	}
}

void To_Gs_Session::OnConnected(int64_t nSessionID)
{
	SendAuth();
}

void To_Gs_Session::SendPing()
{
	protos::Ping send;
	send.set_message("GWS => GS");
	Send1(&send, To_Gs_Meta());

	LOG_FIRST_N(WARNING, 10) << "network: send ping " << send.message();
}

void To_Gs_Session::SendAuth()
{
	protos::LoginAuth send;
	send.set_code(0xA4A8DDA9);
	send.set_id(GateServer::Me()->GetID());
	send.set_group(1);
	send.set_name("GWS");
	send.set_listen_client_port(0);
	Send1(&send, To_Gs_Meta());

	LOG(WARNING) << "network: to game server auth msg: " << send.ShortDebugString();
}

void To_Gs_Session::HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Pong);

	m_PingTimer.Start(TimeUtil::GetCurrentTimeMillis(), 30 * 1000);

	LOG_FIRST_N(WARNING, 10) << "network: recv pong " << pHandleMsg->message();
}

void To_Gs_Session::HandleLoginAuthResult(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LoginAuthResult);

	if (pHandleMsg->error() != 0)
	{
		LOG(ERROR) << "network: to game server auth result error=" << pHandleMsg->error()
			<< " errmsg" << pHandleMsg->errmsg();
	}
	else
	{
		LOG(WARNING) << "network: to game server auth result is successful";

		SendPing();
	}
}

MessageMeta* To_Gs_MessagePacker::NewMeta()
{
	return new To_Gs_Meta();
}

void To_Gs_MessagePacker::PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta)
{
	MessagePacker::PackBefore(nMsgID, pMsg, pMeta);
	if (pMeta)
	{
		auto pRealMeta = static_cast<const To_Gs_Meta*>(pMeta);

		// 自动填充来时的GUID
		if (pRealMeta->Response().guid() == 0)
		{
			const_cast<To_Gs_Meta*>(pRealMeta)->MutableResponse()->set_guid(pRealMeta->Request().guid());
		}
	}
}
