
#include "HttpParse.h"

#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <evpp/logging.h>
#include <third_party/crypto/md5/md5.h>

#include <string.h>
#ifndef WIN32
#define stricmp strcasecmp
#endif

HttpHeaderResult HttpParse::ParseURI(const char* pszURI, evkeyvalq* pKVs, const std::string& strGameKey)
{
	// 获得键值对
	if (evhttp_parse_query(pszURI, pKVs) != 0)
	{
		return HttpHeaderResult{ -100, "parse error" };
	}

	// 命令编号
	const char* pszCmd = evhttp_find_header(pKVs, "cmd");
	if (pszCmd == nullptr)
	{
		return HttpHeaderResult{ -200, "lack param cmd" }; // 缺少参数cmd
	}

	// 参数版本号
	const char* pszVersion = evhttp_find_header(pKVs, "version");
	if (pszVersion == nullptr)
	{
		return HttpHeaderResult{ -210, "lack param version" }; // 缺少参数version
	}
	else
	{
		const std::string strVersion = "1";
		if (strVersion != pszVersion)
		{
			std::ostringstream oss;
			oss << "version error, recv.version=" << pszVersion
				<< " not equal config.version=" << strVersion; // 接收的参数版本号与配置的不一致
			std::string err = oss.str();
			LOG(ERROR) << err;
			return HttpHeaderResult{ -211, err.c_str() };
		}
	}

	// 命令流水号
	const char* pszSN = evhttp_find_header(pKVs, "sn");
	if (pszSN == nullptr)
	{
		return HttpHeaderResult{ -220, "lack param sn" }; // 缺少参数sn
	}

	// 数据
	const char* pszData = evhttp_find_header(pKVs, "data");
	if (pszData == nullptr)
	{
		return HttpHeaderResult{ -230, "lack param data" }; // 缺少参数data
	}

	// 校验码
	const char* pszSign = evhttp_find_header(pKVs, "sign");
	if (pszSign == nullptr)
	{
		return HttpHeaderResult{ -240, "lack param sign" }; // 缺少参数sign
	}

	// md5验证数据
	if (strGameKey.length() > 0)
	{
		MD5 md5(strGameKey + pszCmd + pszVersion + pszSN + pszData);
		if (stricmp(md5.toString().c_str(), pszSign) != 0)
		{
			std::ostringstream oss;
			oss << "HTTP-Handle: sign error, md5.sign=" << md5.toString()
				<< " not equal recv.sign=" << pszSign;
			std::string err = oss.str();
			LOG(ERROR) << err;
			return HttpHeaderResult{ -241, err.c_str() }; // 接收的校验码与md5计算出来的不一致
		}
	}

	HttpHeaderResult result;
	result.nError = 0;
	result.pszCmd = pszCmd;
	result.pszVersion = pszVersion;
	result.pszSN = pszSN;
	result.pszData = pszData;
	result.pszSign = pszSign;
	return std::move(result);
}
