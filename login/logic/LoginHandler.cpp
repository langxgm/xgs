
#include "LoginHandler.h"
#include "login/network/from_link_session.h"
#include "login/network/to_sdk_session.h"
#include "login/logic/HandleMsgMacros.h"
#include "login/logic/UserFactory.h"
#include "login/logic/UserManager.h"
#include "login/logic/User.h"

#include "login/protos/login.pb.h"
#include "login/protos/login2sdk.pb.h"

#include "common/logic/t_user.h"
#include "common/logic/GenGUID.h"

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
	UserManager::InitInstance();
	UserManager::Me()->Init(0x7FFFFFF);
}

LoginHandler::~LoginHandler()
{
	UserManager::DestroyInstance();
}

void LoginHandler::PreInitDatabase()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_user::t_name);

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_user::f_account << 1;
		auto result = coll.create_index(indexBuilder.view(), mongocxx::options::index().unique(true));
		LOG(WARNING) << "create_index " << t_user::t_name << " " << bsoncxx::to_json(result);
	}

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_user::f_userid << 1;
		auto result = coll.create_index(indexBuilder.view(), mongocxx::options::index().unique(true));
		LOG(WARNING) << "create_index " << t_user::t_name << " " << bsoncxx::to_json(result);
	}

	{
		bsoncxx::builder::stream::document indexBuilder;
		indexBuilder << t_user::f_openid << 1;
		auto result = coll.create_index(indexBuilder.view());
		LOG(WARNING) << "create_index " << t_user::t_name << " " << bsoncxx::to_json(result);
	}
}

void LoginHandler::UpdateLogic()
{

}

void LoginHandler::ListenFromLink(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLogin, this, protos::CLLogin, "登陆请求");
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLoginWeixin, this, protos::CLLoginWeixin, "登陆请求(微信平台)");
	REG_MSG_TO(pServer, &LoginHandler::HandleCLLoginFacebook, this, protos::CLLoginFacebook, "登陆请求(Facebook平台)");

	REG_MSG_TO(pServer, &LoginHandler::HandleCLReconnLogin, this, protos::CLReconnLogin, "断线重连请求");
	REG_MSG_TO(pServer, &LoginHandler::HandleLink2LDisconnect, this, protos::Link2LDisconnect, "断线请求");
}

void LoginHandler::ListenFromSdk(ClientWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleSDK2LLoginWeixin, this, protos::SDK2LLoginWeixin, "SDK登陆回应(微信平台)");
}

bool LoginHandler::Login(UserInfo& rUserInfo)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_user::f_account << rUserInfo.strAccount;

	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_user::t_name);

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_user::f_userid << 1
		<< finalize);

	int64_t nUserID = 0;
	auto findResult = coll.find_one(filterBuilder.view(), opts);
	if (!findResult)
	{
		nUserID = GenGUID::GenUserID();

		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

		// 创建user
		bsoncxx::builder::stream::document createBuilder;
		createBuilder << t_user::f_userid << nUserID
			<< t_user::f_account << rUserInfo.strAccount
			<< t_user::f_register_time << date
			<< t_user::f_login_time << date
			<< t_user::f_login_count << (int32_t)1
			<< t_user::f_deviceid << rUserInfo.strDeviceID
			<< t_user::f_ip << rUserInfo.strIP
			<< t_user::f_openid << rUserInfo.strOpenID;

		try
		{
			auto result = coll.insert_one(createBuilder.view());
			LOG_IF(INFO, result) << "create " << t_user::t_name << " userid=" << nUserID
				<< " account=" << rUserInfo.strAccount
				<< ",result inserted_count=" << result->result().inserted_count();
		}
		catch (const mongocxx::bulk_write_exception& e)
		{
			LOG(ERROR) << "create " << t_user::t_name << " userid=" << nUserID
				<< " account=" << rUserInfo.strAccount
				<< ",mongocxx::bulk_write_exception e=" << e.what();
			return false;
		}
	}
	else
	{
		nUserID = MongoElement<b_int64>::GetValue(findResult->view()[t_user::f_userid]);

		bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

		// 更新登陆IP,时间,次数+1
		bsoncxx::builder::stream::document updateBuilder;
		updateBuilder << "$set"
			<< open_document
			<< t_user::f_login_time << date
			<< t_user::f_ip << rUserInfo.strIP
			<< close_document
			<< "$inc"
			<< open_document
			<< t_user::f_login_count << (int32_t)1
			<< close_document;

		auto result = coll.update_one(filterBuilder.view(), updateBuilder.view());
		LOG_IF(INFO, result) << "update " << t_user::t_name << " userid=" << nUserID
			<< " account=" << rUserInfo.strAccount
			<< ",result matched_count=" << result->matched_count()
			<< " modified_count=" << result->modified_count();
	}

	rUserInfo.nUserID = nUserID;
	return true;
}

