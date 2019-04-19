
#include "to_sdk_session.h"
#include "world/logic/HandleMsgMacros.h"

#include "world/protos/core.pb.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

void To_Sdk_Session::RegisterListen()
{
	REG_MSG(&To_Sdk_Session::HandlePong, this, protos::Pong, "ping回应");

	// 消息处理者
}

void To_Sdk_Session::Update()
{
	ClientWorker::Update();

	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_PingTimer.IsTimeout(nNow))
	{
		m_PingTimer.Stop();

		SendPing();
	}
}

void To_Sdk_Session::OnConnected(int64_t nSessionID)
{
	SendPing();
}

void To_Sdk_Session::SendPing()
{
	protos::Ping send;
	send.set_message("WS => SDK");
	Send1(&send);

	LOG_FIRST_N(WARNING, 10) << "network: send ping " << send.message();
}

void To_Sdk_Session::HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Pong);

	m_PingTimer.Start(TimeUtil::GetCurrentTimeMillis(), 30 * 1000);

	LOG_FIRST_N(WARNING, 10) << "network: recv pong " << pHandleMsg->message();
}
