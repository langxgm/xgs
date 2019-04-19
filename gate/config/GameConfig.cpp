
#include "GameConfig.h"

#include "gate/pbconfig/msginfo.conf.pb.h"

#include "xshare/config/PBConfigManager.h"
#include "xshare/net/CRC32.h"

#include <iomanip>

void GameConfig::InitMsgTable()
{
	int32_t i = 0;
	for (auto it : *PBConfigManager::Me()->ObjectPtr<std::string, const pbconfig::MsgInfo*>())
	{
		const std::string& strMsgName = it.second->name();
		uint32_t nMsgID = Crc32(strMsgName.c_str());
		m_mapMsgTable.insert(std::make_pair(nMsgID, strMsgName));

		// 打印消息
		std::cout << std::setw(3) << ++i << "."
			<< " [" << std::setw(10) << nMsgID << "]:"
			<< " { open=" << it.second->open()
			<< ", c2w=" << it.second->dir().c2w()
			<< ", w2c=" << it.second->dir().w2c()
			<< ", log.on=" << it.second->log().on()
			<< " }, " << strMsgName
			<< std::endl;
	}
}

const pbconfig::MsgInfo* GameConfig::GetMsgInfo(uint32_t nMsgID)
{
	auto it = m_mapMsgTable.find(nMsgID);
	if (it != m_mapMsgTable.end())
	{
		return GetMsgInfo(it->second);
	}
	return nullptr;
}

const pbconfig::MsgInfo* GameConfig::GetMsgInfo(const std::string& strMsgName)
{
	auto pMsgInfo = PBConfigManager::Me()->At<std::string, const pbconfig::MsgInfo*>(strMsgName);
	assert(pMsgInfo);
	return pMsgInfo.get();
}