bool LoginHandler::QueryAccount(int64_t nUserID, std::string& rAccount)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_user::f_userid << nUserID;

	mongocxx::options::find opts;
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_user::f_account << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto findResult = dbExecutor.FindOne(t_user::t_name, filterBuilder.view(), opts);
	if (findResult)
	{
		rAccount = MongoElement<b_utf8>::GetValue(findResult->view()[t_user::f_account]).to_string();
		return true;
	}

	return false;
}

bool LoginHandler::QueryUserIDByOpenID(const std::vector<std::string>& vecOpenID, const std::function<void(std::string&&, int64_t)>& func)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_user::f_openid << open_document;
	auto arrayBuilder = filterBuilder << "$in" << open_array;
	for (auto& openID : vecOpenID)
	{
		arrayBuilder << openID;
	}
	arrayBuilder << close_array;
	filterBuilder << close_document;

	mongocxx::options::find opts;
	opts.limit(vecOpenID.size());
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_user::f_userid << 1
		<< t_user::f_openid << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto cursor = dbExecutor.Find(t_user::t_name, filterBuilder.view(), opts);
	for (auto&& doc : cursor)
	{
		int64_t nUserID = MongoElement<b_int64>::GetValue(doc[t_user::f_userid]);
		std::string strOpenID = MongoElement<b_utf8>::GetValue(doc[t_user::f_openid]).to_string();

		func(std::move(strOpenID), nUserID);
	}

	return true;
}

bool LoginHandler::QueryOpenIDByUserID(const std::vector<int64_t>& vecUserID, const std::function<void(std::string&&, int64_t)>& func)
{
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << t_user::f_userid << open_document;
	auto arrayBuilder = filterBuilder << "$in" << open_array;
	for (auto& nUserID : vecUserID)
	{
		arrayBuilder << nUserID;
	}
	arrayBuilder << close_array;
	filterBuilder << close_document;

	mongocxx::options::find opts;
	opts.limit(vecUserID.size());
	opts.projection(bsoncxx::builder::stream::document{}
		<< t_user::f_userid << 1
		<< t_user::f_openid << 1
		<< finalize);

	MongoExecutor dbExecutor;
	auto cursor = dbExecutor.Find(t_user::t_name, filterBuilder.view(), opts);
	for (auto&& doc : cursor)
	{
		int64_t nUserID = MongoElement<b_int64>::GetValue(doc[t_user::f_userid]);
		std::string strOpenID = MongoElement<b_utf8>::GetValue(doc[t_user::f_openid]).to_string();

		func(std::move(strOpenID), nUserID);
	}

	return true;
}

void LoginHandler::LinkUser(int64_t nUserID, int64_t nSessionID, const std::string& strOpenID,
	const std::string& strSessionKey, const std::string& strLoginKey)
{
	UserPtr pUser = UserManager::Me()->GetUser(nUserID);
	if (!pUser)
	{
		auto pUserFactory = UserManager::Me()->GetUserFactory();
		if (!pUserFactory)
		{
			assert(false);
			return;
		}

		pUser.reset(pUserFactory->NewUser());
		if (!pUser)
		{
			return; // new error
		}

		pUser->Init(nUserID, nSessionID);

		pUser->SetLoginKey(strLoginKey);

		pUser->SetOpenID(strOpenID);
		pUser->SetSessionKey(strSessionKey);

		if (UserManager::Me()->AddUser(nUserID, pUser))
		{
			pUser->Online();

			LOG(INFO) << "LinkUser/ok AddUser userid=" << nUserID
				<< " userNum=" << UserManager::Me()->GetUserNum() << " userLimit=" << UserManager::Me()->GetUserLimit();
		}
		else
		{
			LOG(WARNING) << "LinkUser/fail AddUser userid=" << nUserID << " user full"
				<< " userNum=" << UserManager::Me()->GetUserNum() << " userLimit=" << UserManager::Me()->GetUserLimit();
		}
	}
	else
	{
		pUser->Reconnect(nSessionID);

		UserManager::Me()->ModifyMapping(pUser, strOpenID, strSessionKey, strLoginKey);

		pUser->Online();

		LOG(INFO) << "LinkUser/ok ModifyMapping userid=" << nUserID;
	}
}

