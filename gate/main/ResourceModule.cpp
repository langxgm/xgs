
#include "ResourceModule.h"
#include "gate/config/SystemConfig.h"
#include "gate/config/GameConfig.h"

#include "gate/pbconfig/msginfo.conf.pb.h"

#include "xshare/config/ProtobufReader.h"
#include "xshare/config/PBConfigManager.h"

#include <iostream>

std::string ResourceModule::GetName()
{
	return "ResourceModule";
}

bool ResourceModule::Init()
{
	bool bRet = true;

	// 系统配置
	{
		ProtobufReader r;

		SystemConfig::InitInstance();
		bRet &= SystemConfig::Me()->InitGateConfig("system_config/gate.json", r);
		bRet &= SystemConfig::Me()->InitServerInfo("system_config/serverinfo.json", r);
	}
	// 游戏配置
	{
		MultiProtobufReader r;

		GameConfig::InitInstance();

		PBConfigManager::InitInstance();
		bRet &= PBConfigManager::Me()->Init<ProtobufMap<std::string, const pbconfig::MsgInfo*>>("game_config/msginfo.json", "name", r);
		PBConfigManager::Me()->PrintAll(std::cout);
		if (bRet) GameConfig::Me()->InitMsgTable();
	}

	return bRet;
}

void ResourceModule::Exit()
{
	PBConfigManager::DestroyInstance();
	GameConfig::DestroyInstance();
	SystemConfig::DestroyInstance();
}

void ResourceModule::RunOnce()
{

}
