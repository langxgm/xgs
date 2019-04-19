//------------------------------------------------------------------------
// * @filename: LinkManager.h
// *
// * @brief: 用户连接管理
// *
// * @author: XGM
// * @date: 2018/12/27
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xbase/TimeChecker.h"

#include <map>
#include <memory>
#include <stdint.h>

class UserLink;
typedef std::shared_ptr<UserLink> UserLinkPtr;

class UserLinkManager : public Singleton<UserLinkManager>
{
protected:
	friend class Singleton<UserLinkManager>;
	UserLinkManager();
	virtual ~UserLinkManager();
public:
	//------------------------------------------------------------------------
	// 初始化
	//------------------------------------------------------------------------
	virtual void Init(uint32_t nUserLimit);

	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	void Update();

	//------------------------------------------------------------------------
	// 创建用户
	//------------------------------------------------------------------------
	UserLinkPtr NewUser();

	//------------------------------------------------------------------------
	// 增加用户
	//------------------------------------------------------------------------
	bool AddUser(int64_t nUserID, const UserLinkPtr& pUser);

	//------------------------------------------------------------------------
	// 删除用户
	//------------------------------------------------------------------------
	bool DelUser(int64_t nUserID);
	void DelUser(const UserLinkPtr& pUser);

	//------------------------------------------------------------------------
	// 获取用户
	//------------------------------------------------------------------------
	UserLinkPtr GetUser(int64_t nUserID);
	UserLinkPtr GetUserBySessionID(int64_t nSessionID);

	//------------------------------------------------------------------------
	// 修改SessionID
	//------------------------------------------------------------------------
	void ModifySessionID(int64_t nOldSessionID, const UserLinkPtr& pUser);

public:
	//------------------------------------------------------------------------
	// 获取用户数量
	//------------------------------------------------------------------------
	uint32_t GetUserNum();

	//------------------------------------------------------------------------
	// 获取用户数量上限
	//------------------------------------------------------------------------
	uint32_t GetUserLimit() { return m_nUserLimit; }

private:
	// 用户列表管理<UserID, 用户>
	std::map<int64_t, UserLinkPtr> m_mapUsers;

	// 用户映射<SessionID, 用户>
	std::map<int64_t, UserLinkPtr> m_mapSessionIDUsers;

	// 用户人数限制
	uint32_t m_nUserLimit = 0;

	// 心跳检测定时器
	TimeChecker m_HeartbeatTimer;
};
