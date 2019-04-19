
#include "FeedbackHandler.h"
#include "game/network/from_gws_session.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/t_player_feedback.h"

#include "game/protos/feedback.pb.h"

#include "xbase/TimeUtil.h"
#include "xdb/mongo/MongoExecutor.h"
#include "xshare/work/WorkDispatcher.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <libgo/coroutine.h>

void FeedbackHandler::PreInitDatabase()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_player_feedback::t_name);

	bsoncxx::builder::stream::document indexBuilder;
	indexBuilder << t_player_feedback::f_send_time << -1;
	auto result = coll.create_index(indexBuilder.view());
	LOG(WARNING) << "create_index " << t_player_feedback::t_name << " " << bsoncxx::to_json(result);
}

void FeedbackHandler::ListenFromGws(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &FeedbackHandler::HandleCGFeedback, this, protos::CGFeedback, "意见反馈请求");
}

bool FeedbackHandler::AddFeedback(int64_t nPlayerGUID, const std::string& strContent)
{
	bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

	bsoncxx::builder::stream::document createBuilder;
	createBuilder << t_player_feedback::f_player_guid << nPlayerGUID
		<< t_player_feedback::f_send_time << date
		<< t_player_feedback::f_content << strContent;

	try
	{
		MongoExecutor dbExecutor;
		auto result = dbExecutor.InsertOne(t_player_feedback::t_name, createBuilder.view());
		LOG_IF(INFO, result) << "create " << t_player_feedback::t_name << " guid=" << nPlayerGUID
			<< ",result inserted_count=" << result->result().inserted_count();
	}
	catch (const mongocxx::bulk_write_exception& e)
	{
		LOG(ERROR) << "create " << t_player_feedback::t_name << " guid=" << nPlayerGUID
			<< ",mongocxx::bulk_write_exception e=" << e.what();
		return false;
	}

	return true;
}

void FeedbackHandler::HandleCGFeedback(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGFeedback);

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			// 内容截取(256-1)个有效字符
			if (pHandleMsg->content().length() >= t_player_feedback::MaxContentLen)
			{
				pHandleMsg->mutable_content()->erase(t_player_feedback::MaxContentLen - 1);
			}

			AddFeedback(pHandleMsg->guid(), pHandleMsg->content());

			protos::GCFeedback send;
			send.set_error(0);
			send.set_guid(pHandleMsg->guid());
			From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);
		};
	});
}
