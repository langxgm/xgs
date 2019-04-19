//------------------------------------------------------------------------
// * @filename: SystemConfig.h
// *
// * @brief: 系统配置
// *
// * @author: XGM
// * @date: 2018/05/10
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
	class GateConfig;
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
	// 网关服务器配置
	//------------------------------------------------------------------------
	bool InitGateConfig(const std::string& strFileName, Reader& r);
	const pbconfig::GateConfig* GetGateConfig() { return m_pGateConfig; }

	//------------------------------------------------------------------------
	// 服务器信息
	//------------------------------------------------------------------------
	bool InitServerInfo(const std::string& strFileName, Reader& r);
	const pbconfig::ServerInfo* GetServerInfo() { return m_pServerInfo; }

private:
	const pbconfig::GateConfig* m_pGateConfig = nullptr;
	const pbconfig::ServerInfo* m_pServerInfo = nullptr;
};
