//------------------------------------------------------------------------
// * @filename: SystemConfig.h
// *
// * @brief: 系统配置
// *
// * @author: XGM
// * @date: 2017/12/11
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"

#include "xshare/config/ConfigReader.h"

#include <string>

namespace google {
	namespace protobuf {
		class Message;
	}
}

namespace pbconfig {
	class SdkConfig;
	class ServerInfo;
}

class SystemConfig : public Singleton<SystemConfig>
{
public:
	typedef ConfigReader<google::protobuf::Message*, const google::protobuf::Message*> Reader;
protected:
	friend class Singleton<SystemConfig>;
	SystemConfig() {}
	virtual ~SystemConfig();
public:
	//------------------------------------------------------------------------
	// SDK服务器配置
	//------------------------------------------------------------------------
	bool InitSdkConfig(const std::string& strFileName, Reader& r);
	const pbconfig::SdkConfig* GetSdkConfig() { return m_pSdkConfig; }

	//------------------------------------------------------------------------
	// 服务器信息
	//------------------------------------------------------------------------
	bool InitServerInfo(const std::string& strFileName, Reader& r);
	const pbconfig::ServerInfo* GetServerInfo() { return m_pServerInfo; }

private:
	const pbconfig::SdkConfig* m_pSdkConfig = nullptr;
	const pbconfig::ServerInfo* m_pServerInfo = nullptr;
};
