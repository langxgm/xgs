
#include "GameDataHandler.h"
#include "game/network/from_gws_session.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/t_player_level.h"

#include "game/protos/gamedata.pb.h"

#include "xbase/TimeUtil.h"
#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"
#include "xshare/work/WorkDispatcher.h"

#include <bsoncxx/json.hpp>
#include <libgo/coroutine.h>

void GameDataHandler::PreInitDatabase()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_player_level::t_name);

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_player_level::f_player_guid << 1;
		auto result = coll.create_index(indexBuilder.view(), mongocxx::options::index().unique(true));
		LOG(WARNING) << "create_index " << t_player_level::t_name << " " << bsoncxx::to_json(result);
	}

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_player_level::f_update_time << -1;
		auto result = coll.create_index(indexBuilder.view());
		LOG(WARNING) << "create_index " << t_player_level::t_name << " " << bsoncxx::to_json(result);
	}
}

void GameDataHandler::ListenFromGws(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &GameDataHandler::HandleCGSaveLevelData, this, protos::CGSaveLevelData, "保存关卡数据请求");
}

bool GameDataHandler::QuerySaveLevelData(int64_t nPlayerGUID, int32_t& rLevelNum, std::string& rLevelData)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player_level::f_player_guid << nPlayerGUID;

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_player_level::f_finished_num << 1
		<< t_player_level::f_finished_levels << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto findResult = dbExecutor.FindOne(t_player_level::t_name, filterBuilder.view(), opts);
	//LOG_IF(INFO, findResult) << "query " << t_player_level::t_name << " guid=" << nPlayerGUID;
	if (findResult)
	{
		rLevelNum = MongoElement<b_int32>::GetValue(findResult->view()[t_player_level::f_finished_num]);
		rLevelData = MongoElement<b_utf8>::GetValue(findResult->view()[t_player_level::f_finished_levels]).to_string();
		return true;
	}

	return false;
}

bool GameDataHandler::QueryMultiSaveLevelData(const std::vector<int64_t>& rGUIDs,
	const std::function<void(int64_t, int32_t, std::string&&)>& func)
{
	// 查询指定的玩家
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player_level::f_player_guid << open_document;
	auto arrayBuilder = filterBuilder << "$in" << open_array;
	for (auto nGUID : rGUIDs)
	{
		arrayBuilder << nGUID;
	}
	arrayBuilder << close_array;
	filterBuilder << close_document;

	mongocxx::options::find opts;
	opts.limit(rGUIDs.size());
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_player_level::f_player_guid << 1
		<< t_player_level::f_finished_num << 1
		<< t_player_level::f_finished_levels << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto cursor = dbExecutor.Find(t_player_level::t_name, filterBuilder.view(), opts);
	for (auto&& doc : cursor)
	{
		int64_t nPlayerGUID = MongoElement<b_int64>::GetValue(doc[t_player_level::f_player_guid]);
		int32_t nLevelNum = MongoElement<b_int32>::GetValue(doc[t_player_level::f_finished_num]);
		std::string strLevelData = MongoElement<b_utf8>::GetValue(doc[t_player_level::f_finished_levels]).to_string();

		func(nPlayerGUID, nLevelNum, std::move(strLevelData));
	}

	return true;
}

bool GameDataHandler::RandomPlayerList(int64_t nPlayerGUID, std::vector<int64_t>& rRandomList, uint32_t nLimit,
	const std::vector<int64_t>& rExcludes)
{
	// 1.排除指定玩家GUID和自己
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player_level::f_player_guid << open_document;
	auto arrayBuilder = filterBuilder << "$nin" << open_array;
	for (auto nGUID : rExcludes)
	{
		arrayBuilder << nGUID;
	}
	if (nPlayerGUID > 0)
	{
		arrayBuilder << nPlayerGUID;
	}
	arrayBuilder << close_array;
	filterBuilder << close_document;

	// 2.获取最近更新数据的玩家
	mongocxx::options::find opts;
	opts.limit(nLimit);
	opts.sort(bsoncxx::builder::stream::document{} << t_player_level::f_update_time << -1 << finalize);
	opts.projection(bsoncxx::builder::stream::document{} << t_player_level::f_player_guid << 1 << finalize);

	MongoExecutor dbExecutor;
	auto cursor = dbExecutor.Find(t_player_level::t_name, filterBuilder.view(), opts);
	for (auto&& doc : cursor)
	{
		int64_t nGUID = MongoElement<b_int64>::GetValue(doc[t_player_level::f_player_guid]);
		rRandomList.push_back(nGUID);
	}

	return true;
}

void GameDataHandler::HandleCGSaveLevelData(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGSaveLevelData);

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			bsoncxx::builder::stream::document filterBuilder;
			filterBuilder << t_player_level::f_player_guid << pHandleMsg->guid();

			bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

			bsoncxx::builder::stream::document updateBuilder;
			updateBuilder << "$set"
				<< open_document
				<< t_player_level::f_finished_num << (int32_t)pHandleMsg->finishednum()
				<< t_player_level::f_finished_levels << pHandleMsg->finishedlevels()
				<< t_player_level::f_update_time << date
				<< close_document
				<< "$inc"
				<< open_document
				<< t_player_level::f_update_count << (int32_t)1
				<< close_document;

			// 创建或更新玩家关卡数据
			MongoExecutor dbExecutor;
			auto result = dbExecutor.UpdateOne(t_player_level::t_name, filterBuilder.view(), updateBuilder.view(),
				mongocxx::options::update().upsert(true));
			LOG_IF(INFO, result) << "update " << t_player_level::t_name << " guid=" << pHandleMsg->guid()
				<< ",result matched_count=" << result->matched_count()
				<< " modified_count=" << result->modified_count()
				<< " upserted_count=" << result->result().upserted_count();

			protos::GCSaveLevelData send;
			send.set_error(0);
			send.set_guid(pHandleMsg->guid());
			From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);
		};
	});
}
