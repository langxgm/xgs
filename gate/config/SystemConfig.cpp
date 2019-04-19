
#include "SystemConfig.h"

#include "gate/pbconfig/gate.conf.pb.h"
#include "gate/pbconfig/serverinfo.conf.pb.h"

#include "xshare/config/ProtobufConf.h"

SystemConfig::~SystemConfig()
{
	delete m_pGateConfig;
}

bool SystemConfig::InitGateConfig(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pGateConfig)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pGateConfig == nullptr);
	m_pGateConfig = conf.Value().release();
	LOG(WARNING) << m_pGateConfig->ShortDebugString();
	return bRet;
}

bool SystemConfig::InitServerInfo(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pServerInfo)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pServerInfo == nullptr);
	m_pServerInfo = conf.Value().release();
	LOG(WARNING) << m_pServerInfo->ShortDebugString();
	return bRet;
}
