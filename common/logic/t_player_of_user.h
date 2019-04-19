//------------------------------------------------------------------------
// * @filename: t_player_of_user.h
// *
// * @brief: 用户的角色集合定义
// *
// * @author: XGM
// * @date: 2017/09/11
//------------------------------------------------------------------------
#pragma once

namespace t_player_of_user
{
	// 集合名称
	const char t_name[] = "player_of_user";
	// 文档格式
	const char t_document_format[] = "{ userid:int64, server_id:int32"
		", guid:int64, name:string, portrait:int32, level:int32"
		", register_time:date, login_time:date, login_count:int32, deviceid:string, ip:string }";

	// 字段名称
	const char f_userid[] = "userid"; // userid
	const char f_server_id[] = "server_id"; // 游戏服务器ID
	const char f_guid[] = "guid"; // PlayerGUID
	const char f_name[] = "name"; // 角色名称
	const char f_portrait[] = "portrait"; // 头像ID
	const char f_level[] = "level"; // 角色等级
	const char f_register_time[] = "register_time"; // 注册时间
	const char f_login_time[] = "login_time"; // 最后登录时间
	const char f_login_count[] = "login_count"; // 登陆次数
	const char f_deviceid[] = "deviceid"; // 登陆的设备id
	const char f_ip[] = "ip"; // 最后登陆ip
}
