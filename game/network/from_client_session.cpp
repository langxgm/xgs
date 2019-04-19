
#include "from_client_session.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/PlayerManager.h"
#include "game/logic/Player.h"

//#include "game/protos/verify.pb.h"

#include <glog/logging.h>

void From_Client_Session::RegisterListen()
{
	//REG_MSG(&From_Client_Session::HandlePing, this, protos::CSPing, "ping请求");

	// 消息处理者
}

void From_Client_Session::OnDisconnect(int64_t nSessionID)
{
	//auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
	//if (pPlayer)
	//{
	//	pPlayer->OnDisconnect();
	//}
}

void From_Client_Session::OnMissMessage(uint32_t nMsgID, const void* pMsg, size_t nLen, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	// 注意:这里是异步调用

#ifdef _DEBUG
/*
	int64_t nPlayerGUID = 0;

	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
	if (pPlayer)
	{
		nPlayerGUID = pPlayer->GetPlayerGUID();
	}

	// 未注册的消息
	DLOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen
		<< " guid=" << nPlayerGUID
		<< " sid=" << nSessionID;
*/
#endif
}

void From_Client_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	/*
	MSG_CHECK_OF(protos::CSPing);

	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
	if (!pPlayer)
	{
		DLOG(WARNING) << "HandlePing/fail sessionid=" << nSessionID << " ping.code=" << pHandleMsg->code() << " not logged in";
		return;
	}

	pPlayer->GetNetHeartbeat().OnRecvAction();

	protos::SCPong send;
	send.set_code(pHandleMsg->code());
	Send(nSessionID, &send);

	LOG_FIRST_N(WARNING, 10) << "network: recv ping C => S";
	LOG_FIRST_N(WARNING, 10) << "network: send pong S => C";
	*/
}
