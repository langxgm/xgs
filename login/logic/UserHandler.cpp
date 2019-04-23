
#include "UserHandler.h"
#include "login/network/from_link_session.h"
#include "login/logic/HandleMsgMacros.h"

#include "login/protos/login.pb.h"

#include "common/logic/t_player_of_user.h"
#include "common/logic/t_server_cache.h"

#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"
#include "xshare/work/WorkDispatcher.h"

#include <glog/logging.h>
#include <libgo/coroutine.h>

UserHandler::UserHandler()
{

}

UserHandler::~UserHandler()
{

}

void UserHandler::PreInitDatabase()
{

}

void UserHandler::UpdateLogic()
{

}

void UserHandler::ListenFromLink(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &UserHandler::HandleCLServerList, this, protos::CLServerList, "服务器列表请求");
}

bool UserHandler::QueryPlayerOfUserInfo(int64_t nUserID, std::map<int32_t, LittlePlayerOfUserInfo>& rPlayers)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player_of_user::f_userid << nUserID;

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_player_of_user::f_userid << 1
		<< t_player_of_user::f_server_id << 1
		<< t_player_of_user::f_guid << 1
		<< t_player_of_user::f_name << 1
		<< t_player_of_user::f_portrait << 1
		<< t_player_of_user::f_level << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto cursor = dbExecutor.Find(t_player_of_user::t_name, filterBuilder.view(), opts);
	for (auto&& doc : cursor)
	{
		LittlePlayerOfUserInfo info;
		info.nUserID = MongoElement<b_int64>::GetValue(doc[t_player_of_user::f_userid]);
		info.nServerID = MongoElement<b_int32>::GetValue(doc[t_player_of_user::f_server_id]);
		info.nPlayerGUID = MongoElement<b_int64>::GetValue(doc[t_player_of_user::f_guid]);
		info.strPlayerName = MongoElement<b_utf8>::GetValue(doc[t_player_of_user::f_name]).to_string();
		info.nPortrait = MongoElement<b_int32>::GetValue(doc[t_player_of_user::f_portrait]);
		info.nPlayerLevel = MongoElement<b_int32>::GetValue(doc[t_player_of_user::f_level]);

		rPlayers.insert(std::make_pair(info.nServerID, std::move(info)));
	}

	return true;
}

void UserHandler::HandleCLServerList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLServerList);

	int64_t nUserID = pMeta->GetGUID();

	WorkDispatcherAsync(nUserID).RunInLoop([=]() {
		go[=]() {

			// 获取玩家信息
			std::map<int32_t, LittlePlayerOfUserInfo> playerOfUserInfos;
			if (QueryPlayerOfUserInfo(nUserID, playerOfUserInfos) == false)
			{
				protos::LCServerList send;
				send.set_error(1);
				send.set_errmsg("query player of user info error"); // 查询玩家信息错误
				From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(WARNING) << "HandleCLServerList/fail userid=" << nUserID
					<< " query player of user info error";
				return;
			}

			// 转到主线程
			WorkDispatcherSync().RunInLoop([=]() {

				protos::LCServerList send;

				// 填充服务器信息
				for (const auto& server : dbcache::t_server_cache::Me()->GetServerList(dbcache::ServerType::Game))
				{
					auto pNew = send.add_server_list();
					pNew->set_id(server.nID);
					pNew->set_name(server.strName);
					pNew->set_state(protos::LCServerList::OPEN);
					pNew->set_addr(server.strAddr);

					// 填充玩家信息
					auto it = playerOfUserInfos.find(server.nID);
					if (it != playerOfUserInfos.end())
					{
						auto& rPlayer = it->second;
						pNew->mutable_player_info()->set_guid(rPlayer.nPlayerGUID);
						pNew->mutable_player_info()->set_name(std::move(rPlayer.strPlayerName));
						pNew->mutable_player_info()->set_portraitid(rPlayer.nPortrait);
						pNew->mutable_player_info()->set_level(rPlayer.nPlayerLevel);
					}
				}

				From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(INFO) << "HandleCLServerList/ok userid=" << nUserID
					<< " server_list_size=" << send.server_list_size();
			});
		};
	});
}
