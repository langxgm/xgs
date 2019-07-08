//------------------------------------------------------------------------
// * @filename: DataStore.h
// *
// * @brief: 数据存储模板
// *
// * @author: XGM
// * @date: 2019/07/08
//------------------------------------------------------------------------
#pragma once

#include "xbase/TimeUtil.h"
#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"

#include <evpp/logging.h>
#include <mongocxx/exception/bulk_write_exception.hpp>

namespace google {
	namespace protobuf {
		class Message;
	}
}

template<typename table_type>
class DataStore
{
public:
	//------------------------------------------------------------------------
	// 加载
	//------------------------------------------------------------------------
	bool Load(int64_t nPlayerGUID, google::protobuf::Message* pData)
	{
		bsoncxx::builder::stream::document filterBuilder;
		filterBuilder << table_type::f_player_guid << nPlayerGUID;

		MongoExecutor dbExecutor;
		auto coll = dbExecutor.GetCollection(table_type::t_name);

		auto findResult = dbExecutor.FindOne(table_type::t_name, filterBuilder.view());
		if (findResult)
		{
			auto data = MongoElement<b_binary>::GetValue(findResult->view()[table_type::f_data]);
			if (pData->ParseFromArray(data.data(), data.size()) == false)
			{
				pData->Clear();
				LOG(ERROR) << "ParseFromArray fail table=" << table_type::t_name << " guid=" << nPlayerGUID;
				return false;
			}
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------
	// 存储
	//------------------------------------------------------------------------
	template<const uint32_t MaxBuffSize = 16384>
	bool Save(int64_t nPlayerGUID, google::protobuf::Message* pData)
	{
		uint8_t buffer[MaxBuffSize];

		uint32_t nSize = pData->ByteSizeLong();
		LOG_IF(ERROR, nSize > MaxBuffSize) << "SerializeToArray fail table=" << table_type::t_name
			<< " guid=" << nPlayerGUID
			<< " max_size=" << MaxBuffSize
			<< " cur_size=" << nSize;
		if (nSize > MaxBuffSize)
		{
			return false;
		}

		if (pData->SerializeToArray(buffer, MaxBuffSize) == false)
		{
			LOG(ERROR) << "SerializeToArray fail table=" << table_type::t_name << " guid=" << nPlayerGUID;
			return false;
		}

		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };
		bsoncxx::types::b_binary binary{ bsoncxx::binary_sub_type::k_binary, nSize, buffer };

		bsoncxx::builder::stream::document filterBuilder;
		filterBuilder << table_type::f_player_guid << nPlayerGUID;

		MongoExecutor dbExecutor;
		auto coll = dbExecutor.GetCollection(table_type::t_name);

		auto findResult = coll.find_one(filterBuilder.view());
		if (!findResult)
		{
			bsoncxx::builder::stream::document createBuilder;
			createBuilder << table_type::f_player_guid << nPlayerGUID
				<< table_type::f_update_time << date
				<< table_type::f_update_count << (int32_t)1
				<< table_type::f_dbversion << table_type::v_dbversion
				<< table_type::f_data << binary;

			try
			{
				auto result = coll.insert_one(createBuilder.view());
				DLOG_IF(INFO, result) << "create " << table_type::t_name << " guid=" << nPlayerGUID
					<< ",result inserted_count=" << result->result().inserted_count();
			}
			catch (const mongocxx::bulk_write_exception& e)
			{
				LOG(ERROR) << "create " << table_type::t_name << " guid=" << nPlayerGUID
					<< ",mongocxx::bulk_write_exception e=" << e.what();
				return false;
			}
		}
		else
		{
			bsoncxx::builder::stream::document updateBuilder;
			updateBuilder << "$set"
				<< open_document
				<< table_type::f_update_time << date
				<< table_type::f_dbversion << table_type::v_dbversion
				<< table_type::f_data << binary
				<< close_document
				<< "$inc"
				<< open_document
				<< table_type::f_update_count << (int32_t)1
				<< close_document;

			auto result = coll.update_one(filterBuilder.view(), updateBuilder.view());
			LOG_IF(INFO, result) << "update " << table_type::t_name << " guid=" << nPlayerGUID
				<< ",result matched_count=" << result->matched_count()
				<< " modified_count=" << result->modified_count();
		}
		return true;
	}

	//------------------------------------------------------------------------
	// 删除
	//------------------------------------------------------------------------
	bool Delete(int64_t nPlayerGUID)
	{
		bsoncxx::builder::stream::document filterBuilder;
		filterBuilder << table_type::f_player_guid << nPlayerGUID;

		MongoExecutor dbExecutor;
		auto result = dbExecutor.DeleteOne(table_type::t_name, filterBuilder.view());
		DLOG_IF(INFO, result) << "delete " << table_type::t_name << " guid=" << nPlayerGUID
			<< ",result deleted_count=" << result->result().deleted_count();
		return true;
	}
};
