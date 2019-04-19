//------------------------------------------------------------------------
// * @filename: t_server.h
// *
// * @brief: 服务器集合定义
// *
// * @author: XGM
// * @date: 2018/07/14
//------------------------------------------------------------------------
#pragma once

namespace t_server
{
	// 集合名称
	const char t_name[] = "server";
	// 文档格式
	const char t_document_format[] = "{ name:string, id:int32, version:string, addr:string, url:string, client_res_url:string }";
	// 字段名称
	const char f_name[] = "name"; // 服务器名称
	const char f_id[] = "id"; // 服务器ID
	const char f_version[] = "version"; // 服务器版本号
	const char f_addr[] = "addr"; // 服务器地址IP:PORT
	const char f_url[] = "url"; // 服务器链接地址
	const char f_client_res_url[] = "client_res_url"; // 客户端资源链接地址
}
