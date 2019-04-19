
#include "ResourceModule.h"
#include "sdk/config/SystemConfig.h"

#include "sdk/pbconfig/serverinfo.conf.pb.h"
#include "sdk/pbconfig/wxappinfo.conf.pb.h"

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
		bRet &= SystemConfig::Me()->InitSdkConfig("system_config/sdk.json", r);
		bRet &= SystemConfig::Me()->InitServerInfo("system_config/serverinfo.json", r);

		MultiProtobufReader mr;

		PBConfigManager::InitInstance();
		bRet &= PBConfigManager::Me()->Init<ProtobufMap<std::string, const pbconfig::WXAppInfo*>>("system_config/wxappinfo.json", "appname", mr);
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
