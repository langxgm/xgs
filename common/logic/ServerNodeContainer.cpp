
#include "ServerNodeContainer.h"
#include "ServerNode.h"

#include "xbase/TimeUtil.h"
#include "xbase/Random.h"

#include <assert.h>

ServerNodeContainer::ServerNodeContainer()
{

}

ServerNodeContainer::~ServerNodeContainer()
{
	m_mapGroupNode.clear();
	m_mapIDNode.clear();
	m_mapSessionNode.clear();
}

void ServerNodeContainer::Update()
{
	for (auto& it : m_mapIDNode)
	{
		auto& pNode = it.second;
		pNode->Update();
	}
}

bool ServerNodeContainer::AddServer(const ServerNodePtr& pNode)
{
	if (m_mapSessionNode.find(pNode->GetSessionID()) != m_mapSessionNode.end())
	{
		return false;
	}
	if (m_mapIDNode.find(pNode->GetID()) != m_mapIDNode.end())
	{
		return false;
	}

	m_mapSessionNode[pNode->GetSessionID()] = pNode;
	m_mapIDNode[pNode->GetID()] = pNode;
	m_mapGroupNode[pNode->GetGroup()].push_back(pNode);
	return true;
}

bool ServerNodeContainer::DelServer(const ServerNodePtr& pNode)
{
	if (pNode)
	{
		m_mapSessionNode.erase(pNode->GetSessionID());
		m_mapIDNode.erase(pNode->GetID());

		auto it = m_mapGroupNode.find(pNode->GetGroup());
		if (it != m_mapGroupNode.end())
		{
			auto& vec = it->second;
			for (auto it = vec.begin(); it != vec.end(); ++it)
			{
				if ((*it)->GetID() == pNode->GetID())
				{
					vec.erase(it);
					break;
				}
			}
			if (vec.size() == 0)
			{
				m_mapGroupNode.erase(it);
			}
		}
		return true;
	}
	return false;
}

bool ServerNodeContainer::DelServerByID(int32_t nID)
{
	return DelServer(GetServerByID(nID));
}

bool ServerNodeContainer::DelServerBySessionID(int64_t nSessionID)
{
	return DelServer(GetServerBySessionID(nSessionID));
}

uint32_t ServerNodeContainer::GetServerNum()
{
	assert(m_mapIDNode.size() == m_mapSessionNode.size());
	return m_mapIDNode.size();
}

uint32_t ServerNodeContainer::GetServerNumInGroup(int32_t nGroup)
{
	auto it = m_mapGroupNode.find(nGroup);
	if (it != m_mapGroupNode.end())
	{
		return it->second.size();
	}
	return 0;
}

ServerNodePtr ServerNodeContainer::GetServerByID(int32_t nID)
{
	auto it = m_mapIDNode.find(nID);
	if (it != m_mapIDNode.end())
	{
		return it->second;
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::GetServerBySessionID(int64_t nSessionID)
{
	auto it = m_mapSessionNode.find(nSessionID);
	if (it != m_mapSessionNode.end())
	{
		return it->second;
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::GetServerByIndex(uint32_t nIndex)
{
	if (nIndex < m_mapIDNode.size())
	{
		auto it = m_mapIDNode.begin();
		while (nIndex--)
		{
			++it;
		}
		return it->second;
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::GetServerByIndex(int32_t nGroup, uint32_t nIndex)
{
	auto it = m_mapGroupNode.find(nGroup);
	if (it != m_mapGroupNode.end())
	{
		auto& vec = it->second;
		if (nIndex < vec.size())
		{
			return vec[nIndex];
		}
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::RandomServer()
{
	uint32_t nNum = GetServerNum();
	if (nNum > 0)
	{
		Random<uint32_t> r(TimeUtil::GetCurrentTimeMillis());
		uint32_t nIndex = r.rand(0, nNum - 1);
		return GetServerByIndex(nIndex);
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::RandomServerInGroup(int32_t nGroup)
{
	uint32_t nNum = GetServerNumInGroup(nGroup);
	if (nNum > 0)
	{
		Random<uint32_t> r(TimeUtil::GetCurrentTimeMillis());
		uint32_t nIndex = r.rand(0, nNum - 1);
		return GetServerByIndex(nGroup, nIndex);
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::GetServerByService(uint32_t nStep, const std::string& strService)
{
	if (m_mapIDNode.size() == 0)
	{
		return nullptr;
	}

	uint32_t nEnd = nStep % m_mapIDNode.size();
	auto it = m_mapIDNode.begin();
	auto ret = m_mapIDNode.end();
	do
	{
		auto& node = it->second;
		if (node->CheckServiceCond(strService, {}))
		{
			ret = it;
		}
		++it;
	} while (nEnd--);

	return ret != m_mapIDNode.end() ? ret->second : nullptr;
}

ServerNodePtr ServerNodeContainer::GetServerByService(int32_t nGroup, uint32_t nStep, const std::string& strService)
{
	auto itGroup = m_mapGroupNode.find(nGroup);
	if (itGroup == m_mapGroupNode.end())
	{
		return nullptr;
	}

	auto& vecNode = itGroup->second;
	if (vecNode.size() == 0)
	{
		return nullptr;
	}

	uint32_t nEnd = nStep % vecNode.size();
	auto it = vecNode.begin();
	auto ret = vecNode.end();
	do
	{
		auto& node = *it;
		if (node->CheckServiceCond(strService, {}))
		{
			ret = it;
		}
		++it;
	} while (nEnd--);

	return ret != vecNode.end() ? *ret : nullptr;
}

ServerNodePtr ServerNodeContainer::RandomServerByService(const std::string& strService)
{
	uint32_t nNum = GetServerNum();
	if (nNum > 0)
	{
		uint32_t nIndex = GetCycleIndex()++;
		return GetServerByService(nIndex, strService);
	}
	return nullptr;
}

ServerNodePtr ServerNodeContainer::RandomServerInGroupByService(int32_t nGroup, const std::string& strService)
{
	uint32_t nNum = GetServerNumInGroup(nGroup);
	if (nNum > 0)
	{
		uint32_t nIndex = GetCycleIndex()++;
		return GetServerByService(nGroup, nIndex, strService);
	}
	return nullptr;
}
