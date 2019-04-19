//------------------------------------------------------------------------
// * @filename: FeedbackHandler.h
// *
// * @brief: 玩家意见反馈
// *
// * @author: XGM
// * @date: 2018/02/23
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

class ServerWorker;

class FeedbackHandler : public LogicHandler, public DBHandler, public Singleton<FeedbackHandler>
{
protected:
	friend class Singleton<FeedbackHandler>;
	FeedbackHandler() {}
	virtual ~FeedbackHandler() {}
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(FeedbackHandler); }

	//------------------------------------------------------------------------
	// 初始化数据库
	//------------------------------------------------------------------------
	virtual void PreInitDatabase() override;

public:
	//------------------------------------------------------------------------
	// 监听Gws的消息
	//------------------------------------------------------------------------
	void ListenFromGws(ServerWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 增加玩家意见反馈
	//------------------------------------------------------------------------
	static bool AddFeedback(int64_t nPlayerGUID, const std::string& strContent);

public:
	void HandleCGFeedback(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
