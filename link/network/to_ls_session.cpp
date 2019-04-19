
#include "to_ls_session.h"
#include "link/logic/HandleMsgMacros.h"
#include "link/logic/LoginHandler.h"
#include "link/logic/UserHandler.h"

#include "link/protos/core.pb.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

void To_Ls_Session::RegisterListen()
{
	REG_MSG(&To_Ls_Session::HandlePong, this, protos::Pong, "ping回应");

	// 消息处理者
	LoginHandler::Me()->ListenFromLs(this);
	UserHandler::Me()->ListenFromLs(this);
}

MessagePacker* To_Ls_Session::CreateMessagePacker()
{
	return new To_Ls_MessagePacker();
}

void To_Ls_Session::Update()
{
	ClientWorker::Update();

	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_PingTimer.IsTimeout(nNow))
	{
		m_PingTimer.Stop();

		SendPing();
	}
}

void To_Ls_Session::OnConnected(int64_t nSessionID)
{
	SendPing();
}

void To_Ls_Session::SendPing()
{
	protos::Ping send;
	send.set_message("LINK => LS");
	Send1(&send, To_Ls_Meta());

	LOG_FIRST_N(WARNING, 10) << "network: send ping " << send.message();
}

void To_Ls_Session::HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Pong);

	m_PingTimer.Start(TimeUtil::GetCurrentTimeMillis(), 30 * 1000);

	LOG_FIRST_N(WARNING, 10) << "network: recv pong " << pHandleMsg->message();
}

MessageMeta* To_Ls_MessagePacker::NewMeta()
{
	return new To_Ls_Meta();
}

void To_Ls_MessagePacker::PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta)
{
	MessagePacker::PackBefore(nMsgID, pMsg, pMeta);
	if (pMeta)
	{
		auto pRealMeta = static_cast<const To_Ls_Meta*>(pMeta);

		// 自动填充来时的userid
		if (pRealMeta->Response().userid() == 0)
		{
			const_cast<To_Ls_Meta*>(pRealMeta)->MutableResponse()->set_userid(pRealMeta->Request().userid());
		}
	}
}
