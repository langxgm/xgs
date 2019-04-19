//------------------------------------------------------------------------
// * @filename: ServerNode.h
// *
// * @brief: 服务器节点
// *
// * @author: XGM
// * @date: 2018/08/31
//------------------------------------------------------------------------
#pragma once

#include "ServerType.h"

#include <string>
#include <vector>
#include <stdint.h>

class ServerNode
{
public:
	ServerNode() {}
	virtual ~ServerNode() {}

public:
	//------------------------------------------------------------------------
	// 服务器类型
	//------------------------------------------------------------------------
	virtual ServerType GetType() = 0;

	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	virtual void Update() = 0;

	//------------------------------------------------------------------------
	// 认证
	//------------------------------------------------------------------------
	virtual void DoAuth(int64_t nSessionID) = 0;
	virtual void OnAuth() = 0;
	virtual void OnAuthResult() = 0;

	//------------------------------------------------------------------------
	// Ping
	//------------------------------------------------------------------------
	virtual void DoPing() = 0;
	virtual void OnPing() = 0;

	//------------------------------------------------------------------------
	// Pong
	//------------------------------------------------------------------------
	virtual void DoPong() = 0;
	virtual void OnPong() = 0;

public:
	//------------------------------------------------------------------------
	// 服务器ID
	//------------------------------------------------------------------------
	virtual int32_t GetID() const { return 0; }
	virtual void SetID(int32_t nID) {}

	//------------------------------------------------------------------------
	// 服务器组
	//------------------------------------------------------------------------
	virtual int32_t GetGroup() const { return 0; }
	virtual void SetGroup(int32_t nGroup) {}

	//------------------------------------------------------------------------
	// 服务器名字
	//------------------------------------------------------------------------
	virtual const std::string& GetName() const { static std::string s_null;  return s_null; }
	virtual void SetName(const std::string& strName) {}

	//------------------------------------------------------------------------
	// IP
	//------------------------------------------------------------------------
	virtual const std::string& GetIP()const { static std::string s_null; return s_null; }
	virtual void SetIP(const std::string& strIP) {}

	//------------------------------------------------------------------------
	// 连接ID
	//------------------------------------------------------------------------
	virtual int64_t GetSessionID() const { return 0; }
	virtual void SetSessionID(int64_t nSessionID) {}

	//------------------------------------------------------------------------
	// 服务
	//------------------------------------------------------------------------
	virtual bool IsHaveService(const std::string& strService) const { return false; }
	virtual bool CheckServiceCond(const std::string& strService, const std::vector<int64_t>& vecParam) const { return false; }
	virtual bool AddService(const std::string& strService, const std::vector<int64_t>& vecParam) { return false; }
	virtual bool DelService(const std::string& strService) { return false; }
};
