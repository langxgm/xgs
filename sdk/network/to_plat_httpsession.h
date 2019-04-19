//------------------------------------------------------------------------
// * @filename: to_plat_httpsession.h
// *
// * @brief: 连接平台的HTTP请求
// *
// * @author: XGM
// * @date: 2017/11/14
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/http/HttpClientWorker.h"

class To_Plat_HttpSession : public HttpClientWorker, public Singleton<To_Plat_HttpSession>
{
protected:
	friend class Singleton<To_Plat_HttpSession>;
	To_Plat_HttpSession();
	virtual ~To_Plat_HttpSession();
public:
};
