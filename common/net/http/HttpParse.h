//------------------------------------------------------------------------
// * @filename: HttpParse.h
// *
// * @brief: 解析HTTP参数
// *
// * @author: XGM
// * @date: 2018/07/14
//------------------------------------------------------------------------
#pragma once

#include "HttpHandleResult.h"

struct evkeyvalq;

class HttpParse
{
public:
	//------------------------------------------------------------------------
	// 解析URI
	//------------------------------------------------------------------------
	static HttpHeaderResult ParseURI(const char* pszURI, evkeyvalq* pKVs, const std::string& strGameKey);
};
