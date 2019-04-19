//------------------------------------------------------------------------
// * @filename: from_plat_httpsession.h
// *
// * @brief: 来自平台的HTTP请求
// *
// * @author: XGM
// * @date: 2017/11/27
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/http/HttpServerWorker.h"

class From_Plat_HttpSession : public HttpServerWorker, public Singleton<From_Plat_HttpSession>
{
protected:
	friend class Singleton<From_Plat_HttpSession>;
	From_Plat_HttpSession();
	virtual ~From_Plat_HttpSession();
public:
	//------------------------------------------------------------------------
	// 注册回调
	//------------------------------------------------------------------------
	virtual void RegisterCallback() override;

public:
	void HandleDefault(evpp::EventLoop* loop, const evpp::http::ContextPtr& ctx, const evpp::http::HTTPSendResponseCallback& respcb);
};
