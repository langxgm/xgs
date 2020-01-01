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

struct UserInfo
{
	int64_t nUserID = 0;
	std::string strAccount;
	std::string strDeviceID;
	std::string strIP;
	std::string strOpenID;
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
	// 监听Link的消息
	//------------------------------------------------------------------------
	void ListenFromLink(ServerWorker* pServer);

	//------------------------------------------------------------------------
	// 监听Sdk的消息
	//------------------------------------------------------------------------
	void ListenFromSdk(ClientWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 登陆
	//------------------------------------------------------------------------
	static bool Login(UserInfo& rUserInfo);

	//------------------------------------------------------------------------
	// 查询用户账号
	//------------------------------------------------------------------------
	static bool QueryAccount(int64_t nUserID, std::string& rAccount);

	//------------------------------------------------------------------------
	// 通过openid查询用户UserID
	//------------------------------------------------------------------------
	static bool QueryUserIDByOpenID(const std::vector<std::string>& vecOpenID,
		const std::function<void(std::string&&, int64_t)>& func);

	//------------------------------------------------------------------------
	// 通过用户UserID查询openid
	//------------------------------------------------------------------------
	static bool QueryOpenIDByUserID(const std::vector<int64_t>& vecUserID,
		const std::function<void(std::string&&, int64_t)>& func);

	//------------------------------------------------------------------------
	// 用户进入
	//------------------------------------------------------------------------
	void LinkUser(int64_t nUserID, int64_t nSessionID, const std::string& strOpenID,
		const std::string& strSessionKey, const std::string& strLoginKey);

public:
	void HandleCLLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLLoginAccount(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleSDK2LLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLLoginFacebook(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleCLReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

	void HandleLink2LDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
