
#include "SystemConfig.h"

#include "world/pbconfig/world.conf.pb.h"

#include "xshare/config/ProtobufConf.h"

SystemConfig::~SystemConfig()
{
	delete m_pWorldConfig;
}

bool SystemConfig::InitWorldConfig(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pWorldConfig)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pWorldConfig == nullptr);
	m_pWorldConfig = conf.Value().release();
	LOG(WARNING) << m_pWorldConfig->ShortDebugString();
	return bRet;
}
