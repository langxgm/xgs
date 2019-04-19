//------------------------------------------------------------------------
// * @filename: from_client_session.h
// *
// * @brief: 来自客户端的连接
// *
// * @author: XGM
// * @date: 2018/12/27
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/ServerWorker.h"

class From_Client_Session : public ServerWorker, public Singleton<From_Client_Session>
{
protected:
	friend class Singleton<From_Client_Session>;
	From_Client_Session();
	virtual ~From_Client_Session();
public:
	//------------------------------------------------------------------------
	// 注册监听的消息
	//------------------------------------------------------------------------
	virtual void RegisterListen() override;

	//------------------------------------------------------------------------
	// 断开连接
	//------------------------------------------------------------------------
	virtual void OnDisconnect(int64_t nSessionID) override;

public:
	void HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
