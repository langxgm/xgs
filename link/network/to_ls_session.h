//------------------------------------------------------------------------
// * @filename: to_ls_session.h
// *
// * @brief: 连接登陆服务器
// *
// * @author: XGM
// * @date: 2018/12/27
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xbase/TimeChecker.h"
#include "xshare/net/MessagePacker.h"
#include "xshare/net/ClientWorker.h"

class To_Ls_Session : public ClientWorker, public Singleton<To_Ls_Session>
{
protected:
	friend class Singleton<To_Ls_Session>;
	To_Ls_Session() {}
	virtual ~To_Ls_Session() {}
public:
	//------------------------------------------------------------------------
	// 注册监听的消息
	//------------------------------------------------------------------------
	virtual void RegisterListen() override;

	//------------------------------------------------------------------------
	// 创建消息包装器
	//------------------------------------------------------------------------
	virtual MessagePacker* CreateMessagePacker() override;

	//------------------------------------------------------------------------
	// 每帧执行消息
	//------------------------------------------------------------------------
	virtual void Update() override;

	//------------------------------------------------------------------------
	// 连接成功(异步的)
	//------------------------------------------------------------------------
	virtual void OnConnected(int64_t nSessionID) override;

public:
	void SendPing();
	void HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

private:
	// Ping计时器
	TimeChecker m_PingTimer;
};

#include "link/protos/link2ls_meta.pb.h"

class To_Ls_Meta : public MessageMeta
{
public:
	To_Ls_Meta()
	{
	}

	explicit To_Ls_Meta(int64_t nUserID)
	{
		m_RpcMeta.mutable_req()->set_userid(nUserID);
	}

	virtual uint32_t GetByteSize() const override
	{
		return MessageMeta::GetByteSize() + m_RpcMeta.ByteSizeLong();
	}

public:
	virtual ::google::protobuf::Message* GetUserdata() override { return &m_RpcMeta; }
	virtual int64_t GetGUID() override { return m_RpcMeta.resp().userid(); }
	virtual void SetGUID(int64_t nGUID) override { m_RpcMeta.mutable_req()->set_userid(nGUID); }
public:
	const protos::Link2LSMeta& Request() const { return m_RpcMeta.req(); }
	protos::Link2LSMeta* MutableRequest() { return m_RpcMeta.mutable_req(); }
	const protos::LS2LinkMeta& Response() const { return m_RpcMeta.resp(); }
	protos::LS2LinkMeta* MutableResponse() { return m_RpcMeta.mutable_resp(); }
private:
	protos::Link2LSRpcMeta m_RpcMeta;
};

class To_Ls_MessagePacker : public MessagePacker
{
public:
	virtual MessageMeta* NewMeta() override;
protected:
	virtual void PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta) override;
};
