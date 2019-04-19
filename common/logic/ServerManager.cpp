
#include "ServerManager.h"
#include "ServerNodeContainer.h"

#include <assert.h>

ServerManager::ServerManager()
{
	for (auto& v : m_Servers)
	{
		v.reset(new ServerNodeContainer());
	}
}

ServerManager::~ServerManager()
{

}

void ServerManager::Update(ServerType nType)
{
	auto pNodeContainer = MutableServers(nType);

	pNodeContainer->Update();
}

bool ServerManager::AddServer(ServerType nType, const ServerNodePtr& pNode)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->AddServer(pNode);
}

bool ServerManager::DelServerByID(ServerType nType, int32_t nID)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->DelServerByID(nID);
}

bool ServerManager::DelServerBySessionID(ServerType nType, int64_t nSessionID)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->DelServerBySessionID(nSessionID);
}

uint32_t ServerManager::GetServerNum(ServerType nType)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->GetServerNum();
}

uint32_t ServerManager::GetServerNumInGroup(ServerType nType, int32_t nGroup)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->GetServerNumInGroup(nGroup);
}

uint32_t ServerManager::GetAllServerNum()
{
	uint32_t nNum = 0;
	for (int32_t i = 0; i < static_cast<int32_t>(ServerType::Max); ++i)
	{
		nNum += GetServerNum(static_cast<ServerType>(i));
	}
	return nNum;
}

ServerNodeContainerPtr ServerManager::GetServers(ServerType nType)
{
	auto pNodeContainer = MutableServers(nType);
	return pNodeContainer;
}

ServerNodeContainerPtr ServerManager::MutableServers(ServerType nType)
{
	int32_t i = static_cast<int32_t>(nType);
	assert(i >= 0 && i < static_cast<int32_t>(ServerType::Max));
	return m_Servers[i];
}

ServerNodePtr ServerManager::GetServerByID(ServerType nType, int32_t nID)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->GetServerByID(nID);
}

ServerNodePtr ServerManager::GetServerBySessionID(ServerType nType, int64_t nSessionID)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->GetServerBySessionID(nSessionID);
}

ServerNodePtr ServerManager::GetServerByIndex(ServerType nType, uint32_t nIndex)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->GetServerByIndex(nIndex);
}

ServerNodePtr ServerManager::RandomServer(ServerType nType)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->RandomServer();
}

ServerNodePtr ServerManager::RandomServerInGroup(ServerType nType, int32_t nGroup)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->RandomServerInGroup(nGroup);
}

ServerNodePtr ServerManager::RandomServerByService(ServerType nType, const std::string& strService)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->RandomServerByService(strService);
}

ServerNodePtr ServerManager::RandomServerInGroupByService(ServerType nType, int32_t nGroup, const std::string& strService)
{
	auto pNodeContainer = MutableServers(nType);

	return pNodeContainer->RandomServerInGroupByService(nGroup, strService);
}
