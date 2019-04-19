
#include "NetworkModule.h"
#include "SdkServer.h"
#include "sdk/network/from_ws_session.h"
#include "sdk/network/from_ls_session.h"
#include "sdk/network/to_plat_httpsession.h"
#include "sdk/network/to_tzplat_httpsession.h"
#include "sdk/network/from_plat_httpsession.h"

#include "sdk/config/SystemConfig.h"
#include "sdk/pbconfig/sdk.conf.pb.h"

#include <evpp/event_loop_thread.h>

std::string NetworkModule::GetName()
{
	return "NetworkModule";
}

bool NetworkModule::Init()
{
	auto& netConf = SystemConfig::Me()->GetSdkConfig()->networkmodule();

	if (netConf.to_plat_http_cert_filename().length() > 0)
	{
		x_ssl_init_once();
		x_ssl_certificate(netConf.to_plat_http_cert_filename().c_str());
	}

	From_Ws_Session::InitInstance();
	From_Ws_Session::Me()->Init(SdkServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_ws_listen_addr()/*"0.0.0.0:30001"*/, "(WS ==> SDK(local))", netConf.from_ws_thread_num()/*1*/, netConf.from_ws_session_num()/*1*/);
	From_Ws_Session::Me()->Start();

	From_Ls_Session::InitInstance();
	From_Ls_Session::Me()->Init(SdkServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_ls_listen_addr()/*"0.0.0.0:31001"*/, "(LS ==> SDK(local))", netConf.from_ls_thread_num()/*1*/, netConf.from_ls_session_num()/*1*/);
	From_Ls_Session::Me()->Start();

	To_Plat_HttpSession::InitInstance();
	To_Plat_HttpSession::Me()->Init(SdkServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_plat_http_host()/*"api.weixin.qq.com"*/, netConf.to_plat_http_port()/*443*/, netConf.to_plat_http_cert_filename().length() > 0/*true*/,
		netConf.to_plat_http_thread_num()/*2*/, netConf.to_plat_http_max_conn_pool()/*100*/, netConf.to_plat_http_timeout()/*2.0*/);
	To_Plat_HttpSession::Me()->Start();

	To_TZPlat_HttpSession::InitInstance();
	To_TZPlat_HttpSession::Me()->Init(SdkServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_tzplat_http_host()/*"api.tz.com"*/, netConf.to_tzplat_http_port()/*80*/, netConf.to_tzplat_http_cert_filename().length() > 0/*false*/,
		netConf.to_tzplat_http_thread_num()/*2*/, netConf.to_tzplat_http_max_conn_pool()/*100*/, netConf.to_tzplat_http_timeout()/*2.0*/);
	To_TZPlat_HttpSession::Me()->Start();

	//From_Plat_HttpSession::InitInstance();
	//From_Plat_HttpSession::Me()->Init(SdkServer::Me()->GetLoopDaemonThread()->loop(),
	//	netConf.from_plat_listen_ports()/*"30080"*/, netConf.from_plat_thread_num()/*1*/);
	//From_Plat_HttpSession::Me()->Start();
	return true;
}

void NetworkModule::Exit()
{
	//From_Plat_HttpSession::Me()->Stop();
	//From_Plat_HttpSession::DestroyInstance();

	To_TZPlat_HttpSession::Me()->Stop();
	To_TZPlat_HttpSession::DestroyInstance();

	To_Plat_HttpSession::Me()->Stop();
	To_Plat_HttpSession::DestroyInstance();

	From_Ls_Session::Me()->Stop();
	From_Ls_Session::DestroyInstance();

	From_Ws_Session::Me()->Stop();
	From_Ws_Session::DestroyInstance();

	if (x_ssl_ctx() != nullptr)
	{
		x_ssl_clean();
	}
}

void NetworkModule::RunOnce()
{
	From_Ws_Session::Me()->Update();
	From_Ls_Session::Me()->Update();
	To_Plat_HttpSession::Me()->Update();
	To_TZPlat_HttpSession::Me()->Update();
	//From_Plat_HttpSession::Me()->Update();
}
