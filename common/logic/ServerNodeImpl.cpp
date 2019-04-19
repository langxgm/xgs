
#include "ServerNodeImpl.h"

#include "xbase/TimeUtil.h"

void ServerNode_Base::Update()
{
	int64_t nNow = TimeUtil::GetCurrentTimeMillis();

	if (m_PingTimer.IsTimeout(nNow))
	{
		m_PingTimer.Stop();

		DoPing();
	}
}

void ServerNode_Base::OnPing()
{
	DoPong();
}

void ServerNode_Base::OnPong()
{
	m_PingTimer.Start(TimeUtil::GetCurrentTimeMillis(), 30 * 1000);
}

bool ServerNode_Base::IsHaveService(const std::string& strService) const
{
	return m_mapService.find(strService) != m_mapService.end();
}

bool ServerNode_Base::CheckServiceCond(const std::string& strService, const std::vector<int64_t>& vecParam) const
{
	auto it = m_mapService.find(strService);
	if (it != m_mapService.end())
	{
		// 条件检测
		return true;
	}
	return false;
}

bool ServerNode_Base::AddService(const std::string& strService, const std::vector<int64_t>& vecParam)
{
	if (vecParam.size() != 3)
	{
		return false;
	}

	ServerService service;
	service.strName = strService;
	service.aParam.nParam1 = vecParam[0];
	service.aParam.nParam2 = vecParam[1];
	service.aParam.nParam3 = vecParam[2];

	auto it = m_mapService.insert(std::make_pair(strService, service));
	return it.second;
}

bool ServerNode_Base::DelService(const std::string& strService)
{
	auto it = m_mapService.find(strService);
	if (it != m_mapService.end())
	{
		m_mapService.erase(it);
		return true;
	}
	return false;
}
