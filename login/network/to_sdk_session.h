//------------------------------------------------------------------------
// * @filename: to_sdk_session.h
// *
// * @brief: 连接SDK服务器
// *
// * @author: XGM
// * @date: 2017/11/20
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xbase/TimeChecker.h"
#include "xshare/net/ClientWorker.h"

class To_Sdk_Session : public ClientWorker, public Singleton<To_Sdk_Session>
{
protected:
	friend class Singleton<To_Sdk_Session>;
	To_Sdk_Session() {}
	virtual ~To_Sdk_Session() {}
public:
	//------------------------------------------------------------------------
	// 注册监听的消息
	//------------------------------------------------------------------------
	virtual void RegisterListen() override;

	//------------------------------------------------------------------------
	// 每帧执行消息
	//------------------------------------------------------------------------
	virtual void Update() override;

	//------------------------------------------------------------------------
	// 连接成功(异步的)
	//------------------------------------------------------------------------
	virtual void OnConnected(int64_t nSessionID) override;

public:
	void SendPing();
	void HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

private:
	// Ping计时器
	TimeChecker m_PingTimer;
};
