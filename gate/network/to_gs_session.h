//------------------------------------------------------------------------
// * @filename: to_gs_session.h
// *
// * @brief: 连接GS服务器
// *
// * @author: XGM
// * @date: 2018/05/10
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xbase/TimeChecker.h"
#include "xshare/net/MessagePacker.h"
#include "xshare/net/ClientWorker.h"

class To_Gs_Session : public ClientWorker, public Singleton<To_Gs_Session>
{
protected:
	friend class Singleton<To_Gs_Session>;
	To_Gs_Session();
	virtual ~To_Gs_Session();
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
	// 处理未注册的消息(异步的)
	//------------------------------------------------------------------------
	virtual void OnMissMessage(uint32_t nMsgID, const void* pMsg, size_t nLen, int64_t nSessionID, const MessageMetaPtr& pMeta) override;

	//------------------------------------------------------------------------
	// 连接成功(异步的)
	//------------------------------------------------------------------------
	virtual void OnConnected(int64_t nSessionID) override;

public:
	void SendPing();
	void SendAuth();
public:
	void HandlePong(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLoginAuthResult(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);

private:
	// Ping计时器
	TimeChecker m_PingTimer;
};

#include "gate/protos/meta.pb.h"

class To_Gs_Meta : public MessageMeta
{
public:
	To_Gs_Meta()
	{
	}

	explicit To_Gs_Meta(int64_t nPlayerGUID)
	{
		m_RpcMeta.mutable_req()->set_guid(nPlayerGUID);
	}

	virtual uint32_t GetByteSize() const override
	{
		return MessageMeta::GetByteSize() + m_RpcMeta.ByteSizeLong();
	}

public:
	virtual ::google::protobuf::Message* GetUserdata() override { return &m_RpcMeta; }
	virtual int64_t GetGUID() override { return m_RpcMeta.resp().guid(); }
	virtual void SetGUID(int64_t nGUID) override { m_RpcMeta.mutable_req()->set_guid(nGUID); }
public:
	const protos::GWS2GMeta& Request() const { return m_RpcMeta.req(); }
	protos::GWS2GMeta* MutableRequest() { return m_RpcMeta.mutable_req(); }
	const protos::G2GWSMeta& Response() const { return m_RpcMeta.resp(); }
	protos::G2GWSMeta* MutableResponse() { return m_RpcMeta.mutable_resp(); }
private:
	protos::GWS2GRpcMeta m_RpcMeta;
};

class To_Gs_MessagePacker : public MessagePacker
{
public:
	virtual MessageMeta* NewMeta() override;
protected:
	virtual void PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta) override;
};
