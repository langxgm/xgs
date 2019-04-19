//------------------------------------------------------------------------
// * @filename: t_player.h
// *
// * @brief: player集合定义
// *
// * @author: XGM
// * @date: 2017/08/24
//------------------------------------------------------------------------
#pragma once

namespace t_player
{
	// 集合名称
	const char t_name[] = "player";
	// 文档格式
	const char t_document_format[] = "{ guid:int64, name:string, portrait:int32, level:int32"
		", register_time:date, login_time:date, login_count:int32, deviceid:string, ip:string }";

	// 字段名称
	const char f_guid[] = "guid"; // PlayerGUID
	const char f_name[] = "name"; // 角色名称
	const char f_portrait[] = "portrait"; // 头像ID
	const char f_level[] = "level"; // 角色等级
	const char f_register_time[] = "register_time"; // 注册时间
	const char f_login_time[] = "login_time"; // 最后登录时间
	const char f_login_count[] = "login_count"; // 登陆次数
	const char f_deviceid[] = "deviceid"; // 登陆的设备id
	const char f_ip[] = "ip"; // 最后登陆ip

	// 常量定义
	const int MaxNameLen = 64; // 名称长度上限
}
