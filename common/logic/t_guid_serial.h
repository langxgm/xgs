//------------------------------------------------------------------------
// * @filename: t_guid_serial.h
// *
// * @brief: guid_serial集合定义
// *
// * @author: XGM
// * @date: 2017/09/11
//------------------------------------------------------------------------
#pragma once

#include <stdint.h>

namespace t_guid_serial
{
	// 集合名称
	const char t_name[] = "guid_serial";
	// 文档格式
	const char t_document_format[] = "{ guid_type:string, guid_value:int64 }";
	// 字段名称
	const char f_guid_type[] = "guid_type"; // GUID类型
	const char f_guid_value[] = "guid_value"; // GUID值

	// 默认值
	// userid
	const char    v_user_id_name[] = "user_id";
	const int64_t v_user_id_init = 1073741824;
	// player_guid
	const char    v_player_guid_name[] = "player_guid";
	const int64_t v_player_guid_init = 16777216;
	// party_guid
	const char    v_party_guid_name[] = "party_guid";
	const int64_t v_party_guid_init = 100000000;
	// share_guid
	const char    v_share_guid_name[] = "share_guid";
	const int64_t v_share_guid_init = 100000000;
	// game_room_guid
	const char    v_game_room_guid_name[] = "game_room_guid";
	const int64_t v_game_room_guid_init = 100000000;
}
