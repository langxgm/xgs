
#include "to_ws_session.h"
#include "from_client_session.h"
#include "gate/main/GateServer.h"
#include "gate/logic/HandleMsgMacros.h"
#include "gate/logic/PlayerManager.h"
#include "gate/logic/Player.h"
#include "gate/logic/LoginHandler.h"

#include "gate/config/GameConfig.h"
#include "gate/pbconfig/msginfo.conf.pb.h"

#include "gate/protos/core.pb.h"

#include "xbase/TimeUtil.h"
#include "xshare/work/WorkDispatcher.h"

#include <glog/logging.h>
#include <evpp/buffer.h>

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

void To_Ws_Session::OnMissMessage(uint32_t nMsgID, const void* pMsg, size_t nLen, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	// 注意:这里是异步调用

	auto pRealMeta = std::static_pointer_cast<To_Ws_Meta>(pMeta);
	if (!pRealMeta)
	{
		return;
	}

	// 消息配置
	auto pMsgInfo = GameConfig::Me()->GetMsgInfo(nMsgID);
	if (!pMsgInfo)
	{
		LOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen
			<< " not found pbconfig::MsgInfo";
		return;
	}
	// 消息是否开放
	if (pMsgInfo->open() == false)
	{
		LOG(INFO) << "OnMissMessage/ok msgid=" << nMsgID << " name=" << pMsgInfo->name()
			<< " pbconfig::MsgInfo is closed";
		return;
	}
	// 是否发往Client
	if (pMsgInfo->dir().w2c() == false)
	{
		LOG(INFO) << "OnMissMessage/ok msgid=" << nMsgID << " name=" << pMsgInfo->name()
			<< " pbconfig::MsgInfo.dir().w2c() == false";
		return;
	}

	if (pRealMeta->Response().guid() == 0
		&& pRealMeta->Response().guids_size() == 0)
	{
		LOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen << " name=" << pMsgInfo->name()
			<< " resp.guid==0 && resp.guids_size==0";
		return;
	}

	auto pWriteBuffer = From_Client_Session::Me()->GetBufferAllocator().Alloc();
	if (!pWriteBuffer)
	{
		LOG(WARNING) << "OnMissMessage/fail msgid=" << nMsgID << " msgLen=" << nLen << " name=" << pMsgInfo->name()
			<< " alloc write buffer is nullptr";
		return;
	}

	// 先打包数据
	MessageMeta meta;
	meta.SetMsgID(nMsgID);
	From_Client_Session::Me()->WriteBuffer(pWriteBuffer.get(), pMsg, nLen, &meta);

	std::shared_ptr<BufferAllocator::BufferPtr::element_type> sharedBuffer = std::move(pWriteBuffer);

	// 转到主线程
	WorkDispatcherSync().RunInLoop([=]() {

		if (pRealMeta->Response().guid() != 0)
		{
			auto pPlayer = PlayerManager::Me()->GetPlayer(pRealMeta->Response().guid());
			if (pPlayer)
			{
				From_Client_Session::Me()->Send(pPlayer->GetSessionID(), sharedBuffer->data(), sharedBuffer->length());

				if (pMsgInfo->log().on())
				{
					LOG(INFO) << /*"OnMissMessage/ok "*/"{W => C} " << pMsgInfo->name() << "[" << nLen << "]"
						<< " guid=" << pPlayer->GetPlayerGUID() << " sid=" << pPlayer->GetSessionID();
				}
			}
		}

		if (pRealMeta->Response().guids_size() > 0)
		{
			for (auto& nPlayerGUID : pRealMeta->Response().guids())
			{
				auto pPlayer = PlayerManager::Me()->GetPlayer(nPlayerGUID);
				if (pPlayer)
				{
					From_Client_Session::Me()->Send(pPlayer->GetSessionID(), sharedBuffer->data(), sharedBuffer->length());
				}
			}

			if (pMsgInfo->log().on())
			{
				LOG(INFO) << /*"OnMissMessage/ok "*/"{W => C} " << pMsgInfo->name() << "[" << nLen << "]"
					<< " guids_size=" << pRealMeta->Response().guids_size();
			}
		}

		From_Client_Session::Me()->ResetBuffer(sharedBuffer.get());
	});
}

void To_Ws_Session::OnConnected(int64_t nSessionID)
{
	SendAuth();
}

void To_Ws_Session::SendPing()
{
	protos::Ping send;
	send.set_message("GWS => WS");
	Send1(&send, To_Ws_Meta());

	LOG_FIRST_N(WARNING, 10) << "network: send ping " << send.message();
}

void To_Ws_Session::SendAuth()
{
	protos::LoginAuth send;
	send.set_code(0xA4A8DDA9);
	send.set_id(GateServer::Me()->GetID());
	send.set_group(1);
	send.set_name("GWS");
	send.set_listen_client_port(0);
	Send1(&send, To_Ws_Meta());

	LOG(WARNING) << "network: to world server auth msg: " << send.ShortDebugString();
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
			<< " errmsg=" << pHandleMsg->errmsg();
	}
	else
	{
		LOG(WARNING) << "network: to world server auth result is successful";

		SendPing();
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
