
#include "ResourceModule.h"
#include "game/config/SystemConfig.h"

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
		bRet &= SystemConfig::Me()->InitGameConfig("system_config/game.json", r);
	}

	// 游戏配置
	{
		MultiProtobufReader r;

		PBConfigManager::InitInstance();
		PBConfigManager::Me()->PrintAll(std::cout);
	}
	return bRet;
}

void ResourceModule::Exit()
{
	PBConfigManager::DestroyInstance();
	SystemConfig::DestroyInstance();
}

void ResourceModule::RunOnce()
{

}
