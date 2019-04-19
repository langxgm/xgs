//------------------------------------------------------------------------
// * @filename: LoginHandler.h
// *
// * @brief: 登陆逻辑处理
// *
// * @author: XGM
// * @date: 2017/11/16
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

class ServerWorker;

class LoginHandler : public LogicHandler, public DBHandler, public Singleton<LoginHandler>
{
protected:
	friend class Singleton<LoginHandler>;
	LoginHandler() {}
	virtual ~LoginHandler() {}
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(LoginHandler); }

	//------------------------------------------------------------------------
	// 初始化数据库
	//------------------------------------------------------------------------
	virtual void PreInitDatabase() override;

public:
	//------------------------------------------------------------------------
	// 监听Ls的消息
	//------------------------------------------------------------------------
	void ListenFromLs(ServerWorker* pServer);

public:
	void HandleL2SDKLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
