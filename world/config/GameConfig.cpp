
#include "GameConfig.h"

#include "xshare/config/ProtobufConf.h"

GameConfig::~GameConfig()
{
	delete m_pGameMatchSystem;
}
