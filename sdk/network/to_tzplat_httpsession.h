//------------------------------------------------------------------------
// * @filename: to_tzplat_httpsession.h
// *
// * @brief: 连接TZ平台的HTTP请求
// *
// * @author: XGM
// * @date: 2018/11/07
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/http/HttpClientWorker.h"

class To_TZPlat_HttpSession : public HttpClientWorker, public Singleton<To_TZPlat_HttpSession>
{
protected:
	friend class Singleton<To_TZPlat_HttpSession>;
	To_TZPlat_HttpSession();
	virtual ~To_TZPlat_HttpSession();
public:
};
