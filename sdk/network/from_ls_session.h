//------------------------------------------------------------------------
// * @filename: from_ls_session.h
// *
// * @brief: 来自LS的连接
// *
// * @author: XGM
// * @date: 2018/11/07
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/ServerWorker.h"

class From_Ls_Session : public ServerWorker, public Singleton<From_Ls_Session>
{
protected:
	friend class Singleton<From_Ls_Session>;
	From_Ls_Session();
	virtual ~From_Ls_Session();
public:
	//------------------------------------------------------------------------
	// 注册监听的消息
	//------------------------------------------------------------------------
	virtual void RegisterListen() override;

	//------------------------------------------------------------------------
	// 断开连接
	//------------------------------------------------------------------------
	virtual void OnDisconnect(int64_t nSessionID) override;

public:
	void HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
