//------------------------------------------------------------------------
// * @filename: t_player_mail.h
// *
// * @brief: 玩家邮件集合定义
// *
// * @author: XGM
// * @date: 2017/09/25
//------------------------------------------------------------------------
#pragma once

//namespace t_player_mail
//{
//	// 集合名称
//	const char t_name[] = "player_mail";
//	// 文档格式
//	const char t_document_format[] = "{ player_guid:int64, mails:[ { uid:int64, type:int32 } ] }";
//	// 字段名称
//	const char f_player_guid[] = "player_guid"; // PlayerGUID
//	const char f_mails[] = "mails"; // 邮件列表
//	namespace mails
//	{
//		const char f_uid[] = "uid"; // 邮件唯一ID
//		const char f_type[] = "type"; // 邮件类型
//	}
//}

// 邮箱(一条记录就是一封邮件)
namespace t_mail_box
{
	// 集合名称
	const char t_name[] = "mail_box";
	// 文档格式
	const char t_document_format[] = "{"
		" player_guid:int64, type:int32, sender_guid:int64, send_time:date"
		", title:string, content:string, isread:int32, isfetch:int32"
		", attachment:[ { type:int32, id:int32, num:int32 } ]"
		" }";
	// 字段名称
	const char f_player_guid[] = "player_guid"; // PlayerGUID
	const char f_type[] = "type"; // 邮件类型(0=系统邮件,1=玩家邮件)
	const char f_sender_guid[] = "sender_guid"; // 发送者PlayerGUID
	const char f_send_time[] = "send_time"; // 邮件发送时间
	const char f_title[] = "title"; // 邮件标题
	const char f_content[] = "content"; // 邮件内容
	const char f_isread[] = "isread"; // 是否已读
	const char f_isfetch[] = "isfetch"; // 是否领取附件
	const char f_attachment[] = "attachment"; // 附件
	namespace attachment
	{
		const char f_type[] = "type"; // 附件类型
		const char f_id[] = "id"; // 附件ID
		const char f_num[] = "num"; // 附件数量
	}

}
