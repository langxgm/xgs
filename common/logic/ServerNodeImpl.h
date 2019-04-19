//------------------------------------------------------------------------
// * @filename: ServerNodeImpl.h
// *
// * @brief: 服务器节点
// *
// * @author: XGM
// * @date: 2018/08/31
//------------------------------------------------------------------------
#pragma once

#include "ServerNode.h"

#include "xbase/TimeChecker.h"

#include <map>

// 服务器信息
struct ServerInfo
{
	int32_t nID = 0; // 服务器ID
	int32_t nGroup = 0; // 服务器分组
	std::string strName; // 服务器名字
	std::string strIP; // 服务器IP
	int32_t nListenClientPort = 0; // 监听客户端端口
	int64_t nSessionID = 0; // 连接ID
};

// 服务信息
struct ServerService
{
	std::string strName; // 服务名称

	// 服务参数
	struct Param
	{
		int64_t nParam1 = 0; // 参数1
		int64_t nParam2 = 0; // 参数2
		int64_t nParam3 = 0; // 参数3
	} aParam;
};
typedef std::map<std::string, ServerService> ServerServiceMap;

// 基础服务器节点
class ServerNode_Base : public ServerNode
{
public:
	ServerNode_Base() {}
	virtual ~ServerNode_Base() {}

public:
	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	virtual void Update() override;

	//------------------------------------------------------------------------
	// 认证
	//------------------------------------------------------------------------
	virtual void DoAuth(int64_t nSessionID) override {}
	virtual void OnAuth() override {}
	virtual void OnAuthResult() override {}

	//------------------------------------------------------------------------
	// Ping
	//------------------------------------------------------------------------
	virtual void DoPing() override {}
	virtual void OnPing() override;

	//------------------------------------------------------------------------
	// Pong
	//------------------------------------------------------------------------
	virtual void DoPong() override {}
	virtual void OnPong() override;

public:
	virtual int32_t GetID() const override { return m_ServerInfo.nID; }
	virtual void SetID(int32_t nID) override { m_ServerInfo.nID = nID; }

	virtual int32_t GetGroup() const override { return m_ServerInfo.nGroup; }
	virtual void SetGroup(int32_t nGroup) override { m_ServerInfo.nGroup = nGroup; }

	virtual const std::string& GetName() const override { return m_ServerInfo.strName; }
	virtual void SetName(const std::string& strName) override { m_ServerInfo.strName = strName; }

	virtual const std::string& GetIP()const override { return m_ServerInfo.strIP; }
	virtual void SetIP(const std::string& strIP) override { m_ServerInfo.strIP = strIP; }

	virtual int64_t GetSessionID() const override { return m_ServerInfo.nSessionID; }
	virtual void SetSessionID(int64_t nSessionID) override { m_ServerInfo.nSessionID = nSessionID; }

	virtual bool IsHaveService(const std::string& strService) const override;
	virtual bool CheckServiceCond(const std::string& strService, const std::vector<int64_t>& vecParam) const override;
	virtual bool AddService(const std::string& strService, const std::vector<int64_t>& vecParam) override;
	virtual bool DelService(const std::string& strService) override;

public:
	TimeChecker& GetPingTimer() { return m_PingTimer; }
protected:
	// 服务器信息
	ServerInfo m_ServerInfo;

	// 服务映射表
	ServerServiceMap m_mapService;

	// Ping计时器
	TimeChecker m_PingTimer;
};
