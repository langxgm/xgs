//------------------------------------------------------------------------
// * @filename: LoginHandler.h
// *
// * @brief: 登陆逻辑处理
// *
// * @author: XGM
// * @date: 2018/05/10
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

class ServerWorker;
class ClientWorker;

class LoginHandler : public LogicHandler, public Singleton<LoginHandler>
{
protected:
	friend class Singleton<LoginHandler>;
	LoginHandler();
	virtual ~LoginHandler();
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(LoginHandler); }

	//------------------------------------------------------------------------
	// 更新逻辑
	//------------------------------------------------------------------------
	virtual void UpdateLogic() override;

public:
	//------------------------------------------------------------------------
	// 监听客户端的消息
	//------------------------------------------------------------------------
	void ListenFromClient(ServerWorker* pServer);

	//------------------------------------------------------------------------
	// 监听Gs的消息
	//------------------------------------------------------------------------
	void ListenFromGs(ClientWorker* pServer);

	//------------------------------------------------------------------------
	// 监听Ws的消息
	//------------------------------------------------------------------------
	void ListenFromWs(ClientWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 登陆网关
	//------------------------------------------------------------------------
	void LoginGate(int64_t nPlayerGUID, int64_t nSessionID);

public:
	void HandleCGLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleGCLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCGReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleGCReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleW2GWSReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleG2GWSDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleW2GWSDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleG2GWSKickPlayer(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleW2GWSKickPlayer(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
