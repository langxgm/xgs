
#include "ResourceModule.h"
#include "world/config/SystemConfig.h"
#include "world/config/GameConfig.h"

#include "xshare/config/ProtobufReader.h"
#include "xshare/config/PBConfigManager.h"

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
		bRet &= SystemConfig::Me()->InitWorldConfig("system_config/world.json", r);
	}

	// 游戏配置
	{
		ProtobufReader r;

		GameConfig::InitInstance();

		MultiProtobufReader mr;

		PBConfigManager::InitInstance();
		PBConfigManager::Me()->PrintAll(std::cout);
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
