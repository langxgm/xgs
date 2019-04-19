
#include "LoginHandler.h"
#include "game/main/GameServer.h"
#include "game/network/from_gws_session.h"
#include "game/network/to_ws_session.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/PlayerFactory.h"
#include "game/logic/PlayerManager.h"
#include "game/logic/Player.h"
#include "game/logic/t_player.h"

#include "game/protos/login2game.pb.h"
#include "game/protos/gamedata.pb.h"

#include "common/logic/ServerManager.h"
#include "common/logic/ServerNode.h"

#include "xbase/TimeUtil.h"
#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"
#include "xshare/net/GenSN.h"
#include "xshare/work/WorkDispatcher.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <libgo/coroutine.h>

LoginHandler::LoginHandler()
{
	PlayerManager::InitInstance();
	PlayerManager::Me()->Init(0xFFFFFF);
}

LoginHandler::~LoginHandler()
{
	PlayerManager::DestroyInstance();
}

void LoginHandler::PreInitDatabase()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_player::t_name);

	bsoncxx::builder::stream::document indexBuilder;
	indexBuilder << t_player::f_guid << 1;
	auto result = coll.create_index(indexBuilder.view(), mongocxx::options::index().unique(true));
	LOG(WARNING) << "create_index " << t_player::t_name << " " << bsoncxx::to_json(result);
}

void LoginHandler::UpdateLogic()
{
	PlayerManager::Me()->Update();
}

void LoginHandler::ListenFromGws(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleCGLogin, this, protos::CGLogin, "登陆请求");

	REG_MSG_TO(pServer, &LoginHandler::HandleCGReconnLogin, this, protos::CGReconnLogin, "断线重连请求");
	REG_MSG_TO(pServer, &LoginHandler::HandleGWS2GDisconnect, this, protos::GWS2GDisconnect, "断线请求");

	REG_MSG_TO(pServer, &LoginHandler::HandleCGChangeName, this, protos::CGChangeName, "修改玩家名称请求");
}

void LoginHandler::ListenFromWs(ClientWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleWGLogin, this, protos::WGLogin, "登陆请求返回");
}

bool LoginHandler::LoginGame(LoginPlayerInfo& rInfo)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player::f_guid << rInfo.nPlayerGUID;

	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_player::t_name);

	auto findResult = coll.find_one(filterBuilder.view());
	if (!findResult)
	{
		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

		// 创建player
		rInfo.strPlayerName = "player" + std::to_string(rInfo.nPlayerGUID % 1000);
		rInfo.nPortrait = 0;
		rInfo.nPlayerLevel = 0;

		bsoncxx::builder::stream::document createBuilder;
		createBuilder << t_player::f_guid << rInfo.nPlayerGUID
			<< t_player::f_name << rInfo.strPlayerName
			<< t_player::f_portrait << rInfo.nPortrait
			<< t_player::f_level << rInfo.nPlayerLevel
			<< t_player::f_register_time << date
			<< t_player::f_login_time << date
			<< t_player::f_login_count << (int32_t)1
			<< t_player::f_deviceid << rInfo.strDeviceID
			<< t_player::f_ip << rInfo.strIP;

		try
		{
			auto result = coll.insert_one(createBuilder.view());
			LOG_IF(INFO, result) << "create " << t_player::t_name << " guid=" << rInfo.nPlayerGUID
				<< ",result inserted_count=" << result->result().inserted_count();
		}
		catch (const mongocxx::bulk_write_exception& e)
		{
			LOG(ERROR) << "create " << t_player::t_name << " guid=" << rInfo.nPlayerGUID
				<< ",mongocxx::bulk_write_exception e=" << e.what();
			return false;
		}
	}
	else
	{
		rInfo.strPlayerName = MongoElement<b_utf8>::GetValue(findResult->view()[t_player::f_name]).to_string();
		rInfo.nPortrait = MongoElement<b_int32>::GetValue(findResult->view()[t_player::f_portrait]);
		rInfo.nPlayerLevel = MongoElement<b_int32>::GetValue(findResult->view()[t_player::f_level]);

		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

		// 更新登陆IP,时间,次数+1
		bsoncxx::builder::stream::document updateBuilder;
		updateBuilder << "$set"
			<< open_document
			<< t_player::f_login_time << date
			<< t_player::f_ip << rInfo.strIP
			<< close_document
			<< "$inc"
			<< open_document
			<< t_player::f_login_count << (int32_t)1
			<< close_document;

		auto result = coll.update_one(filterBuilder.view(), updateBuilder.view());
		LOG_IF(INFO, result) << "update " << t_player::t_name << " guid=" << rInfo.nPlayerGUID
			<< ",result matched_count=" << result->matched_count()
			<< " modified_count=" << result->modified_count();
	}
	return true;
}

