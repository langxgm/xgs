
#include "LoginHandler.h"
#include "world/network/from_gws_session.h"
#include "world/network/from_gs_session.h"
#include "world/logic/HandleMsgMacros.h"
#include "world/logic/PlayerFactory.h"
#include "world/logic/PlayerManager.h"
#include "world/logic/Player.h"

#include "world/protos/login2game.pb.h"

#include "common/logic/t_player_of_user.h"
#include "common/logic/t_user.h"
#include "common/logic/GenGUID.h"
#include "common/logic/ServerManager.h"
#include "common/logic/ServerNode.h"

#include "xbase/TimeUtil.h"
#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"
#include "xshare/net/CRC32.h"
#include "xshare/net/GenSN.h"
#include "xshare/work/WorkDispatcher.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <libgo/coroutine.h>

LoginHandler::LoginHandler()
{
	PlayerManager::InitInstance();
	PlayerManager::Me()->Init(0x7FFFFFF);
}

LoginHandler::~LoginHandler()
{
	PlayerManager::DestroyInstance();
}

void LoginHandler::PreInitDatabase()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_player_of_user::t_name);

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_player_of_user::f_userid << 1;
		auto result = coll.create_index(indexBuilder.view());
		LOG(WARNING) << "create_index " << t_player_of_user::t_name << " " << bsoncxx::to_json(result);
	}

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_player_of_user::f_server_id << 1;
		auto result = coll.create_index(indexBuilder.view());
		LOG(WARNING) << "create_index " << t_player_of_user::t_name << " " << bsoncxx::to_json(result);
	}

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_player_of_user::f_guid << 1;
		auto result = coll.create_index(indexBuilder.view(), mongocxx::options::index().unique(true));
		LOG(WARNING) << "create_index " << t_player_of_user::t_name << " " << bsoncxx::to_json(result);
	}
}

void LoginHandler::UpdateLogic()
{

}

void LoginHandler::ListenFromGws(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleGWS2WReconnLogin, this, protos::GWS2WReconnLogin, "断线重连请求");
	REG_MSG_TO(pServer, &LoginHandler::HandleGWS2WDisconnect, this, protos::GWS2WDisconnect, "断线请求");
}

void LoginHandler::ListenFromGs(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleGWLogin, this, protos::GWLogin, "登陆请求");
}

bool LoginHandler::LoginWorld(PlayerOfUserInfo& rInfo)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player_of_user::f_userid << rInfo.nUserID
		<< t_player_of_user::f_server_id << rInfo.nServerID;

	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_player_of_user::t_name);

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_player_of_user::f_guid << 1
		<< finalize);

	int64_t nGUID = 0;
	auto findResult = coll.find_one(filterBuilder.view(), opts);
	if (!findResult)
	{
		nGUID = GenGUID::GenPlayerGUID();

		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

		// 创建user
		bsoncxx::builder::stream::document createBuilder;
		createBuilder << t_player_of_user::f_userid << rInfo.nUserID
			<< t_player_of_user::f_server_id << rInfo.nServerID
			<< t_player_of_user::f_guid << rInfo.nPlayerGUID
			<< t_player_of_user::f_name << rInfo.strPlayerName
			<< t_player_of_user::f_portrait << rInfo.nPortrait
			<< t_player_of_user::f_level << rInfo.nPlayerLevel
			<< t_player_of_user::f_register_time << date
			<< t_player_of_user::f_login_time << date
			<< t_player_of_user::f_login_count << (int32_t)1
			<< t_player_of_user::f_deviceid << rInfo.strDeviceID
			<< t_player_of_user::f_ip << rInfo.strIP;

		try
		{
			auto result = coll.insert_one(createBuilder.view());
			LOG_IF(INFO, result) << "create " << t_player_of_user::t_name << " userid=" << rInfo.nUserID
				<< " server_id=" << rInfo.nServerID
				<< " guid=" << nGUID
				<< ",result inserted_count=" << result->result().inserted_count();
		}
		catch (const mongocxx::bulk_write_exception& e)
		{
			LOG(ERROR) << "create " << t_player_of_user::t_name << " userid=" << rInfo.nUserID
				<< " server_id=" << rInfo.nServerID
				<< " guid=" << nGUID
				<< ",mongocxx::bulk_write_exception e=" << e.what();
			return false;
		}
	}
	else
	{
		nGUID = MongoElement<b_int64>::GetValue(findResult->view()[t_player_of_user::f_guid]);

		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

		// 更新登陆IP,时间,次数+1
		bsoncxx::builder::stream::document updateBuilder;
		updateBuilder << "$set"
			<< open_document
			<< t_player_of_user::f_name << rInfo.strPlayerName
			<< t_player_of_user::f_portrait << rInfo.nPortrait
			<< t_player_of_user::f_level << rInfo.nPlayerLevel
			<< t_player_of_user::f_login_time << date
			<< t_player_of_user::f_ip << rInfo.strIP
			<< close_document
			<< "$inc"
			<< open_document
			<< t_player_of_user::f_login_count << (int32_t)1
			<< close_document;

		auto result = coll.update_one(filterBuilder.view(), updateBuilder.view());
		LOG_IF(INFO, result) << "update " << t_player_of_user::t_name << " userid=" << rInfo.nUserID
			<< " server_id=" << rInfo.nServerID
			<< " guid=" << nGUID
			<< ",result matched_count=" << result->matched_count()
			<< " modified_count=" << result->modified_count();
	}

	rInfo.nPlayerGUID = nGUID;
	return true;
}

