
#include "NetworkModule.h"
#include "GateServer.h"
#include "gate/network/from_client_httpsession.h"
#include "gate/network/from_client_session.h"
#include "gate/network/to_gs_session.h"
#include "gate/network/to_ws_session.h"

#include "gate/config/SystemConfig.h"
#include "gate/pbconfig/gate.conf.pb.h"

#include <evpp/event_loop_thread.h>

std::string NetworkModule::GetName()
{
	return "NetworkModule";
}

bool NetworkModule::Init()
{
	auto& netConf = SystemConfig::Me()->GetGateConfig()->networkmodule();

	To_Ws_Session::InitInstance();
	To_Ws_Session::Me()->Init(GateServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_ws_remote_addr()/*"127.0.0.1:21001"*/, "(GWS(local) ==> WS)", netConf.to_ws_thread_num()/*1*/, netConf.to_ws_session_num()/*1*/);
	To_Ws_Session::Me()->Start();

	To_Gs_Session::InitInstance();
	To_Gs_Session::Me()->Init(GateServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.to_gs_remote_addr()/*"127.0.0.1:10001"*/, "(GWS(local) ==> GS)", netConf.to_gs_thread_num()/*1*/, netConf.to_gs_session_num()/*1*/);
	To_Gs_Session::Me()->Start();

	From_Client_Session::InitInstance();
	From_Client_Session::Me()->Init(GateServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_c_listen_addr()/*"0.0.0.0:9001*/, "(C ==> GWS(local))", netConf.from_c_thread_num()/*4*/, netConf.from_c_session_num()/*5000*/);
	From_Client_Session::Me()->AuthTimeout() = 10 * 1000; // 连接认证超时时间
	From_Client_Session::Me()->Start();

	From_Client_HttpSession::InitInstance();
	From_Client_HttpSession::Me()->Init(GateServer::Me()->GetLoopDaemonThread()->loop(),
		netConf.from_c_http_listen_ports()/*"9101"*/, netConf.from_c_http_thread_num()/*1*/);
	From_Client_HttpSession::Me()->Start();
	return true;
}

void NetworkModule::Exit()
{
	From_Client_HttpSession::Me()->Stop();
	From_Client_HttpSession::DestroyInstance();

	From_Client_Session::Me()->Stop();
	From_Client_Session::DestroyInstance();

	To_Gs_Session::Me()->Stop();
	To_Gs_Session::DestroyInstance();

	To_Ws_Session::Me()->Stop();
	To_Ws_Session::DestroyInstance();
}

void NetworkModule::RunOnce()
{
	To_Ws_Session::Me()->Update();
	To_Gs_Session::Me()->Update();
	From_Client_Session::Me()->Update();
	From_Client_HttpSession::Me()->Update();
}