bool LoginHandler::QueryPlayerName(int64_t nPlayerGUID, std::string& rPlayerName, int32_t& rPortraitID)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player::f_guid << nPlayerGUID;

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_player::f_name << 1
		<< t_player::f_portrait << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto findResult = dbExecutor.FindOne(t_player::t_name, filterBuilder.view(), opts);
	//LOG_IF(INFO, findResult) << "query " << t_player::t_name << " guid=" << nPlayerGUID;
	if (findResult)
	{
		rPlayerName = MongoElement<b_utf8>::GetValue(findResult->view()[t_player::f_name]).to_string();
		rPortraitID = MongoElement<b_int32>::GetValue(findResult->view()[t_player::f_portrait]);
		return true;
	}

	return false;
}

bool LoginHandler::QueryMultiPlayerName(const std::vector<int64_t>& rGUIDs,
	const std::function<void(int64_t, std::string&&, int32_t)>& func)
{
	// 查询指定的玩家
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_player::f_guid << open_document;
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
		<< t_player::f_guid << 1
		<< t_player::f_name << 1
		<< t_player::f_portrait << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto cursor = dbExecutor.Find(t_player::t_name, filterBuilder.view(), opts);
	for (auto&& doc : cursor)
	{
		int64_t nPlayerGUID = MongoElement<b_int64>::GetValue(doc[t_player::f_guid]);
		std::string strPlayerName = MongoElement<b_utf8>::GetValue(doc[t_player::f_name]).to_string();
		int32_t nPortraitID = MongoElement<b_int32>::GetValue(doc[t_player::f_portrait]);

		func(nPlayerGUID, std::move(strPlayerName), nPortraitID);
	}

	return true;
}

void LoginHandler::LinkPlayer(int64_t nPlayerGUID, int64_t nSessionID, const std::string& strLoginKey)
{
	auto pPlayer = PlayerManager::Me()->GetPlayer(nPlayerGUID);
	if (!pPlayer)
	{
		pPlayer = PlayerManager::Me()->NewPlayer();
		if (!pPlayer)
		{
			return; // new error
		}

		pPlayer->Init(nPlayerGUID, nSessionID);

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

		PlayerManager::Me()->ModifyMapping(pPlayer, strLoginKey);

		pPlayer->Online();

		LOG(INFO) << "LinkPlayer/ok ModifyMapping guid=" << nPlayerGUID;
	}
}

void LoginHandler::HandleCGLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGLogin);

	protos::GWLogin send;
	send.set_allocated_route(pHandleMsg->release_route());
	send.set_allocated_param(pHandleMsg->release_param());
	send.mutable_route()->set_gs_id(GameServer::Me()->GetID());
	To_Ws_Session::Me()->Send1(&send, To_Ws_Meta());

	LOG(INFO) << "HandleCGLogin/ok ip=" << send.param().ip();
}

