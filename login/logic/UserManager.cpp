
#include "UserManager.h"
#include "UserMapping.h"
#include "UserConcreteFactory.h"
#include "User.h"

UserManager::UserManager()
{

}

UserManager::~UserManager()
{
	m_pUserFactory.reset();
	m_pUserMapping.reset();
}

void UserManager::Init(uint32_t nUserLimit)
{
	m_pUserMapping.reset(new UserMapping());
	m_pUserFactory.reset(CreateUserFactory());

	m_nUserLimit = nUserLimit;
}

UserFactory* UserManager::CreateUserFactory()
{
	return new UserDefaultFactory();
}

UserFactory* UserManager::GetUserFactory()
{
	return m_pUserFactory.get();
}

bool UserManager::AddUser(int64_t nUserID, const UserPtr& pUser)
{
	if (GetUserNum() >= GetUserLimit())
	{
		return false; // 人满了
	}
	return m_pUserMapping->AddUser(nUserID, pUser);
}

bool UserManager::DelUser(int64_t nUserID)
{
	return m_pUserMapping->DelUser(nUserID);
}

UserPtr UserManager::GetUser(int64_t nUserID)
{
	return m_pUserMapping->GetUser(nUserID);
}

UserPtr UserManager::GetUserByOpenID(const std::string& strOpenID)
{
	return m_pUserMapping->GetUserByOpenID(strOpenID);
}

UserPtr UserManager::GetUserByLoginKey(const std::string& strLoginKey)
{
	return m_pUserMapping->GetUserByLoginKey(strLoginKey);
}

void UserManager::ModifyMapping(const UserPtr& pUser, const std::string& strOpenID,
	const std::string& strSessionKey, const std::string& strLoginKey)
{
	m_pUserMapping->DelUser(pUser->GetUserID());

	pUser->SetOpenID(strOpenID);
	pUser->SetSessionKey(strSessionKey);

	pUser->SetLoginKey(strLoginKey);

	m_pUserMapping->AddUser(pUser->GetUserID(), pUser);
}

uint32_t UserManager::GetUserNum()
{
	return m_pUserMapping->GetUserNum();
}
