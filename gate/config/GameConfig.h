//------------------------------------------------------------------------
// * @filename: GameConfig.h
// *
// * @brief: 游戏配置
// *
// * @author: XGM
// * @date: 2018/07/07
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"

#include <map>

namespace pbconfig {
	class MsgInfo;
}

class GameConfig : public Singleton<GameConfig>
{
protected:
	friend class Singleton<GameConfig>;
	GameConfig() {}
	virtual ~GameConfig() {}
public:
	//------------------------------------------------------------------------
	// 消息映射表
	//------------------------------------------------------------------------
	void InitMsgTable();
	const pbconfig::MsgInfo* GetMsgInfo(uint32_t nMsgID);
	const pbconfig::MsgInfo* GetMsgInfo(const std::string& strMsgName);

private:
	// <消息ID, 消息名称>
	std::map<uint32_t, std::string> m_mapMsgTable;
};
