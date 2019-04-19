//------------------------------------------------------------------------
// * @filename: ServerManager.h
// *
// * @brief: 服务器管理
// *
// * @author: XGM
// * @date: 2018/05/18
//------------------------------------------------------------------------
#pragma once

#include "ServerType.h"

#include "xbase/Singleton.h"

#include <array>
#include <memory>
#include <stdint.h>

class ServerNode;
typedef std::shared_ptr<ServerNode> ServerNodePtr;
class ServerNodeContainer;
typedef std::shared_ptr<ServerNodeContainer> ServerNodeContainerPtr;

class ServerManager : public Singleton<ServerManager>
{
public:
	typedef std::array<ServerNodeContainerPtr, static_cast<int32_t>(ServerType::Max)> NodeTypeList;
protected:
	friend class Singleton<ServerManager>;
	ServerManager();
	virtual ~ServerManager();
public:
	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	virtual void Update(ServerType nType);

	//------------------------------------------------------------------------
	// 增加服务器
	//------------------------------------------------------------------------
	bool AddServer(ServerType nType, const ServerNodePtr& pNode);

	//------------------------------------------------------------------------
	// 删除服务器
	//------------------------------------------------------------------------
	bool DelServerByID(ServerType nType, int32_t nID);
	bool DelServerBySessionID(ServerType nType, int64_t nSessionID);

	//------------------------------------------------------------------------
	// 获得服务器数量
	//------------------------------------------------------------------------
	uint32_t GetServerNum(ServerType nType);
	uint32_t GetServerNumInGroup(ServerType nType, int32_t nGroup);

	//------------------------------------------------------------------------
	// 获得所有服务器数量
	//------------------------------------------------------------------------
	uint32_t GetAllServerNum();

	//------------------------------------------------------------------------
	// 获得服务器表
	//------------------------------------------------------------------------
	ServerNodeContainerPtr GetServers(ServerType nType);
	ServerNodeContainerPtr MutableServers(ServerType nType);

	//------------------------------------------------------------------------
	// 获得服务器
	//------------------------------------------------------------------------
	ServerNodePtr GetServerByID(ServerType nType, int32_t nID);
	ServerNodePtr GetServerBySessionID(ServerType nType, int64_t nSessionID);

	//------------------------------------------------------------------------
	// 通过下标获得服务器
	//------------------------------------------------------------------------
	ServerNodePtr GetServerByIndex(ServerType nType, uint32_t nIndex);

	//------------------------------------------------------------------------
	// 随机一个服务器
	//------------------------------------------------------------------------
	ServerNodePtr RandomServer(ServerType nType);
	ServerNodePtr RandomServerInGroup(ServerType nType, int32_t nGroup);

	//------------------------------------------------------------------------
	// 随机一个服务器(通过检测服务)
	//------------------------------------------------------------------------
	ServerNodePtr RandomServerByService(ServerType nType, const std::string& strService);
	ServerNodePtr RandomServerInGroupByService(ServerType nType, int32_t nGroup, const std::string& strService);

private:
	// 服务器映射
	NodeTypeList m_Servers;
};
