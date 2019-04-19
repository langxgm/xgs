//------------------------------------------------------------------------
// * @filename: UserManager.h
// *
// * @brief: 用户管理
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"

#include <string>
#include <memory>
#include <stdint.h>

class User;
typedef std::shared_ptr<User> UserPtr;
typedef std::unique_ptr<User> UserUPtr;

class UserMapping;
typedef std::unique_ptr<UserMapping> UserMappingPtr;

class UserFactory;
typedef std::unique_ptr<UserFactory> UserFactoryPtr;

class UserManager : public Singleton<UserManager>
{
protected:
	friend class Singleton<UserManager>;
	UserManager();
	virtual ~UserManager();
public:
	//------------------------------------------------------------------------
	// 初始化
	//------------------------------------------------------------------------
	virtual void Init(uint32_t nUserLimit);

	//------------------------------------------------------------------------
	// 创建用户的工厂
	//------------------------------------------------------------------------
	virtual UserFactory* CreateUserFactory();

	//------------------------------------------------------------------------
	// 获得用户工厂
	//------------------------------------------------------------------------
	UserFactory* GetUserFactory();

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

	template<typename T>
	std::shared_ptr<T> GetUser_cast(int64_t nUserID)
	{
		return std::static_pointer_cast<T>(GetUser(nUserID));
	}

	template<typename T>
	std::shared_ptr<T> GetUserByOpenID_cast(const std::string& strOpenID)
	{
		return std::static_pointer_cast<T>(GetUserByOpenID(strOpenID));
	}

	template<typename T>
	std::shared_ptr<T> GetUserByLoginKey_cast(const std::string& strLoginKey)
	{
		return std::static_pointer_cast<T>(GetUserByLoginKey(strLoginKey));
	}

	//------------------------------------------------------------------------
	// 修改映射关系
	//------------------------------------------------------------------------
	void ModifyMapping(const UserPtr& pUser, const std::string& strOpenID,
		const std::string& strSessionKey, const std::string& strLoginKey);

public:
	//------------------------------------------------------------------------
	// 获取用户数量
	//------------------------------------------------------------------------
	uint32_t GetUserNum();

	//------------------------------------------------------------------------
	// 获取用户数量上限
	//------------------------------------------------------------------------
	uint32_t GetUserLimit() { return m_nUserLimit; }

public:
	// 用户的映射表
	UserMappingPtr m_pUserMapping;

	// 创建用户的工厂
	UserFactoryPtr m_pUserFactory;

	// 用户人数限制
	uint32_t m_nUserLimit = 0;
};
