
#include "GenGUID.h"

#include "common/logic/t_guid_serial.h"

#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"

#include <glog/logging.h>
#include <bsoncxx/json.hpp>

static const int64_t g_nStep = INT64_C(100000); // 每次重置后自增步长

GUIDValue g_PartyGUID(t_guid_serial::v_party_guid_name, t_guid_serial::v_party_guid_init, g_nStep);
GUIDValue g_ShareGUID(t_guid_serial::v_share_guid_name, t_guid_serial::v_share_guid_init, g_nStep);
GUIDValue g_GameRoomGUID(t_guid_serial::v_game_room_guid_name, t_guid_serial::v_game_room_guid_init, g_nStep);

void GenGUID::PreInitDatabase()
{
	InitIndex();
	InitUserID();
	InitPlayerGUID();
	InitPartyGUID();
	InitShareGUID();
	InitGameRoomGUID();
}

void GenGUID::InitIndex()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_guid_serial::t_name);

	bsoncxx::builder::stream::document indexBuilder;
	indexBuilder << t_guid_serial::f_guid_type << 1;
	auto result = coll.create_index(indexBuilder.view(), mongocxx::options::index().unique(true));
	LOG(WARNING) << "create_index " << t_guid_serial::t_name << " " << bsoncxx::to_json(result);
}

int64_t GenGUID::InitGUIDByType(const std::string& strTypeName, int64_t nInitValue, bool bReset /*= false*/)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_guid_serial::f_guid_type << strTypeName;

	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_guid_serial::t_name);

	int64_t nGUID = nInitValue;

	// 查找
	auto findResult = coll.find_one(filterBuilder.view());
	if (!findResult)
	{
		// 创建并初始化
		bsoncxx::builder::stream::document createBuilder;
		createBuilder << t_guid_serial::f_guid_type << strTypeName
			<< t_guid_serial::f_guid_value << nInitValue;
		coll.insert_one(createBuilder.view());
		LOG(WARNING) << "init " << strTypeName << "=" << nInitValue;
	}
	else
	{
		// 打印当前值
		nGUID = MongoElement<b_int64>::GetValue(findResult->view()[t_guid_serial::f_guid_value]);
		LOG(WARNING) << "current " << strTypeName << "=" << nGUID;

		if (bReset)
		{
			nGUID = nInitValue;

			bsoncxx::builder::stream::document updateBuilder;
			updateBuilder << "$set"
				<< open_document
				<< t_guid_serial::f_guid_value << nInitValue
				<< close_document;
			auto result = coll.update_one(filterBuilder.view(), updateBuilder.view());
			if (result && result->matched_count() == 1 && result->modified_count() == 1)
			{
				LOG(WARNING) << "reset " << strTypeName << "=" << nInitValue;
			}
			else
			{
				LOG(FATAL) << "reset " << strTypeName << "=" << nInitValue << " fail"; // 重置失败
			}
		}
	}

	return nGUID;
}

int64_t GenGUID::GenGUIDByType(const std::string& strTypeName, int64_t nAddValue/* = 1*/)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_guid_serial::f_guid_type << strTypeName;

	// GUID+1
	bsoncxx::builder::stream::document updateBuilder;
	updateBuilder << "$inc"
		<< open_document
		<< t_guid_serial::f_guid_value << (int64_t)nAddValue
		<< close_document;

	MongoExecutor dbExecutor;
	auto result = dbExecutor.FindOneAndUpdate(t_guid_serial::t_name, filterBuilder.view(), updateBuilder.view());

	int64_t nGUID = 0;
	if (result)
	{
		nGUID = MongoElement<b_int64>::GetValue(result->view()[t_guid_serial::f_guid_value]);
		if (nGUID > 0)
		{
			nGUID += nAddValue;
		}
		else
		{
			LOG(ERROR) << "find_one_and_update " << strTypeName << " " << t_guid_serial::f_guid_value << " element type valid";
		}
	}
	else
	{
		LOG(ERROR) << "find_one_and_update " << strTypeName << " faild";
	}

	LOG(INFO) << "gen " << strTypeName << "=" << nGUID;
	return nGUID;
}

void GenGUID::InitUserID()
{
	InitGUIDByType(t_guid_serial::v_user_id_name, t_guid_serial::v_user_id_init);
}

void GenGUID::InitPlayerGUID()
{
	InitGUIDByType(t_guid_serial::v_player_guid_name, t_guid_serial::v_player_guid_init);
}

void GenGUID::InitPartyGUID()
{
	g_PartyGUID.Init();
}

void GenGUID::InitShareGUID()
{
	g_ShareGUID.Init();
}

void GenGUID::InitGameRoomGUID()
{
	g_GameRoomGUID.Init();
}

int64_t GenGUID::GenUserID()
{
	return GenGUIDByType(t_guid_serial::v_user_id_name);
}

int64_t GenGUID::GenPlayerGUID()
{
	return GenGUIDByType(t_guid_serial::v_player_guid_name);
}

int64_t GenGUID::GenPartyGUID()
{
	return g_PartyGUID.Next();
}

int64_t GenGUID::GenShareGUID()
{
	return g_ShareGUID.Next();
}

int64_t GenGUID::GenGameRoomGUID()
{
	return g_GameRoomGUID.Next();
}

GUIDValue::GUIDValue(const std::string& strName, int64_t nInitGUID, int64_t nStep)
	: m_strName(strName)
	, m_nInitGUID(nInitGUID)
	, m_nStep(nStep)
{

}

void GUIDValue::Init()
{
	m_nCurGUID = GenGUID::InitGUIDByType(m_strName, m_nInitGUID);
	m_nCurGUID = GenGUID::GenGUIDByType(m_strName, m_nStep);
}

int64_t GUIDValue::Next()
{
	std::lock_guard<std::mutex> guard(m_Mutex);

	auto nGUID = ++m_nCurGUID;
	if (nGUID % m_nStep == 0)
	{
		m_nCurGUID = GenGUID::GenGUIDByType(m_strName, m_nStep);
		nGUID = m_nCurGUID;
	}
	return nGUID;
}
