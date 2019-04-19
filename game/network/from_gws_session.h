//------------------------------------------------------------------------
// * @filename: from_gws_session.h
// *
// * @brief: 来自GWS的连接
// *
// * @author: XGM
// * @date: 2017/08/07
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/MessagePacker.h"
#include "xshare/net/ServerWorker.h"

class From_Gws_Session : public ServerWorker, public Singleton<From_Gws_Session>
{
protected:
	friend class Singleton<From_Gws_Session>;
	From_Gws_Session();
	virtual ~From_Gws_Session();
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
	// 断开连接
	//------------------------------------------------------------------------
	virtual void OnDisconnect(int64_t nSessionID) override;

	//------------------------------------------------------------------------
	// 处理未注册的消息(异步的)
	//------------------------------------------------------------------------
	virtual void OnMissMessage(uint32_t nMsgID, const void* pMsg, size_t nLen, int64_t nSessionID, const MessageMetaPtr& pMeta) override;

public:
	void HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLoginAuth(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};

#include "game/protos/meta.pb.h"

class From_Gws_Meta : public MessageMeta
{
public:
	From_Gws_Meta()
	{
	}

	explicit From_Gws_Meta(int64_t nPlayerGUID)
	{
		m_RpcMeta.mutable_resp()->set_guid(nPlayerGUID);
	}

	explicit From_Gws_Meta(const std::vector<int64_t>& vecPlayerGUID)
	{
		m_RpcMeta.mutable_resp()->mutable_guids()->Reserve(vecPlayerGUID.size());
		for (auto& nGUID : vecPlayerGUID)
		{
			m_RpcMeta.mutable_resp()->add_guids(nGUID);
		}
	}

	virtual uint32_t GetByteSize() const override
	{
		return MessageMeta::GetByteSize() + m_RpcMeta.ByteSizeLong();
	}

public:
	virtual ::google::protobuf::Message* GetUserdata() override { return &m_RpcMeta; }
	virtual int64_t GetGUID() override { return m_RpcMeta.req().guid(); }
	virtual void SetGUID(int64_t nGUID) override { m_RpcMeta.mutable_resp()->set_guid(nGUID); }
public:
	const protos::GWS2GMeta& Request() const { return m_RpcMeta.req(); }
	protos::GWS2GMeta* MutableRequest() { return m_RpcMeta.mutable_req(); }
	const protos::G2GWSMeta& Response() const { return m_RpcMeta.resp(); }
	protos::G2GWSMeta* MutableResponse() { return m_RpcMeta.mutable_resp(); }
private:
	protos::GWS2GRpcMeta m_RpcMeta;
};

class From_Gws_MessagePacker : public MessagePacker
{
public:
	virtual MessageMeta* NewMeta() override;
protected:
	virtual void PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta) override;
};
