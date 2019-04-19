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
	class WorldConfig;
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
	// 世界服务器配置
	//------------------------------------------------------------------------
	bool InitWorldConfig(const std::string& strFileName, Reader& r);
	const pbconfig::WorldConfig* GetWorldConfig() { return m_pWorldConfig; }

private:
	const pbconfig::WorldConfig* m_pWorldConfig = nullptr;
};
