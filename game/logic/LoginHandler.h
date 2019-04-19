//------------------------------------------------------------------------
// * @filename: LoginHandler.h
// *
// * @brief: 登陆逻辑处理
// *
// * @author: XGM
// * @date: 2017/08/24
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

#include <vector>
#include <functional>

class ServerWorker;
class ClientWorker;

struct LoginPlayerInfo
{
	int64_t nPlayerGUID = 0;
	std::string strPlayerName;
	int32_t nPortrait = 0;
	int32_t nPlayerLevel = 0;
	std::string strDeviceID;
	std::string strIP;
};

class LoginHandler : public LogicHandler, public DBHandler, public Singleton<LoginHandler>
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
	// 初始化数据库
	//------------------------------------------------------------------------
	virtual void PreInitDatabase() override;

	//------------------------------------------------------------------------
	// 更新逻辑
	//------------------------------------------------------------------------
	virtual void UpdateLogic() override;

public:
	//------------------------------------------------------------------------
	// 监听Gws的消息
	//------------------------------------------------------------------------
	void ListenFromGws(ServerWorker* pServer);

	//------------------------------------------------------------------------
	// 监听Ws的消息
	//------------------------------------------------------------------------
	void ListenFromWs(ClientWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 登陆
	//------------------------------------------------------------------------
	static bool LoginGame(LoginPlayerInfo& rInfo);

	//------------------------------------------------------------------------
	// 查询玩家名称
	//------------------------------------------------------------------------
	static bool QueryPlayerName(int64_t nPlayerGUID, std::string& rPlayerName, int32_t& rPortraitID);

	//------------------------------------------------------------------------
	// 查询多个玩家名称
	//------------------------------------------------------------------------
	static bool QueryMultiPlayerName(const std::vector<int64_t>& rGUIDs,
		const std::function<void(int64_t, std::string&&, int32_t)>& func);

	//------------------------------------------------------------------------
	// 玩家进入
	//------------------------------------------------------------------------
	void LinkPlayer(int64_t nPlayerGUID, int64_t nSessionID, const std::string& strLoginKey);

public:
	void HandleCGLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleWGLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCGReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleGWS2GDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCGChangeName(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
