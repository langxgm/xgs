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
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

class ServerWorker;

class UserHandler : public LogicHandler, public DBHandler, public Singleton<UserHandler>
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
	// 初始化数据库
	//------------------------------------------------------------------------
	virtual void PreInitDatabase() override;

	//------------------------------------------------------------------------
	// 更新逻辑
	//------------------------------------------------------------------------
	virtual void UpdateLogic() override;

public:
	//------------------------------------------------------------------------
	// 监听Link的消息
	//------------------------------------------------------------------------
	void ListenFromLink(ServerWorker* pServer);

public:
	void HandleCLServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
