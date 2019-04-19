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
	class LoginConfig;
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
	// 登陆服务器配置
	//------------------------------------------------------------------------
	bool InitLoginConfig(const std::string& strFileName, Reader& r);
	const pbconfig::LoginConfig* GetLoginConfig() { return m_pLoginConfig; }

private:
	const pbconfig::LoginConfig* m_pLoginConfig = nullptr;
};
