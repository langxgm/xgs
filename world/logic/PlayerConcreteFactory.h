//------------------------------------------------------------------------
// * @filename: PlayerConcreteFactory.h
// *
// * @brief: 创建玩家的具体工厂
// *
// * @author: XGM
// * @date: 2018/05/11
//------------------------------------------------------------------------
#pragma once

#include "PlayerFactory.h"

class PlayerDefaultFactory : public PlayerFactory
{
public:
	//------------------------------------------------------------------------
	// 创建玩家
	//------------------------------------------------------------------------
	virtual Player* NewPlayer() override;
};