void LoginHandler::HandleWGLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::WGLogin);

	int64_t nGwsSessionID = -1;
	if (auto pServer = ServerManager::Me()->GetServerByID(ServerType::Gateway, pHandleMsg->route().gws_id()))
	{
		nGwsSessionID = pServer->GetSessionID();
		return;
	}

	if (pHandleMsg->error() != 0)
	{
		LOG(WARNING) << "HandleWGLogin/fail error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg();
		protos::GCLogin send;
		send.set_error(pHandleMsg->error());
		send.set_allocated_errmsg(pHandleMsg->release_errmsg());
		send.set_allocated_route(pHandleMsg->release_route());
		From_Gws_Session::Me()->Send(nGwsSessionID, &send, From_Gws_Meta());
		return;
	}

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			int64_t nPlayerGUID = pHandleMsg->guid();

			LoginPlayerInfo info;
			info.nPlayerGUID = nPlayerGUID;
			info.strDeviceID = pHandleMsg->param().deviceid();
			info.strIP = pHandleMsg->param().ip();

			if (LoginGame(info) == false)
			{
				protos::GCLogin send;
				send.set_error(1); // 重复创建,同时收到多个登陆消息
				send.set_errmsg("repeat create player");
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_guid(nPlayerGUID);
				From_Gws_Session::Me()->Send(nGwsSessionID, &send, From_Gws_Meta());

				LOG(WARNING) << "HandleWGLogin/fail guid=" << nPlayerGUID << " repeat create player";
				return;
			}

			// 生成登陆Key
			std::string strLoginKey = GenSN::T20() + std::to_string(nPlayerGUID % 10000);

			protos::GCLogin send;
			send.set_allocated_route(pHandleMsg->release_route());
			send.set_login_key(strLoginKey);
			send.set_guid(nPlayerGUID);
			send.set_name(info.strPlayerName);
			send.set_portraitid(info.nPortrait);
			send.set_level(info.nPlayerLevel);
			From_Gws_Session::Me()->Send(nGwsSessionID, &send, From_Gws_Meta());

			LOG(INFO) << "HandleWGLogin/ok guid=" << nPlayerGUID
				<< " ip=" << pHandleMsg->param().ip();

			// 转到主线程
			WorkDispatcherSync().RunInLoop([=]() {

				LinkPlayer(nPlayerGUID, nGwsSessionID, strLoginKey);
			});
		};
	});
}

void LoginHandler::HandleCGReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGReconnLogin);

	auto pPlayer = PlayerManager::Me()->GetPlayerByLoginKey(pHandleMsg->login_key());
	if (!pPlayer)
	{
		protos::GCReconnLogin send;
		send.set_error(1);
		send.set_errmsg("need to login"); // 找不到,需要重新登陆
		send.set_allocated_route(pHandleMsg->release_route());
		From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleCGReconnLogin/fail need to login login_key=" << pHandleMsg->login_key();
		return;
	}

	pPlayer->Reconnect(nSessionID);

	pPlayer->Online();

	protos::GCReconnLogin send;
	send.set_error(0);
	send.set_guid(pPlayer->GetPlayerGUID());
	send.set_allocated_route(pHandleMsg->release_route());
	From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleCGReconnLogin/ok guid=" << pPlayer->GetPlayerGUID();
}

void LoginHandler::HandleGWS2GDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::GWS2GDisconnect);

	auto pPlayer = PlayerManager::Me()->GetPlayer(pHandleMsg->guid());
	if (!pPlayer)
	{
		protos::G2GWSDisconnect send;
		send.set_error(1);
		send.set_errmsg("not found player"); // 没找到玩家
		send.set_guid(pHandleMsg->guid());
		From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleGWS2GDisconnect/fail guid=" << pHandleMsg->guid() << " not found player";
		return;
	}

	pPlayer->Offline();

	protos::G2GWSDisconnect send;
	send.set_guid(pPlayer->GetPlayerGUID());
	From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleGWS2GDisconnect/ok guid=" << pPlayer->GetPlayerGUID();
}

void LoginHandler::HandleCGChangeName(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGChangeName);

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			// 名称截取(64-1)个有效字符
			if (pHandleMsg->name().length() >= t_player::MaxNameLen)
			{
				pHandleMsg->mutable_name()->erase(t_player::MaxNameLen - 1);
			}

			bsoncxx::builder::stream::document filterBuilder;
			filterBuilder << t_player::f_guid << pHandleMsg->guid();

			bsoncxx::builder::stream::document updateBuilder;
			updateBuilder << "$set"
				<< open_document
				<< t_player::f_name << pHandleMsg->name()
				<< t_player::f_portrait << (int32_t)pHandleMsg->portraitid()
				<< close_document;

			// 更新玩家名称和头像
			MongoExecutor dbExecutor;
			auto result = dbExecutor.UpdateOne(t_player::t_name, filterBuilder.view(), updateBuilder.view());
			LOG_IF(INFO, result) << "update " << t_player::t_name << " guid=" << pHandleMsg->guid()
				<< ",result matched_count=" << result->matched_count()
				<< " modified_count=" << result->modified_count();

			protos::GCChangeName send;
			send.set_error(0);
			send.set_guid(pHandleMsg->guid());
			send.set_allocated_name(pHandleMsg->release_name());
			send.set_portraitid(pHandleMsg->portraitid());
			From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);
		};
	});
}
