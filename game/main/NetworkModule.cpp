
#include "NetworkModule.h"
#include "GameServer.h"
#include "game/network/from_client_session.h"
#include "game/network/from_gws_session.h"
#include "game/network/to_ws_session.h"

#include "game/config/SystemConfig.h"
#include "game/pbconfig/game.conf.pb.h"

#include "common/logic/ServerManager.h"

#include <evpp/event_loop_thread.h>

std::string NetworkModule::GetName()
{
	return "NetworkModule";
}

bool NetworkModule::Init()
{
	auto& netConf = SystemConfig::Me()->GetGameConfig()->networkmodule();

	ServerManager::InitInstance();

	To_Ws_Session::InitInstance();
	To_Ws_Session::Me()->Init(GameServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_ws_remote_addr()/*"127.0.0.1:20001"*/, "(GS(local) ==> WS)", netConf.to_ws_thread_num()/*1*/, netConf.to_ws_session_num()/*1*/);
	To_Ws_Session::Me()->Start();

	From_Gws_Session::InitInstance();
	From_Gws_Session::Me()->Init(GameServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_gws_listen_addr()/*"0.0.0.0:10001"*/, "(GWS ==> GS(local))", netConf.from_gws_thread_num()/*1*/, netConf.from_gws_session_num()/*10*/);
	From_Gws_Session::Me()->Start();

	//From_Client_Session::InitInstance();
	//From_Client_Session::Me()->Init(GameServer::Me()->GetLoopDaemonThread()->loop(),
	//	netConf.from_c_listen_addr()/*"0.0.0.0:9501*/, "(C ==> GS(local))", netConf.from_c_thread_num()/*4*/, netConf.from_c_session_num()/*5000*/);
	//From_Client_Session::Me()->AuthTimeout() = 10 * 1000; // 连接认证超时时间
	//From_Client_Session::Me()->Start();
	return true;
}

void NetworkModule::Exit()
{
	//From_Client_Session::Me()->Stop();
	//From_Client_Session::DestroyInstance();

	From_Gws_Session::Me()->Stop();
	From_Gws_Session::DestroyInstance();

	To_Ws_Session::Me()->Stop();
	To_Ws_Session::DestroyInstance();

	ServerManager::DestroyInstance();
}

void NetworkModule::RunOnce()
{
	To_Ws_Session::Me()->Update();
	From_Gws_Session::Me()->Update();
	//From_Client_Session::Me()->Update();
}