void LoginHandler::HandleCLLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLogin);

	size_t nHash = std::hash<std::string>()(pHandleMsg->deviceid());
	WorkDispatcherAsync(nHash).RunInLoop([=]() {
		go[=]() {

			// 游客登陆
			std::string strAccount = pHandleMsg->deviceid() + "@" + "guest";

			UserInfo userInfo;
			userInfo.nUserID = 0;
			userInfo.strAccount = strAccount;
			userInfo.strDeviceID = pHandleMsg->deviceid();
			userInfo.strIP = pHandleMsg->ip();
			userInfo.strOpenID = "guest-" + pHandleMsg->deviceid();

			if (Login(userInfo) == false)
			{
				protos::LCLogin send;
				send.set_error(1); // 重复创建账号,同时收到多个登陆消息
				send.set_errmsg("repeat create user");
				send.set_userid(userInfo.nUserID);
				send.set_client_sessionid(pHandleMsg->client_sessionid());
				From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

				LOG(WARNING) << "HandleCLLogin/fail account=" << strAccount << " repeat create user";
				return;
			}

			int64_t nUserID = userInfo.nUserID;

			// 生成登陆Key
			std::string strLoginKey = GenSN::T20() + std::to_string(nUserID % 10000);

			protos::LCLogin send;
			send.set_error(0);
			send.set_userid(nUserID);
			send.set_client_sessionid(pHandleMsg->client_sessionid());
			send.set_login_key(strLoginKey);
			From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

			LOG(INFO) << "HandleCLLogin/ok userid=" << nUserID
				<< " ip=" << pHandleMsg->ip();

			// 转到主线程
			WorkDispatcherSync().RunInLoop([=]() {

				std::string strOpenID = std::to_string(nUserID) + "@" + "guest";
				LinkUser(nUserID, nSessionID, strOpenID, strLoginKey, strLoginKey);
			});
		};
	});
}

void LoginHandler::HandleCLLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLoginWeixin);

	protos::L2SDKLoginWeixin send;
	send.set_allocated_route(pHandleMsg->release_route());
	send.set_link_sessionid(nSessionID);
	send.set_allocated_param(pHandleMsg->release_param());
	To_Sdk_Session::Me()->Send1(&send);

	LOG(INFO) << "HandleCLLoginWeixin/ok ip=" << send.param().ip();
}

void LoginHandler::HandleSDK2LLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::SDK2LLoginWeixin);

	if (pHandleMsg->error() != 0)
	{
		LOG(WARNING) << "HandleSDK2LLoginWeixin/fail error=" << pHandleMsg->error() << " errmsg=" << pHandleMsg->errmsg();
		// 发送错误
		protos::LCLoginWeixin send;
		send.set_error(pHandleMsg->error());
		send.set_allocated_errmsg(pHandleMsg->release_errmsg());
		send.set_userid(0);
		send.set_allocated_route(pHandleMsg->release_route());
		send.set_wx_errcode(pHandleMsg->wx_errcode());
		send.set_allocated_wx_errmsg(pHandleMsg->release_wx_errmsg());
		From_Link_Session::Me()->Send(pHandleMsg->link_sessionid(), &send, From_Link_Meta());
		return;
	}

	size_t nHash = std::hash<std::string>()(pHandleMsg->wx_openid());
	WorkDispatcherAsync(nHash).RunInLoop([=]() {
		go[=]() {

			// 微信平台登陆
			std::string strAccount = pHandleMsg->wx_openid() + "@" + "wx";

			UserInfo userInfo;
			userInfo.nUserID = 0;
			userInfo.strAccount = strAccount;
			userInfo.strDeviceID = pHandleMsg->param().deviceid();
			userInfo.strIP = pHandleMsg->param().ip();
			userInfo.strOpenID = pHandleMsg->wx_openid();

			if (Login(userInfo) == false)
			{
				protos::LCLoginWeixin send;
				send.set_error(1); // 重复创建账号
				send.set_errmsg("repeat create user");
				send.set_userid(userInfo.nUserID);
				send.set_allocated_route(pHandleMsg->release_route());
				From_Link_Session::Me()->Send(pHandleMsg->link_sessionid(), &send, From_Link_Meta());

				LOG(WARNING) << "HandleSDK2LLoginWeixin/fail account=" << strAccount << " repeat create user";
				return;
			}

			int64_t nUserID = userInfo.nUserID;

			// 生成登陆Key
			std::string strLoginKey = GenSN::T20() + std::to_string(nUserID % 10000);

			protos::LCLoginWeixin send;
			send.set_error(0);
			send.set_userid(nUserID);
			send.set_allocated_route(pHandleMsg->release_route());
			send.set_login_key(strLoginKey);
			send.set_wx_openid(pHandleMsg->wx_openid());
			From_Link_Session::Me()->Send(pHandleMsg->link_sessionid(), &send, From_Link_Meta());

			LOG(INFO) << "HandleSDK2LLoginWeixin/ok userid=" << nUserID
#ifdef _DEBUG
				<< " login_key=" << strLoginKey
#endif
				<< " ip=" << pHandleMsg->param().ip();

			// 转到主线程
			WorkDispatcherSync().RunInLoop([=]() {

				LinkUser(nUserID, pHandleMsg->link_sessionid(), pHandleMsg->wx_openid(),
					pHandleMsg->wx_session_key(), strLoginKey);
			});
		};
	});
}

