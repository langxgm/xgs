//------------------------------------------------------------------------
// * @filename: ServerNodeMacros.h
// *
// * @brief: 服务器节点宏定义
// *
// * @author: XGM
// * @date: 2018/08/31
//------------------------------------------------------------------------
#pragma once

// OnDisconnect
#define ServerNode_OnDisconnect(server_type) \
	auto pServer = ServerManager::Me()->GetServerBySessionID(server_type, nSessionID);\
	if (!pServer)\
	{\
		LOG(WARNING) << "network: disconnect sessionid=" << nSessionID\
			<< " not found";\
		return;\
	}\
	\
	bool bRet = ServerManager::Me()->DelServerBySessionID(server_type, nSessionID);\
	\
	LOG(WARNING) << "network: disconnect name=" << pServer->GetName()\
		<< " id=" << pServer->GetID()\
		<< " group=" << pServer->GetGroup()\
		<< " sessionid=" << pServer->GetSessionID()\
		<< " result=" << bRet;

// HandleLoginAuth
#define ServerNode_HandleLoginAuth(server_type, servernode_type) \
	auto pConn = GetConnPtr(nSessionID);\
	if (!pConn)\
	{\
		return;\
	}\
	\
	decltype(pHandleMsg->code()) authcode = 0xA4A8DDA9;\
	if (pHandleMsg->code() != authcode)\
	{\
		protos::LoginAuthResult send;\
		send.set_error(1);\
		send.set_errmsg("auth faild code value invalid"); /*认证失败*/\
		Send(nSessionID, &send, *pMeta);\
		return;\
	}\
	\
	auto pNode = ServerManager::Me()->GetServerByID(server_type, pHandleMsg->id());\
	if (pNode)\
	{\
		ServerManager::Me()->DelServerByID(server_type, pHandleMsg->id());\
	}\
	else\
	{\
		pNode = std::make_shared<servernode_type>();\
	}\
	\
	pNode->SetSessionID(nSessionID);\
	pNode->SetID(pHandleMsg->id());\
	pNode->SetGroup(pHandleMsg->group());\
	pNode->SetName(pHandleMsg->name());\
	ServerManager::Me()->AddServer(server_type, pNode);\
	\
	std::vector<std::string> vecString;\
	evpp::StringSplit(pConn->remote_addr(), ":", 0, vecString);\
	if (vecString.size() > 0)\
	{\
		pNode->SetIP(vecString[0]);\
	}\
	\
	protos::LoginAuthResult send;\
	Send(nSessionID, &send, *pMeta);\
	\
	LOG(WARNING) << "network: from server auth msg: " << pHandleMsg->ShortDebugString()\
		<< " , ip=" << pNode->GetIP()\
		<< " sessionid=" << nSessionID;

// HandlePing
#define ServerNode_HandlePing(server_type) \
	LOG_FIRST_N(WARNING, 10) << "network: recv ping " << pHandleMsg->message();\
	auto pNode = ServerManager::Me()->GetServerBySessionID(server_type, nSessionID);\
	if (pNode)\
	{\
		pNode->OnPing();\
	}

// HandlePong
#define ServerNode_HandlePong(server_type) \
	LOG_FIRST_N(WARNING, 10) << "network: recv pong " << pHandleMsg->message();\
	auto pNode = ServerManager::Me()->GetServerBySessionID(server_type, nSessionID);\
	if (pNode)\
	{\
		pNode->OnPong();\
	}

// HandleLoginAuthResult
#define ServerNode_HandleLoginAuthResult(server_type, servernode_type) \
	if (pHandleMsg->error() != 0)\
	{\
		LOG(ERROR) << "network: to server auth result error=" << pHandleMsg->error()\
			<< " errmsg=" << pHandleMsg->errmsg();\
	}\
	else\
	{\
		LOG(WARNING) << "network: to server auth result is successful";\
		\
		ServerNodePtr pNode = std::make_shared<servernode_type>();\
		pNode->SetSessionID(nSessionID);\
		ServerManager::Me()->AddServer(server_type, pNode);\
		\
		pNode->OnAuthResult();\
	}
