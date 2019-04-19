//------------------------------------------------------------------------
// * @filename: from_client_httpsession.h
// *
// * @brief: 来自客户端的HTTP请求
// *
// * @author: XGM
// * @date: 2018/09/03
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/http/HttpServerWorker.h"

class From_Client_HttpSession : public HttpServerWorker, public Singleton<From_Client_HttpSession>
{
protected:
	friend class Singleton<From_Client_HttpSession>;
	From_Client_HttpSession();
	virtual ~From_Client_HttpSession();
public:
	//------------------------------------------------------------------------
	// 注册回调
	//------------------------------------------------------------------------
	virtual void RegisterCallback() override;

public:
	void HandleDefault(evpp::EventLoop* loop, const evpp::http::ContextPtr& ctx, const evpp::http::HTTPSendResponseCallback& respcb);
};
