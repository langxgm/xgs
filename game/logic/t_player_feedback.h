//------------------------------------------------------------------------
// * @filename: t_player_feedback.h
// *
// * @brief: 玩家意见反馈(集合定义)
// *
// * @author: XGM
// * @date: 2018/02/23
//------------------------------------------------------------------------
#pragma once

namespace t_player_feedback
{
	// 集合名称
	const char t_name[] = "player_feedback";
	// 文档格式
	const char t_document_format[] = "{ player_guid:int64, send_time:date, content:string }";
	// 字段名称
	const char f_player_guid[] = "player_guid"; // PlayerGUID
	const char f_send_time[] = "send_time"; // 反馈发送时间
	const char f_content[] = "content"; // 反馈内容

	// 常量定义
	const int MaxContentLen = 256; // 反馈内容长度上限
}
