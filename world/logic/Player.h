//------------------------------------------------------------------------
// * @filename: Player.h
// *
// * @brief: 玩家
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include "xgame/gameplayer/GamePlayer.h"

#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

// 玩家中转的数据
struct PlayerClientInfo
{
	std::string strPlayerName; // 玩家的名字
};
typedef std::unique_ptr<PlayerClientInfo> PlayerClientInfoUPtr;

class Player : public GamePlayer
{
public:
	Player();
	virtual ~Player();
public:
	//------------------------------------------------------------------------
	// 初始化
	//------------------------------------------------------------------------
	virtual void Init(int64_t nPlayerGUID, int64_t nGwsSessionID);

	//------------------------------------------------------------------------
	// 重连
	//------------------------------------------------------------------------
	virtual void Reconnect(int64_t nGwsSessionID);

	//------------------------------------------------------------------------
	// 上线
	//------------------------------------------------------------------------
	virtual void Online();

	//------------------------------------------------------------------------
	// 下线
	//------------------------------------------------------------------------
	virtual void Offline();

	//------------------------------------------------------------------------
	// 获取玩家GUID
	//------------------------------------------------------------------------
	//int64_t GetPlayerGUID() { return m_nPlayerGUID; }

	//------------------------------------------------------------------------
	// 获取玩家在哪个网关
	//------------------------------------------------------------------------
	int64_t GetGwsSessionID() { return m_nSessionID; }

	//------------------------------------------------------------------------
	// 登陆Key
	//------------------------------------------------------------------------
	const std::string& GetLoginKey() const { return m_strLoginKey; }
	void SetLoginKey(const std::string& strLoginKey) { m_strLoginKey = strLoginKey; }

	//------------------------------------------------------------------------
	// 是否在线
	//------------------------------------------------------------------------
	bool IsOnline() const { return m_bOnline; }
	bool IsOffline() const { return !m_bOnline; }

	//------------------------------------------------------------------------
	// openid
	//------------------------------------------------------------------------
	const std::string& GetOpenID() const { return m_strOpenID; }
	void SetOpenID(const std::string& strOpenID) { m_strOpenID = strOpenID; }

	//------------------------------------------------------------------------
	// session_key
	//------------------------------------------------------------------------
	const std::string& GetSessionKey() const { return m_strSessionKey; }
	void SetSessionKey(const std::string& strSessionKey) { m_strSessionKey = strSessionKey; }

	//------------------------------------------------------------------------
	// 获得客户端数据
	//------------------------------------------------------------------------
	PlayerClientInfoUPtr& GetClientInfo() { return m_pClientInfo; }

private:
	// PlayerGUID
	//int64_t m_nPlayerGUID = 0;

	// 玩家在哪个网关服务器
	//int64_t m_nGwsSessionID = 0;

	// 登陆Key,重连验证
	std::string m_strLoginKey;

	// 在线状态
	bool m_bOnline = false;

	// openid
	std::string m_strOpenID;

	// session_key
	std::string m_strSessionKey;

	// 客户端数据
	PlayerClientInfoUPtr m_pClientInfo;
};
