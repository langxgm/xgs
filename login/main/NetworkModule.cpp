
#include "NetworkModule.h"
#include "LoginServer.h"
#include "login/network/from_link_session.h"
#include "login/network/to_sdk_session.h"

#include "login/config/SystemConfig.h"
#include "login/pbconfig/login.conf.pb.h"

#include <evpp/event_loop_thread.h>

std::string NetworkModule::GetName()
{
	return "NetworkModule";
}

bool NetworkModule::Init()
{
	auto& netConf = SystemConfig::Me()->GetLoginConfig()->networkmodule();

	From_Link_Session::InitInstance();
	From_Link_Session::Me()->Init(LoginServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_link_listen_addr()/*"0.0.0.0:17001"*/, "(LINK ==> LS(local))", netConf.from_link_thread_num()/*1*/, netConf.from_link_session_num()/*10*/);
	From_Link_Session::Me()->Start();

	To_Sdk_Session::InitInstance();
	To_Sdk_Session::Me()->Init(LoginServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_sdk_remote_addr()/*"127.0.0.1:31001"*/, "(LS(local) ==> SDK)", netConf.to_sdk_thread_num()/*1*/, netConf.to_sdk_session_num()/*1*/);
	To_Sdk_Session::Me()->Start();
	return true;
}

void NetworkModule::Exit()
{
	To_Sdk_Session::Me()->Stop();
	To_Sdk_Session::DestroyInstance();

	From_Link_Session::Me()->Stop();
	From_Link_Session::DestroyInstance();
}

void NetworkModule::RunOnce()
{
	From_Link_Session::Me()->Update();
	To_Sdk_Session::Me()->Update();
}
