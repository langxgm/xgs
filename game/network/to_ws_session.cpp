
#include "to_ws_session.h"
#include "game/main/GameServer.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/LoginHandler.h"

#include "game/config/SystemConfig.h"
#include "game/pbconfig/game.conf.pb.h"

#include "game/protos/core.pb.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

To_Ws_Session::To_Ws_Session()
{

}

To_Ws_Session::~To_Ws_Session()
{

}

void To_Ws_Session::RegisterListen()
{
	REG_MSG(&To_Ws_Session::HandlePong, this, protos::Pong, "ping回应");
	REG_MSG(&To_Ws_Session::HandleLoginAuthResult, this, protos::LoginAuthResult, "连接认证回应");
	REG_MSG(&To_Ws_Session::HandleRegisterServiceResult, this, protos::RegisterServiceResult, "注册服务回应");

	// 消息处理者
	LoginHandler::Me()->ListenFromWs(this);
}

MessagePacker* To_Ws_Session::CreateMessagePacker()
{
	return new To_Ws_MessagePacker();
}

void To_Ws_Session::Update()
{
	ClientWorker::Update();

	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_PingTimer.IsTimeout(nNow))
	{
		m_PingTimer.Stop();

		SendPing();
	}
}

void To_Ws_Session::OnConnected(int64_t nSessionID)
{
	SendAuth();
}

void To_Ws_Session::SendPing()
{
	protos::Ping send;
	send.set_message("GS => WS");
	Send1(&send, To_Ws_Meta());

	LOG_FIRST_N(WARNING, 10) << "network: send ping " << send.message();
}

void To_Ws_Session::SendAuth()
{
	protos::LoginAuth send;
	send.set_code(0xA4A8DDA9);
	send.set_id(GameServer::Me()->GetID());
	send.set_group(2);
	send.set_name("GS");
	send.set_listen_client_port(SystemConfig::Me()->GetGameConfig()->networkmodule().from_c_listen_port());
	Send1(&send, To_Ws_Meta());

	LOG(WARNING) << "network: to world server auth msg: " << send.ShortDebugString();
}

void To_Ws_Session::SendRegisterService()
{
	protos::RegisterService send;
	Send1(&send, To_Ws_Meta());

	LOG(WARNING) << "network: to world server register service msg: " << send.ShortDebugString();
}

void To_Ws_Session::HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Pong);

	m_PingTimer.Start(TimeUtil::GetCurrentTimeMillis(), 30 * 1000);

	LOG_FIRST_N(WARNING, 10) << "network: recv pong " << pHandleMsg->message();
}

void To_Ws_Session::HandleLoginAuthResult(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::LoginAuthResult);

	if (pHandleMsg->error() != 0)
	{
		LOG(ERROR) << "network: to world server auth result error=" << pHandleMsg->error()
			<< " errmsg" << pHandleMsg->errmsg();
	}
	else
	{
		LOG(WARNING) << "network: to world server auth result is successful";

		SendPing();
		SendRegisterService();
	}
}

void To_Ws_Session::HandleRegisterServiceResult(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::RegisterServiceResult);

	if (pHandleMsg->error() != 0)
	{
		LOG(ERROR) << "network: to world server register service result error=" << pHandleMsg->error()
			<< " errmsg" << pHandleMsg->errmsg();
	}
	else
	{
		LOG(WARNING) << "network: to world server register service result is successful";
	}
}

MessageMeta* To_Ws_MessagePacker::NewMeta()
{
	return new To_Ws_Meta();
}

void To_Ws_MessagePacker::PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta)
{
	MessagePacker::PackBefore(nMsgID, pMsg, pMeta);
	if (pMeta)
	{
		auto pRealMeta = static_cast<const To_Ws_Meta*>(pMeta);

		// 自动填充来时的GUID
		if (pRealMeta->Response().guid() == 0)
		{
			const_cast<To_Ws_Meta*>(pRealMeta)->MutableResponse()->set_guid(pRealMeta->Request().guid());
		}
	}
}
