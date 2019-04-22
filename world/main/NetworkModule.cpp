
#include "NetworkModule.h"
#include "WorldServer.h"
#include "world/network/from_gws_session.h"
#include "world/network/from_gs_session.h"
#include "world/network/to_sdk_session.h"

#include "world/config/SystemConfig.h"
#include "world/pbconfig/world.conf.pb.h"

#include "common/logic/ServerManager.h"

#include <evpp/event_loop_thread.h>

std::string NetworkModule::GetName()
{
	return "NetworkModule";
}

bool NetworkModule::Init()
{
	auto& netConf = SystemConfig::Me()->GetWorldConfig()->networkmodule();

	ServerManager::InitInstance();

	From_Gws_Session::InitInstance();
	From_Gws_Session::Me()->Init(WorldServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_gws_listen_addr()/*"0.0.0.0:21001"*/, "(GWS ==> WS(local))", netConf.from_gws_thread_num()/*1*/, netConf.from_gws_session_num()/*10*/);
	From_Gws_Session::Me()->MaxDealPerFrame() = 500;
	From_Gws_Session::Me()->Start();

	From_Gs_Session::InitInstance();
	From_Gs_Session::Me()->Init(WorldServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_gs_listen_addr()/*"0.0.0.0:20001"*/, "(GS ==> WS(local))", netConf.from_gs_thread_num()/*1*/, netConf.from_gs_session_num()/*10*/);
	From_Gs_Session::Me()->Start();

	To_Sdk_Session::InitInstance();
	To_Sdk_Session::Me()->Init(WorldServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_sdk_remote_addr()/*"127.0.0.1:30001"*/, "(WS(local) ==> SDK)", netConf.to_sdk_thread_num()/*1*/, netConf.to_sdk_session_num()/*1*/);
	To_Sdk_Session::Me()->Start();
	return true;
}

void NetworkModule::Exit()
{
	To_Sdk_Session::Me()->Stop();
	To_Sdk_Session::DestroyInstance();

	From_Gs_Session::Me()->Stop();
	From_Gs_Session::DestroyInstance();

	From_Gws_Session::Me()->Stop();
	From_Gws_Session::DestroyInstance();

	ServerManager::DestroyInstance();
}

void NetworkModule::RunOnce()
{
	From_Gws_Session::Me()->Update();
	From_Gs_Session::Me()->Update();
	To_Sdk_Session::Me()->Update();
}
