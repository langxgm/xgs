
#include "SystemConfig.h"

#include "link/pbconfig/link.conf.pb.h"

#include "xshare/config/ProtobufConf.h"

SystemConfig::~SystemConfig()
{
	delete m_pLinkConfig;
}

bool SystemConfig::InitLinkConfig(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pLinkConfig)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pLinkConfig == nullptr);
	m_pLinkConfig = conf.Value().release();
	LOG(WARNING) << m_pLinkConfig->ShortDebugString();
	return bRet;
}
