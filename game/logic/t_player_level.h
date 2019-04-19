//------------------------------------------------------------------------
// * @filename: t_player_level.h
// *
// * @brief: 玩家关卡集合定义
// *
// * @author: XGM
// * @date: 2017/09/13
//------------------------------------------------------------------------
#pragma once

namespace t_player_level
{
	// 集合名称
	const char t_name[] = "player_level";
	// 文档格式
	const char t_document_format[] = "{ player_guid:int64, finished_num:int32, finished_levels:string, update_time:date, update_count:int32 }";
	// 字段名称
	const char f_player_guid[] = "player_guid"; // PlayerGUID
	const char f_finished_num[] = "finished_num"; // 完成关卡数
	const char f_finished_levels[] = "finished_levels"; // 完成关卡信息
	const char f_update_time[] = "update_time"; // 更新时间
	const char f_update_count[] = "update_count"; // 更新次数
}
