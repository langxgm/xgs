//------------------------------------------------------------------------
// * @filename: t_server_cache.h
// *
// * @brief: 服务器DB缓存
// *
// * @author: XGM
// * @date: 2018/07/14
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"

#include <map>
#include <vector>
#include <array>

namespace dbcache
{
	// 服务器类型
	enum class ServerType : int32_t {
		Gateway, // 网关服务器
		Game, // 游戏服务器
		World, // 世界服务器
		Sdk, // SDK服务器
		ClientRes, // 客户端资源服务器
	};

	// 服务器名称
	namespace server_name
	{
		// 网关服务器
		static const std::string s_gateway = "gws";
		// 游戏服务器
		static const std::string s_game = "gs";
		// 世界服务器
		static const std::string s_world = "ws";
		// SDK服务器
		static const std::string s_sdk = "sdk";
		// 客户端资源服务器
		static const std::string s_client_res = "client_res";
	}

	// 版本号位数
	static const int32_t s_version_size = 3;

	class t_server_cache : public Singleton<t_server_cache>
	{
	public:
		struct ServerInfo
		{
			std::string strName; // 服务器名称
			int32_t nID = 0; // 服务器ID
			std::string strVersion; // 服务器版本号
			std::string strAddr; // 服务器地址IP:PORT
			std::string strUrl; // 服务器链接地址

			// 解析过后的版本号数组
			std::array<int32_t, s_version_size> arrVersion = {}; // 服务器版本号
		};

		typedef std::vector<ServerInfo> ServerList;
		typedef std::map<ServerType, ServerList> ServerMap;

	protected:
		friend class Singleton<t_server_cache>;
		t_server_cache();
		virtual ~t_server_cache();
	public:
		//------------------------------------------------------------------------
		// 初始化数据库
		//------------------------------------------------------------------------
		static void InitDatabase(const std::string& strDBName);

		//------------------------------------------------------------------------
		// 从数据库初始化数据
		//------------------------------------------------------------------------
		static void InitDataFromDatabase(const std::string& strDBName);

		//------------------------------------------------------------------------
		// 增加服务器
		//------------------------------------------------------------------------
		bool AddServer(ServerType nType, const ServerInfo& info);

		//------------------------------------------------------------------------
		// 删除所有服务器
		//------------------------------------------------------------------------
		void RemoveAll();

		//------------------------------------------------------------------------
		// 获得服务器数量
		//------------------------------------------------------------------------
		int32_t GetServerNum(ServerType nType);

		//------------------------------------------------------------------------
		// 获得服务器列表
		//------------------------------------------------------------------------
		const ServerList& GetServerList(ServerType nType);

		//------------------------------------------------------------------------
		// 获得服务器信息
		//------------------------------------------------------------------------
		ServerInfo* GetServer(ServerType nType, int32_t nID);

		//------------------------------------------------------------------------
		// 通过下标获得服务器信息
		//------------------------------------------------------------------------
		ServerInfo* GetServerByIndex(ServerType nType, int32_t nIndex);

	private:
		// 服务器映射
		ServerMap m_mapServer;
	};
}
