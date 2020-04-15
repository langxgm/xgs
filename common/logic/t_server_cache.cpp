
#include "t_server_cache.h"
#include "t_server.h"

#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"

#include <glog/logging.h>
#include <evpp/utility.h>
#include <bsoncxx/json.hpp>

namespace dbcache
{
	t_server_cache::t_server_cache()
	{

	}

	t_server_cache::~t_server_cache()
	{

	}

	void t_server_cache::InitDatabase(const std::string& strDBName)
	{
		MongoExecutor dbExecutor(strDBName);
		auto coll = dbExecutor.GetCollection(t_server::t_name);

		{
			bsoncxx::builder::stream::document indexBuilder;
			indexBuilder << t_server::f_name << 1;
			auto result = coll.create_index(indexBuilder.view());
			LOG(WARNING) << "create_index " << t_server::t_name << " " << bsoncxx::to_json(result);
		}

		{
			bsoncxx::builder::stream::document indexBuilder;
			indexBuilder << t_server::f_id << 1;
			auto result = coll.create_index(indexBuilder.view());
			LOG(WARNING) << "create_index " << t_server::t_name << " " << bsoncxx::to_json(result);
		}

		InitDataFromDatabase(strDBName);
	}

	void t_server_cache::InitDataFromDatabase(const std::string& strDBName)
	{
		LOG(INFO) << "server list load start";

		dbcache::t_server_cache::Me()->RemoveAll();

		bsoncxx::builder::stream::document filterBuilder;

		MongoExecutor dbExecutor(strDBName);
		auto cursor = dbExecutor.Find(t_server::t_name, filterBuilder.view());
		for (auto&& doc : cursor)
		{
			dbcache::t_server_cache::ServerInfo info;

			info.strName = MongoElement<b_utf8>::GetValue(doc[t_server::f_name]).to_string();
			info.nID = MongoElement<b_int32>::GetValue(doc[t_server::f_id]);
			info.strVersion = MongoElement<b_utf8>::GetValue(doc[t_server::f_version]).to_string();
			info.strAddr = MongoElement<b_utf8>::GetValue(doc[t_server::f_addr]).to_string();
			info.strUrl = MongoElement<b_utf8>::GetValue(doc[t_server::f_url]).to_string();

			// 解析版本号
			const int32_t nMaxIndex = info.arrVersion.size();

			std::vector<std::string> vecString;
			evpp::StringSplit(info.strVersion, ".", nMaxIndex, vecString);
			for (uint32_t i = 0; i < vecString.size(); ++i)
			{
				if (i < nMaxIndex)
				{
					info.arrVersion[i] = atoi(vecString[i].c_str());
				}
			}

			// 添加服务器信息
			if (info.strName == server_name::s_gateway)
			{
				dbcache::t_server_cache::Me()->AddServer(ServerType::Gateway, info);
			}
			else if (info.strName == server_name::s_game)
			{
				dbcache::t_server_cache::Me()->AddServer(ServerType::Game, info);
			}
			else if (info.strName == server_name::s_world)
			{
				dbcache::t_server_cache::Me()->AddServer(ServerType::World, info);
			}
			else if (info.strName == server_name::s_sdk)
			{
				dbcache::t_server_cache::Me()->AddServer(ServerType::Sdk, info);
			}
			else if (info.strName == server_name::s_client_res)
			{
				dbcache::t_server_cache::Me()->AddServer(ServerType::ClientRes, info);
			}

			LOG(INFO) << "server info " << bsoncxx::to_json(doc);
		}

		LOG(INFO) << "game server size=" << dbcache::t_server_cache::Me()->GetServerNum(ServerType::Game);

		LOG(INFO) << "server list load finish";
	}

	bool t_server_cache::AddServer(ServerType nType, const ServerInfo& info)
	{
		m_mapServer[nType].push_back(info);
		return true;
	}

	void t_server_cache::RemoveAll()
	{
		m_mapServer.clear();
	}

	int32_t t_server_cache::GetServerNum(ServerType nType)
	{
		return m_mapServer[nType].size();
	}

	const t_server_cache::ServerList& t_server_cache::GetServerList(ServerType nType)
	{
		return m_mapServer[nType];
	}

	t_server_cache::ServerInfo* t_server_cache::GetServer(ServerType nType, int32_t nID)
	{
		for (auto& s : m_mapServer[nType])
		{
			if (s.nID == nID)
			{
				return &s;
			}
		}
		return nullptr;
	}

	t_server_cache::ServerInfo* t_server_cache::GetServerByIndex(ServerType nType, int32_t nIndex)
	{
		auto& vec = m_mapServer[nType];
		if (nIndex >= 0 && nIndex < (int32_t)vec.size())
		{
			return &vec[nIndex];
		}
		return nullptr;
	}
}
