//------------------------------------------------------------------------
// * @filename: User.h
// *
// * @brief: 用户
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include <string>
#include <stdint.h>

class User
{
public:
	User();
	virtual ~User();
public:
	//------------------------------------------------------------------------
	// 初始化
	//------------------------------------------------------------------------
	virtual void Init(int64_t nUserID, int64_t nLinkSessionID);

	//------------------------------------------------------------------------
	// 重连
	//------------------------------------------------------------------------
	virtual void Reconnect(int64_t nLinkSessionID);

	//------------------------------------------------------------------------
	// 上线
	//------------------------------------------------------------------------
	virtual void Online();

	//------------------------------------------------------------------------
	// 下线
	//------------------------------------------------------------------------
	virtual void Offline();

	//------------------------------------------------------------------------
	// 获取UserID
	//------------------------------------------------------------------------
	int64_t GetUserID() { return m_nUserID; }

	//------------------------------------------------------------------------
	// 获取用户在哪个网关
	//------------------------------------------------------------------------
	int64_t GetLinkSessionID() { return m_nLinkSessionID; }

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

private:
	// UserID
	int64_t m_nUserID = 0;

	// 用户在哪个网关服务器
	int64_t m_nLinkSessionID = 0;

	// 登陆Key,重连验证
	std::string m_strLoginKey;

	// 在线状态
	bool m_bOnline = false;

	// openid
	std::string m_strOpenID;

	// session_key
	std::string m_strSessionKey;
};
