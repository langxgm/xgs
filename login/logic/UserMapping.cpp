
#include "UserMapping.h"
#include "User.h"

UserMapping::UserMapping()
{

}

UserMapping::~UserMapping()
{
	m_mapLoginKeyUsers.clear();
	m_mapOpenIDUsers.clear();
	m_mapUsers.clear();
}

bool UserMapping::AddUser(int64_t nUserID, const UserPtr& pUser)
{
	if (m_mapUsers.find(nUserID) == m_mapUsers.end())
	{
		m_mapUsers.insert(std::make_pair(pUser->GetUserID(), pUser));
		m_mapOpenIDUsers.insert(std::make_pair(pUser->GetOpenID(), pUser));
		m_mapLoginKeyUsers.insert(std::make_pair(pUser->GetLoginKey(), pUser));
		return true;
	}
	return false;
}

bool UserMapping::DelUser(int64_t nUserID)
{
	auto pUser = GetUser(nUserID);
	if (pUser)
	{
		m_mapUsers.erase(pUser->GetUserID());
		m_mapOpenIDUsers.erase(pUser->GetOpenID());
		m_mapLoginKeyUsers.erase(pUser->GetLoginKey());
		return true;
	}
	return false;
}

UserPtr UserMapping::GetUser(int64_t nUserID)
{
	auto it = m_mapUsers.find(nUserID);
	if (it != m_mapUsers.end())
	{
		return it->second;
	}
	return nullptr;
}

UserPtr UserMapping::GetUserByOpenID(const std::string& strOpenID)
{
	auto it = m_mapOpenIDUsers.find(strOpenID);
	if (it != m_mapOpenIDUsers.end())
	{
		return it->second;
	}
	return nullptr;
}

UserPtr UserMapping::GetUserByLoginKey(const std::string& strLoginKey)
{
	auto it = m_mapLoginKeyUsers.find(strLoginKey);
	if (it != m_mapLoginKeyUsers.end())
	{
		return it->second;
	}
	return nullptr;
}

uint32_t UserMapping::GetUserNum()
{
	return m_mapUsers.size();
}
