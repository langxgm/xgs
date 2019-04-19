//------------------------------------------------------------------------
// * @filename: GameDataHandler.h
// *
// * @brief: 游戏数据处理
// *
// * @author: XGM
// * @date: 2017/09/14
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

#include <vector>
#include <functional>

class ServerWorker;

class GameDataHandler : public LogicHandler, public DBHandler, public Singleton<GameDataHandler>
{
protected:
	friend class Singleton<GameDataHandler>;
	GameDataHandler() {}
	virtual ~GameDataHandler() {}
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(GameDataHandler); }

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
	// 查询保存的关卡数据
	//------------------------------------------------------------------------
	static bool QuerySaveLevelData(int64_t nPlayerGUID, int32_t& rLevelNum, std::string& rLevelData);

	//------------------------------------------------------------------------
	// 查询多个玩家的关卡数据
	//------------------------------------------------------------------------
	static bool QueryMultiSaveLevelData(const std::vector<int64_t>& rGUIDs,
		const std::function<void(int64_t, int32_t, std::string&&)>& func);

	//------------------------------------------------------------------------
	// 随机玩家列表
	//------------------------------------------------------------------------
	static bool RandomPlayerList(int64_t nPlayerGUID, std::vector<int64_t>& rRandomList, uint32_t nLimit,
		const std::vector<int64_t>& rExcludes);

public:
	void HandleCGSaveLevelData(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