bool LoginHandler::QueryPlayerGUID(int64_t nUserID, int32_t nServerID, int64_t& rPlayerGUID)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player_of_user::f_userid << nUserID
		<< t_player_of_user::f_server_id << nServerID;

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_player_of_user::f_guid << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto findResult = dbExecutor.FindOne(t_player_of_user::t_name, filterBuilder.view(), opts);
	if (findResult)
	{
		rPlayerGUID = MongoElement<b_int64>::GetValue(findResult->view()[t_player_of_user::f_guid]);
		return true;
	}

	return false;
}

bool LoginHandler::QueryUserInfo(int64_t nUserID, LittleUserInfo& rInfo)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_user::f_userid << nUserID;

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_user::f_login_time << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto findResult = dbExecutor.FindOne(t_user::t_name, filterBuilder.view(), opts);
	if (findResult)
	{
		rInfo.nLoginTime = MongoElement<b_int64>::GetValue(findResult->view()[t_user::f_login_time]);

		rInfo.nUserID = nUserID;
		rInfo.strToken = std::to_string(Crc32(std::to_string(nUserID ^ rInfo.nLoginTime).c_str()) * rInfo.nLoginTime);
		return true;
	}

	return false;
}

void LoginHandler::LinkPlayer(int64_t nPlayerGUID, int64_t nSessionID, const std::string& strOpenID,
	const std::string& strSessionKey, const std::string& strLoginKey)
{
	PlayerPtr pPlayer = PlayerManager::Me()->GetPlayer(nPlayerGUID);
	if (!pPlayer)
	{
		auto pPlayerFactory = PlayerManager::Me()->GetPlayerFactory();
		if (!pPlayerFactory)
		{
			assert(false);
			return;
		}

		pPlayer.reset(pPlayerFactory->NewPlayer());
		if (!pPlayer)
		{
			return; // new error
		}

		pPlayer->Init(nPlayerGUID, nSessionID);

		pPlayer->SetLoginKey(strLoginKey);

		pPlayer->SetOpenID(strOpenID);
		pPlayer->SetSessionKey(strSessionKey);

		if (PlayerManager::Me()->AddPlayer(nPlayerGUID, pPlayer))
		{
			pPlayer->Online();

			LOG(INFO) << "LinkPlayer/ok AddPlayer guid=" << nPlayerGUID
				<< " playerNum=" << PlayerManager::Me()->GetPlayerNum() << " playerLimit=" << PlayerManager::Me()->GetPlayerLimit();
		}
		else
		{
			LOG(WARNING) << "LinkPlayer/fail AddPlayer guid=" << nPlayerGUID << " player full"
				<< " playerNum=" << PlayerManager::Me()->GetPlayerNum() << " playerLimit=" << PlayerManager::Me()->GetPlayerLimit();
		}
	}
	else
	{
		pPlayer->Reconnect(nSessionID);

		PlayerManager::Me()->ModifyMapping(pPlayer, strOpenID, strSessionKey, strLoginKey);

		pPlayer->Online();

		LOG(INFO) << "LinkPlayer/ok ModifyMapping guid=" << nPlayerGUID;
	}
}

