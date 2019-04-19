
#include "UserLinkManager.h"
#include "UserLink.h"

#include "xbase/TimeUtil.h"

#include <glog/logging.h>

#include <assert.h>

UserLinkManager::UserLinkManager()
{

}

UserLinkManager::~UserLinkManager()
{
	m_mapSessionIDUsers.clear();
	m_mapUsers.clear();
}

void UserLinkManager::Init(uint32_t nUserLimit)
{
	m_nUserLimit = nUserLimit;
}

void UserLinkManager::Update()
{
	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_HeartbeatTimer.IsStart() == false)
	{
		m_HeartbeatTimer.Start(nNow, 5 * 1000);
	}
	if (m_HeartbeatTimer.IsTimeout(nNow))
	{
		for (auto it = m_mapUsers.begin(); it != m_mapUsers.end();)
		{
			auto& pUser = it->second;
			if (pUser->CheckHeartbeat(nNow))
			{
				++it;
			}
			else
			{
				auto holdUser = pUser;

				m_mapSessionIDUsers.erase(pUser->GetSessionID());
				it = m_mapUsers.erase(it);

				holdUser->OnDisconnect();

				LOG(INFO) << "KickUser heartbeat timeout userid=" << holdUser->GetUserID()
					<< " sid=" << holdUser->GetSessionID();
			}
		}
	}
}

UserLinkPtr UserLinkManager::NewUser()
{
	return UserLinkPtr(new UserLink());
}

bool UserLinkManager::AddUser(int64_t nUserID, const UserLinkPtr& pUser)
{
	if (GetUserNum() >= GetUserLimit())
	{
		return false; // 人满了
	}

	assert(nUserID == pUser->GetUserID());

	if (m_mapUsers.find(nUserID) == m_mapUsers.end())
	{
		auto itSIDUser = m_mapSessionIDUsers.find(pUser->GetSessionID());
		if (itSIDUser == m_mapSessionIDUsers.end())
		{
			m_mapUsers.insert(std::make_pair(nUserID, pUser));
			m_mapSessionIDUsers.insert(std::make_pair(pUser->GetSessionID(), pUser));
			return true;
		}
	}
	return false;
}

bool UserLinkManager::DelUser(int64_t nUserID)
{
	auto pUser = GetUser(nUserID);
	if (pUser)
	{
		m_mapUsers.erase(pUser->GetUserID());
		m_mapSessionIDUsers.erase(pUser->GetSessionID());
	}
	return false;
}

void UserLinkManager::DelUser(const UserLinkPtr& pUser)
{
	if (pUser)
	{
		m_mapUsers.erase(pUser->GetUserID());
		m_mapSessionIDUsers.erase(pUser->GetSessionID());
	}
}

UserLinkPtr UserLinkManager::GetUser(int64_t nUserID)
{
	auto it = m_mapUsers.find(nUserID);
	if (it != m_mapUsers.end())
	{
		return it->second;
	}
	return nullptr;
}

UserLinkPtr UserLinkManager::GetUserBySessionID(int64_t nSessionID)
{
	auto it = m_mapSessionIDUsers.find(nSessionID);
	if (it != m_mapSessionIDUsers.end())
	{
		return it->second;
	}
	return nullptr;
}

void UserLinkManager::ModifySessionID(int64_t nOldSessionID, const UserLinkPtr& pUser)
{
	m_mapSessionIDUsers.erase(nOldSessionID);
	m_mapSessionIDUsers.insert(std::make_pair(pUser->GetSessionID(), pUser));
}

uint32_t UserLinkManager::GetUserNum()
{
	return m_mapUsers.size();
}
