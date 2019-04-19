//------------------------------------------------------------------------
// * @filename: HttpHandleResult.h
// *
// * @brief: Http请求的处理结果
// *
// * @author: XGM
// * @date: 2018/07/14
//------------------------------------------------------------------------
#pragma once

#include <string>
#include <stdint.h>
#include <functional>

// 解析Header结果
struct HttpHeaderResult
{
	HttpHeaderResult()
	{
	}

	explicit HttpHeaderResult(int32_t _nError, const char* _pszErrMsg)
		: nError(_nError)
		, strErrMsg(_pszErrMsg)
	{
	}

	int32_t nError = 0; // 错误号
	std::string strErrMsg; // 错误信息

	const char* pszCmd = ""; // 命令
	const char* pszVersion = ""; // 参数版本号
	const char* pszSN = ""; // 命令流水号
	const char* pszData = ""; // 数据
	const char* pszSign = ""; // 校验码
};

// 处理结果
struct HttpHandleResult
{
	HttpHandleResult()
	{
	}

	explicit HttpHandleResult(int32_t _nError, const char* _pszErrMsg)
		: nError(_nError)
		, strErrMsg(_pszErrMsg)
	{
	}

	int32_t nError = 0; // 错误号
	std::string strErrMsg; // 错误信息
	std::string strData; // 返回Data
};

// 请求的成功/失败的回应
struct HttpResponseCallback
{
	using SuccessFunc = std::function<void(const char* pszSN, const char* pszData)>;
	using FailFunc = std::function<void(int nError, const char* pszErrMsg)>;

	SuccessFunc funcSuccess; // 成功的回调函数
	FailFunc funcFail; // 失败的回调函数
	std::string strSN; // 命令流水号
};
