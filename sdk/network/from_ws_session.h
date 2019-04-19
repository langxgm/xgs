//------------------------------------------------------------------------
// * @filename: from_ws_session.h
// *
// * @brief: 来自WS的连接
// *
// * @author: XGM
// * @date: 2017/11/14
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/ServerWorker.h"

class From_Ws_Session : public ServerWorker, public Singleton<From_Ws_Session>
{
protected:
	friend class Singleton<From_Ws_Session>;
	From_Ws_Session();
	virtual ~From_Ws_Session();
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
