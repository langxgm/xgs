//------------------------------------------------------------------------
// * @filename: GameConfig.h
// *
// * @brief: 游戏配置
// *
// * @author: XGM
// * @date: 2019/01/22
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"

#include "xshare/config/ConfigReader.h"

namespace google {
	namespace protobuf {
		class Message;
	}
}

class GameConfig : public Singleton<GameConfig>
{
public:
	typedef ConfigReader<google::protobuf::Message*, const google::protobuf::Message*> Reader;
protected:
	friend class Singleton<GameConfig>;
	GameConfig() {}
	virtual ~GameConfig();
public:

private:
};
