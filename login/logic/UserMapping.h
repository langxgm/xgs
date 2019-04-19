//------------------------------------------------------------------------
// * @filename: UserMapping.h
// *
// * @brief: 用户的映射表
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdint.h>

class User;
typedef std::shared_ptr<User> UserPtr;

class UserMapping
{
public:
	UserMapping();
	~UserMapping();

	//------------------------------------------------------------------------
	// 增加用户
	//------------------------------------------------------------------------
	bool AddUser(int64_t nUserID, const UserPtr& pUser);

	//------------------------------------------------------------------------
	// 删除用户
	//------------------------------------------------------------------------
	bool DelUser(int64_t nUserID);

	//------------------------------------------------------------------------
	// 获取用户
	//------------------------------------------------------------------------
	UserPtr GetUser(int64_t nUserID);
	UserPtr GetUserByOpenID(const std::string& strOpenID);
	UserPtr GetUserByLoginKey(const std::string& strLoginKey);

	//------------------------------------------------------------------------
	// 获得用户人数
	//------------------------------------------------------------------------
	uint32_t GetUserNum();

private:
	// 用户列表管理<UserID, 用户>
	std::map<int64_t, UserPtr> m_mapUsers;

	// 用户映射<openid, 用户>
	std::map<std::string, UserPtr> m_mapOpenIDUsers;

	// 用户映射<login_key, 用户>
	std::map<std::string, UserPtr> m_mapLoginKeyUsers;
};
