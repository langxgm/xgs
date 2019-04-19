
#include "ResourceModule.h"
#include "link/config/SystemConfig.h"

#include "xshare/config/ProtobufReader.h"

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
		bRet &= SystemConfig::Me()->InitLinkConfig("system_config/link.json", r);
	}

	return bRet;
}

void ResourceModule::Exit()
{
	SystemConfig::DestroyInstance();
}

void ResourceModule::RunOnce()
{

}
