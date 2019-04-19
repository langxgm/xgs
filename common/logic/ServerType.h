//------------------------------------------------------------------------
// * @filename: ServerType.h
// *
// * @brief: 服务器类型
// *
// * @author: XGM
// * @date: 2018/05/18
//------------------------------------------------------------------------
#pragma once

#include <stdint.h>

enum class ServerType : int32_t {
	Gateway, // 网关服务器
	Game, // 游戏服务器
	World, // 世界服务器
	Sdk, // SDK服务器
	Max,
};
