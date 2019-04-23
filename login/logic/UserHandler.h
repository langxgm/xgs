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

#include <map>

class ServerWorker;

struct LittlePlayerOfUserInfo
{
	int64_t nUserID = 0;
	int32_t nServerID = 0;
	int64_t nPlayerGUID = 0;
	std::string strPlayerName;
	int32_t nPortrait = 0;
	int32_t nPlayerLevel = 0;
};

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
	//------------------------------------------------------------------------
	// 查询玩家信息
	//------------------------------------------------------------------------
	static bool QueryPlayerOfUserInfo(int64_t nUserID, std::map<int32_t, LittlePlayerOfUserInfo>& rPlayers);

public:
	void HandleCLServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
