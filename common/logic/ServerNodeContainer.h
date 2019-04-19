//------------------------------------------------------------------------
// * @filename: ServerNodeContainer.h
// *
// * @brief: 服务器节点容器
// *
// * @author: XGM
// * @date: 2018/09/27
//------------------------------------------------------------------------
#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <stdint.h>

class ServerNode;
typedef std::shared_ptr<ServerNode> ServerNodePtr;

class ServerNodeContainer
{
public:
	typedef std::vector<ServerNodePtr> ServerNodeVector;
	// <SessionID, ServerNodePtr>
	typedef std::unordered_map<int64_t, ServerNodePtr> SessionNodeMap;
	// <ServerID, ServerNodePtr>
	typedef std::unordered_map<int32_t, ServerNodePtr> IDNodeMap;
	// <GroupID, ServerNodeVector>
	typedef std::unordered_map<int32_t, ServerNodeVector> GroupNodeMap;
public:
	ServerNodeContainer();
	virtual ~ServerNodeContainer();

	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	virtual void Update();

	//------------------------------------------------------------------------
	// 增加服务器
	//------------------------------------------------------------------------
	bool AddServer(const ServerNodePtr& pNode);

	//------------------------------------------------------------------------
	// 删除服务器
	//------------------------------------------------------------------------
	bool DelServer(const ServerNodePtr& pNode);
	bool DelServerByID(int32_t nID);
	bool DelServerBySessionID(int64_t nSessionID);

	//------------------------------------------------------------------------
	// 获得服务器数量
	//------------------------------------------------------------------------
	uint32_t GetServerNum();
	uint32_t GetServerNumInGroup(int32_t nGroup);

	//------------------------------------------------------------------------
	// 获得服务器
	//------------------------------------------------------------------------
	ServerNodePtr GetServerByID(int32_t nID);
	ServerNodePtr GetServerBySessionID(int64_t nSessionID);

	//------------------------------------------------------------------------
	// 通过下标获得服务器
	//------------------------------------------------------------------------
	ServerNodePtr GetServerByIndex(uint32_t nIndex);
	ServerNodePtr GetServerByIndex(int32_t nGroup, uint32_t nIndex);

	//------------------------------------------------------------------------
	// 随机一个服务器
	//------------------------------------------------------------------------
	ServerNodePtr RandomServer();
	ServerNodePtr RandomServerInGroup(int32_t nGroup);

	//------------------------------------------------------------------------
	// 随机一个服务器(通过检测服务)
	//------------------------------------------------------------------------
	ServerNodePtr GetServerByService(uint32_t nStep, const std::string& strService);
	ServerNodePtr GetServerByService(int32_t nGroup, uint32_t nStep, const std::string& strService);
	ServerNodePtr RandomServerByService(const std::string& strService);
	ServerNodePtr RandomServerInGroupByService(int32_t nGroup, const std::string& strService);

	//------------------------------------------------------------------------
	// 分配的下标
	//------------------------------------------------------------------------
	uint32_t& GetCycleIndex() { return m_nCycleIndex; }

private:
	// <SessionID, ServerNodePtr>
	SessionNodeMap m_mapSessionNode;

	// <ServerID, ServerNodePtr>
	IDNodeMap m_mapIDNode;

	// <GroupID, ServerNodePtr>
	GroupNodeMap m_mapGroupNode;

	// 循环分配下标
	uint32_t m_nCycleIndex = 0;
};