void LoginHandler::HandleGWLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::GWLogin);

	WorkDispatcherAsync(pHandleMsg->param().userid()).RunInLoop([=]() {
		go[=]() {

			LittleUserInfo userInfo;
			if (QueryUserInfo(pHandleMsg->param().userid(), userInfo) == false)
			{
				protos::WGLogin send;
				send.set_error(1); // user信息没查到
				send.set_errmsg("not found user");
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_allocated_param(pHandleMsg->release_param());
				From_Gs_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(WARNING) << "HandleGWLogin/fail userid=" << pHandleMsg->param().userid()
					<< " serverid=" << send.route().gs_id()
					<< " not found user";
				return;
			}

			const int64_t nEffectiveTime = 15 * 60 * 1000; // 有效时间15分钟
			if (userInfo.nLoginTime + nEffectiveTime > TimeUtil::GetCurrentTimeMillis())
			{
				protos::WGLogin send;
				send.set_error(2); // token过期了
				send.set_errmsg("token expired");
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_allocated_param(pHandleMsg->release_param());
				From_Gs_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(WARNING) << "HandleGWLogin/fail userid=" << pHandleMsg->param().userid()
					<< " serverid=" << send.route().gs_id()
					<< " token expired";
				return;
			}

			if (pHandleMsg->param().user_token() != userInfo.strToken)
			{
				protos::WGLogin send;
				send.set_error(3); // token对不上
				send.set_errmsg("token invalid");
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_allocated_param(pHandleMsg->release_param());
				From_Gs_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(WARNING) << "HandleGWLogin/fail userid=" << pHandleMsg->param().userid()
					<< " serverid=" << send.route().gs_id()
					<< " client.user_token=" << send.param().user_token()
					<< " server.user_token=" << userInfo.strToken
					<< " token invalid";
				return;
			}

			PlayerOfUserInfo info;
			info.nUserID = pHandleMsg->param().userid();
			info.nServerID = pHandleMsg->route().gs_id();
			info.nPlayerGUID = 0;
			info.strPlayerName = "";
			info.nPortrait = 0;
			info.nPlayerLevel = 0;
			info.strDeviceID = pHandleMsg->param().deviceid();
			info.strIP = pHandleMsg->param().ip();

			if (LoginWorld(info) == false)
			{
				protos::WGLogin send;
				send.set_error(10); // 重复创建,同时收到多个登陆消息
				send.set_errmsg("repeat create player_of_user");
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_allocated_param(pHandleMsg->release_param());
				send.set_guid(info.nPlayerGUID);
				From_Gs_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(WARNING) << "HandleGWLogin/fail userid=" << info.nUserID
					<< " serverid=" << info.nServerID
					<< " repeat create player_of_user";
				return;
			}

			int64_t nPlayerGUID = info.nPlayerGUID;

			// 生成登陆Key
			std::string strLoginKey = GenSN::T20() + std::to_string(nPlayerGUID % 10000);

			protos::WGLogin send;
			send.set_allocated_route(pHandleMsg->release_route());
			send.set_allocated_param(pHandleMsg->release_param());
			send.set_guid(nPlayerGUID);
			From_Gs_Session::Me()->Send(nSessionID, &send, *pMeta);

			LOG(INFO) << "HandleGWLogin/ok userid=" << info.nUserID
				<< " serverid=" << info.nServerID
				<< " guid=" << nPlayerGUID
				<< " ip=" << pHandleMsg->param().ip();

			// 转到主线程
			WorkDispatcherSync().RunInLoop([=]() {

				int64_t nGwsSessionID = -1;
				if (auto pServer = ServerManager::Me()->GetServerByID(ServerType::Gateway, pHandleMsg->route().gws_id()))
				{
					nGwsSessionID = pServer->GetSessionID();
				}

				std::string strOpenID = std::to_string(nPlayerGUID) + "@" + "world";
				LinkPlayer(nPlayerGUID, nGwsSessionID, strOpenID, strLoginKey, strLoginKey);
			});

		};
	});
}

void LoginHandler::HandleCWReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	/*
	MSG_CHECK_OF(protos::CWReconnLogin);

	auto pPlayer = PlayerManager::Me()->GetPlayerByLoginKey(pHandleMsg->login_key());
	if (!pPlayer)
	{
		protos::WCReconnLogin send;
		send.set_error(1);
		send.set_errmsg("need to login"); // 找不到,需要重新登陆
		send.set_allocated_route(pHandleMsg->release_route());
		From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleCWReconnLogin/fail need to login login_key=" << pHandleMsg->login_key();
		return;
	}

	pPlayer->Reconnect(nSessionID);

	pPlayer->Online();

	protos::WCReconnLogin send;
	send.set_guid(pPlayer->GetPlayerGUID());
	send.set_allocated_route(pHandleMsg->release_route());
	From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleCWReconnLogin/ok guid=" << pPlayer->GetPlayerGUID();
	*/
}

void LoginHandler::HandleGWS2WReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::GWS2WReconnLogin);

	auto pPlayer = PlayerManager::Me()->GetPlayer(pHandleMsg->guid());
	if (!pPlayer)
	{
		protos::W2GWSReconnLogin send;
		send.set_error(1);
		send.set_errmsg("need to login"); // 找不到,需要重新登陆
		From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleGWS2WReconnLogin/fail need to login guid=" << pHandleMsg->guid();
		return;
	}

	pPlayer->Reconnect(nSessionID);

	pPlayer->Online();

	protos::W2GWSReconnLogin send;
	send.set_guid(pPlayer->GetPlayerGUID());
	From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleGWS2WReconnLogin/ok guid=" << pPlayer->GetPlayerGUID();
}

void LoginHandler::HandleGWS2WDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::GWS2WDisconnect);

	auto pPlayer = PlayerManager::Me()->GetPlayer(pHandleMsg->guid());
	if (!pPlayer)
	{
		protos::W2GWSDisconnect send;
		send.set_error(1);
		send.set_errmsg("not found player"); // 没找到玩家
		send.set_guid(pHandleMsg->guid());
		From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleGWS2WDisconnect/fail guid=" << pHandleMsg->guid() << " not found player";
		return;
	}

	pPlayer->Offline();

	protos::W2GWSDisconnect send;
	send.set_guid(pPlayer->GetPlayerGUID());
	From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleGWS2WDisconnect/ok guid=" << pPlayer->GetPlayerGUID();
}
