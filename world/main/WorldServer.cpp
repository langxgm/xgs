
#include "WorldServer.h"
#include "ResourceModule.h"
#include "LogicModule.h"
#include "DatabaseModule.h"
#include "NetworkModule.h"

#include "xbase/ModuleManager.h"

#include <glog/logging.h>

#include <list>
#include <assert.h>

WorldServer::WorldServer()
{

}

WorldServer::~WorldServer()
{

}

std::string WorldServer::GetName()
{
	return "WorldServer";
}

bool WorldServer::Init(int argc, char** argv)
{
	if (!Application::Init(argc, argv))
		return false;

	ModuleManager::InitInstance();

	std::list<Module*> modules;
	// all module
	modules.push_back(new ResourceModule());
	modules.push_back(new LogicModule());
	modules.push_back(new DatabaseModule());
	modules.push_back(new NetworkModule());
	// register
	for (Module* m : modules)
	{
		ModuleManager::Me()->RegModule(m->GetName(), m);
	}
	// init
	for (Module* m : modules)
	{
		bool ret = m->Init();
		assert(ret);
		if (!ret)
		{
			LOG(FATAL) << "Module:" << m->GetName() << " init failed";
		}
	}
	return true;
}

void WorldServer::Exit()
{
	// exit
	uint32_t nSize = ModuleManager::Me()->GetSize();
	for (uint32_t i = nSize; i > 0; --i)
	{
		Module* m = ModuleManager::Me()->GetModuleByIndex(i - 1);
		m->Exit();
	}

	// destroy
	ModuleManager::DestroyInstance();

	// last
	Application::Exit();
}

void WorldServer::RunOnce()
{
	uint32_t nSize = ModuleManager::Me()->GetSize();
	for (uint32_t i = 0; i < nSize; ++i)
	{
		Module* m = ModuleManager::Me()->GetModuleByIndex(i);
		m->RunOnce();
	}
}
