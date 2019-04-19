//------------------------------------------------------------------------
// * @filename: t_user.h
// *
// * @brief: user集合定义
// *
// * @author: XGM
// * @date: 2017/09/11
//------------------------------------------------------------------------
#pragma once

namespace t_user
{
	// 集合名称
	const char t_name[] = "user";
	// 文档格式
	const char t_document_format[] = "{ userid:int64, account:string, register_time:date, login_time:date, login_count:int32, deviceid:string, ip:string }";
	// 字段名称
	const char f_userid[] = "userid"; // userid
	const char f_account[] = "account"; // 账号
	const char f_register_time[] = "register_time"; // 注册时间
	const char f_login_time[] = "login_time"; // 最后登录时间
	const char f_login_count[] = "login_count"; // 登陆次数
	const char f_deviceid[] = "deviceid"; // 登陆的设备id
	const char f_ip[] = "ip"; // 最后登陆ip
	const char f_openid[] = "openid"; // openid
}
