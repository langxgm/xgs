
#include "DatabaseModule.h"

#include "login/config/SystemConfig.h"
#include "login/pbconfig/login.conf.pb.h"

#include "xshare/work/LogicManager.h"
#include "xdb/DBHandler.h"
#include "xdb/mongo/MongoClient.h"

#include "common/logic/t_server_cache.h"

#include <glog/logging.h>

std::string DatabaseModule::GetName()
{
	return "DatabaseModule";
}

bool DatabaseModule::Init()
{
	auto& dbConf = SystemConfig::Me()->GetLoginConfig()->databasemodule();

	// mongodb begin
	MongoClient::InitInstance();
	MongoClient::Me()->Init(dbConf.host()/*"mongodb://127.0.0.1:27017"*/, dbConf.username()/*""*/, dbConf.password()/*""*/);
	MongoClient::Me()->SetDefaultDBName(dbConf.dbname()/*"worlddb"*/);
	if (MongoClient::Me()->Start())
	{
		LOG(WARNING) << "mongodb connect success";
	}
	else
	{
		LOG(ERROR) << "mongodb connect failed";
		return false;
	}
	// mongodb end

	dbcache::t_server_cache::InitInstance();
	dbcache::t_server_cache::InitDatabase("homedb");

	// 初始化数据库
	for (auto handler : *LogicManager::Me())
	{
		auto dbHandler = dynamic_cast<DBHandler*>(handler);
		if (dbHandler)
		{
			dbHandler->PreInitDatabase();
		}
	}
	return true;
}

void DatabaseModule::Exit()
{
	dbcache::t_server_cache::DestroyInstance();

	// mongodb begin
	MongoClient::Me()->Stop();
	MongoClient::DestroyInstance();
	// mongodb end
}

void DatabaseModule::RunOnce()
{

}
