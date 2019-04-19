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
	class GameConfig;
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
	// 游戏服务器配置
	//------------------------------------------------------------------------
	bool InitGameConfig(const std::string& strFileName, Reader& r);
	const pbconfig::GameConfig* GetGameConfig() { return m_pGameConfig; }

private:
	const pbconfig::GameConfig* m_pGameConfig = nullptr;
};
