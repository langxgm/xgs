
#include "SystemConfig.h"

#include "game/pbconfig/game.conf.pb.h"

#include "xshare/config/ProtobufConf.h"

SystemConfig::~SystemConfig()
{
	delete m_pGameConfig;
}

bool SystemConfig::InitGameConfig(const std::string& strFileName, Reader& r)
{
	ProtobufConf<decltype(m_pGameConfig)> conf;
	bool bRet = conf.Init(strFileName, r);
	assert(bRet);
	assert(m_pGameConfig == nullptr);
	m_pGameConfig = conf.Value().release();
	LOG(WARNING) << m_pGameConfig->ShortDebugString();
	return bRet;
}
