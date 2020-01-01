//------------------------------------------------------------------------
// * @filename: LoginHandler.h
// *
// * @brief: 登陆逻辑处理
// *
// * @author: XGM
// * @date: 2018/12/27
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
	// 监听Ls的消息
	//------------------------------------------------------------------------
	void ListenFromLs(ClientWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 登陆网关
	//------------------------------------------------------------------------
	void LoginLink(int64_t nUserID, int64_t nSessionID);

public:
	void HandleCLLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLCLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLLoginAccount(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLCLoginAccount(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLCLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLLoginFacebook(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLCLoginFacebook(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLCReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleL2LinkDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleL2LinkKickPlayer(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
