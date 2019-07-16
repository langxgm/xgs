
#include "from_client_session.h"
#include "to_ws_session.h"
#include "to_gs_session.h"
#include "gate/logic/HandleMsgMacros.h"
#include "gate/logic/PlayerManager.h"
#include "gate/logic/Player.h"
#include "gate/logic/LoginHandler.h"

#include "gate/config/GameConfig.h"
#include "gate/pbconfig/msginfo.conf.pb.h"

#include "gate/protos/verify.pb.h"

#include "xshare/work/WorkDispatcher.h"

#include <glog/logging.h>
#include <evpp/buffer.h>

From_Client_Session::From_Client_Session()
{

}

From_Client_Session::~From_Client_Session()
{

}

void From_Client_Session::RegisterListen()
{
	REG_MSG(&From_Client_Session::HandlePing, this, protos::CSPing, "ping请求");

	// 消息处理者
	LoginHandler::Me()->ListenFromClient(this);
}

void From_Client_Session::OnDisconnect(int64_t nSessionID)
{
	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
	if (pPlayer)
	{
		pPlayer->OnDisconnect();
	}
}

void From_Client_Session::OnMissMessage(uint32_t nMsgID, const void* pMsg, size_t nLen, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	// 注意:这里是异步调用

	std::string strMsg(static_cast<const char*>(pMsg), nLen);

	// 转到主线程
	WorkDispatcherSync().RunInLoop([=, strMsg = std::move(strMsg)]() {

		auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);
		if (!pPlayer)
		{
			LOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen
				<< " sid=" << nSessionID
				<< " not found Player";
			return;
		}

		// 消息配置
		auto pMsgInfo = GameConfig::Me()->GetMsgInfo(nMsgID);
		if (!pMsgInfo)
		{
			LOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen
				<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID()
				<< " not found pbconfig::MsgInfo";
			return;
		}
		// 消息是否开放
		if (pMsgInfo->open() == false)
		{
			LOG(INFO) << "OnMissMessage/ok msgid=" << nMsgID << " name=" << pMsgInfo->name()
				<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID()
				<< " pbconfig::MsgInfo is closed";
			return;
		}

		To_Ws_Meta meta2ws;
		meta2ws.SetMsgID(nMsgID);
		meta2ws.SetGUID(pPlayer->GetPlayerGUID());

		To_Gs_Meta meta2gs;
		meta2gs.SetMsgID(nMsgID);
		meta2gs.SetGUID(pPlayer->GetPlayerGUID());

		MessageMeta* pTargetMeta = nullptr;

		ClientWorker* pTargetWorker = nullptr;

		if (pMsgInfo->dir().c2w()) // 是否发往World
		{
			pTargetWorker = To_Ws_Session::Me();
			pTargetMeta = &meta2ws;
		}
		else if (pMsgInfo->dir().c2g()) // 是否发往Game
		{
			pTargetWorker = To_Gs_Session::Me();
			pTargetMeta = &meta2gs;
		}
		else
		{
			LOG(WARNING) << "OnMissMessage/ok msgid=" << nMsgID << " name=" << pMsgInfo->name()
				<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID()
				<< " pbconfig::MsgInfo.dir().c2w() == false && c2g() == false";
			return;
		}

		auto pWriteBuffer = pTargetWorker->GetBufferAllocator().Alloc();
		if (!pWriteBuffer)
		{
			LOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " name=" << pMsgInfo->name()
				<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID()
				<< " alloc write buffer is nullptr";
			return;
		}

		pTargetWorker->WriteBuffer(pWriteBuffer.get(), strMsg.c_str(), nLen, pTargetMeta);
		pTargetWorker->Send1(pWriteBuffer->data(), pWriteBuffer->length());
		pTargetWorker->ResetBuffer(pWriteBuffer.get());

		if (pMsgInfo->log().on())
		{
			const char* pszDir = "Undefined direction";
			if (pMsgInfo->dir().c2w())
			{
				pszDir = "{C => W} ";
			}
			else if (pMsgInfo->dir().c2g())
			{
				pszDir = "{C => G} ";
			}
			LOG(INFO) << /*"OnMissMessage/ok "*/pszDir << pMsgInfo->name() << "[" << nLen << "]"
				<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID();
		}
	});
}

void From_Client_Session::HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
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
}
