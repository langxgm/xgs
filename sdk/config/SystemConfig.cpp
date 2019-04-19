
#include "SystemConfig.h"

#include "sdk/pbconfig/sdk.conf.pb.h"
#include "sdk/pbconfig/serverinfo.conf.pb.h"

#include "xshare/config/ProtobufConf.h"

SystemConfig::~SystemConfig()
{
	delete m_pServerInfo;
	delete m_pSdkConfig;
}

bool SystemConfig::InitSdkConfig(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pSdkConfig)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pSdkConfig == nullptr);
	m_pSdkConfig = conf.Value().release();
	LOG(WARNING) << m_pSdkConfig->ShortDebugString();
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
