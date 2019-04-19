//------------------------------------------------------------------------
// * @filename: UserHandler.h
// *
// * @brief: 用户逻辑处理
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

class UserHandler : public LogicHandler, public Singleton<UserHandler>
{
protected:
	friend class Singleton<UserHandler>;
	UserHandler();
	virtual ~UserHandler();
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(UserHandler); }

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
	void HandleCLServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLCServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
