
#include "NetworkModule.h"
#include "LinkServer.h"
#include "link/network/from_client_session.h"
#include "link/network/to_ls_session.h"

#include "link/config/SystemConfig.h"
#include "link/pbconfig/link.conf.pb.h"

#include <evpp/event_loop_thread.h>

std::string NetworkModule::GetName()
{
	return "NetworkModule";
}

bool NetworkModule::Init()
{
	auto& netConf = SystemConfig::Me()->GetLinkConfig()->networkmodule();

	To_Ls_Session::InitInstance();
	To_Ls_Session::Me()->Init(LinkServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_ls_remote_addr()/*"127.0.0.1:17001"*/, "(LINK(local) ==> LS)", netConf.to_ls_thread_num()/*1*/, netConf.to_ls_session_num()/*1*/);
	To_Ls_Session::Me()->Start();

	From_Client_Session::InitInstance();
	From_Client_Session::Me()->Init(LinkServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_c_listen_addr()/*"0.0.0.0:7001"*/, "(C ==> LINK(local))", netConf.from_c_thread_num()/*1*/, netConf.from_c_session_num()/*5000*/);
	From_Client_Session::Me()->AuthTimeout() = 10 * 1000; // 连接认证超时时间
	From_Client_Session::Me()->Start();
	return true;
}

void NetworkModule::Exit()
{
	From_Client_Session::Me()->Stop();
	From_Client_Session::DestroyInstance();

	To_Ls_Session::Me()->Stop();
	To_Ls_Session::DestroyInstance();
}

void NetworkModule::RunOnce()
{
	To_Ls_Session::Me()->Update();
	From_Client_Session::Me()->Update();
}
