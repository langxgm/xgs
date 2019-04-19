
#include "SystemConfig.h"

#include "login/pbconfig/login.conf.pb.h"

#include "xshare/config/ProtobufConf.h"

SystemConfig::~SystemConfig()
{
	delete m_pLoginConfig;
}

bool SystemConfig::InitLoginConfig(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pLoginConfig)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pLoginConfig == nullptr);
	m_pLoginConfig = conf.Value().release();
	LOG(WARNING) << m_pLoginConfig->ShortDebugString();
	return bRet;
}
