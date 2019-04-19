//------------------------------------------------------------------------
// * @filename: SystemConfig.h
// *
// * @brief: 系统配置
// *
// * @author: XGM
// * @date: 2018/12/27
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
	class LinkConfig;
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
	bool InitLinkConfig(const std::string& strFileName, Reader& r);
	const pbconfig::LinkConfig* GetLinkConfig() { return m_pLinkConfig; }

private:
	const pbconfig::LinkConfig* m_pLinkConfig = nullptr;
};