void LoginHandler::HandleCLLoginFacebook(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLLoginFacebook);

	size_t nHash = std::hash<std::string>()(pHandleMsg->param().deviceid());
	WorkDispatcherAsync(nHash).RunInLoop([=]() {
		go[=]() {

			// facebook平台登陆
			std::string strAccount = pHandleMsg->param().code() + "@" + "fb";

			UserInfo userInfo;
			userInfo.nUserID = 0;
			userInfo.strAccount = strAccount;
			userInfo.strDeviceID = pHandleMsg->param().deviceid();
			userInfo.strIP = pHandleMsg->param().ip();
			userInfo.strOpenID = pHandleMsg->param().code();

			if (Login(userInfo) == false)
			{
				protos::LCLoginFacebook send;
				send.set_error(1); // 重复创建账号
				send.set_errmsg("repeat create user");
				send.set_userid(userInfo.nUserID);
				send.set_allocated_route(pHandleMsg->release_route());
				From_Link_Session::Me()->Send(nSessionID, &send, From_Link_Meta());

				LOG(WARNING) << "HandleCLLoginFacebook/fail account=" << strAccount << " repeat create user";
				return;
			}

			int64_t nUserID = userInfo.nUserID;

			// 生成登陆Key
			std::string strLoginKey = GenSN::T20() + std::to_string(nUserID % 10000);

			protos::LCLoginFacebook send;
			send.set_error(0);
			send.set_userid(nUserID);
			send.set_allocated_route(pHandleMsg->release_route());
			send.set_login_key(strLoginKey);
			From_Link_Session::Me()->Send(nSessionID, &send, From_Link_Meta());

			LOG(INFO) << "HandleCLLoginFacebook/ok userid=" << nUserID
#ifdef _DEBUG
				<< " login_key=" << strLoginKey
#endif
				<< " ip=" << pHandleMsg->param().ip();

			// 转到主线程
			WorkDispatcherSync().RunInLoop([=]() {

				std::string strOpenID = pHandleMsg->param().code() + "@" + "fb";
				LinkUser(nUserID, nSessionID, strOpenID, strLoginKey, strLoginKey);
			});
		};
	});
}

void LoginHandler::HandleCLReconnLogin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CLReconnLogin);

	auto pUser = UserManager::Me()->GetUserByLoginKey(pHandleMsg->login_key());
	if (!pUser)
	{
		protos::LCReconnLogin send;
		send.set_error(1);
		send.set_errmsg("need to login"); // 找不到,需要重新登陆
		send.set_allocated_route(pHandleMsg->release_route());
		From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleCLReconnLogin/fail need to login login_key=" << pHandleMsg->login_key();
		return;
	}

	pUser->Reconnect(nSessionID);

	pUser->Online();

	protos::LCReconnLogin send;
	send.set_error(0);
	send.set_userid(pUser->GetUserID());
	send.set_allocated_route(pHandleMsg->release_route());
	From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleCLReconnLogin/ok userid=" << pUser->GetUserID();
}

void LoginHandler::HandleLink2LDisconnect(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::Link2LDisconnect);

	auto pUser = UserManager::Me()->GetUser(pHandleMsg->userid());
	if (!pUser)
	{
		protos::L2LinkDisconnect send;
		send.set_error(1);
		send.set_errmsg("not found user"); // 没找到用户
		send.set_userid(pHandleMsg->userid());
		From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

		LOG(INFO) << "HandleLink2LDisconnect/fail userid=" << pHandleMsg->userid() << " not found user";
		return;
	}

	pUser->Offline();

	protos::L2LinkDisconnect send;
	send.set_userid(pUser->GetUserID());
	From_Link_Session::Me()->Send(nSessionID, &send, *pMeta);

	LOG(INFO) << "HandleLink2LDisconnect/ok userid=" << pUser->GetUserID();
}
