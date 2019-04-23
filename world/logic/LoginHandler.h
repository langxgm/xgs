//------------------------------------------------------------------------
// * @filename: LoginHandler.h
// *
// * @brief: 登陆逻辑处理
// *
// * @author: XGM
// * @date: 2017/09/11
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

struct PlayerOfUserInfo
{
	int64_t nUserID = 0;
	int32_t nServerID = 0;
	int64_t nPlayerGUID = 0;
	std::string strPlayerName;
	int32_t nPortrait = 0;
	int32_t nPlayerLevel = 0;
	std::string strDeviceID;
	std::string strIP;
};

struct LittleUserInfo
{
	int64_t nUserID = 0;
	int64_t nLoginTime = 0;
	std::string strToken;
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
	// 监听Gs的消息
	//------------------------------------------------------------------------
	void ListenFromGs(ServerWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 登陆
	//------------------------------------------------------------------------
	static bool LoginWorld(PlayerOfUserInfo& rUserInfo);

	//------------------------------------------------------------------------
	// 查询PlayerGUID
	//------------------------------------------------------------------------
	static bool QueryPlayerGUID(int64_t nUserID, int32_t nServerID, int64_t& rPlayerGUID);

	//------------------------------------------------------------------------
	// 查询用户信息
	//------------------------------------------------------------------------
	static bool QueryUserInfo(int64_t nUserID, LittleUserInfo& rInfo);

	//------------------------------------------------------------------------
	// 玩家进入
	//------------------------------------------------------------------------
	void LinkPlayer(int64_t nPlayerGUID, int64_t nSessionID, const std::string& strOpenID,
		const std::string& strSessionKey, const std::string& strLoginKey);

public:
	void HandleGWLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCWReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleGWS2WReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleGWS2WDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
