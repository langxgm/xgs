//------------------------------------------------------------------------
// * @filename: from_gs_session.h
// *
// * @brief: 来自GS的连接
// *
// * @author: XGM
// * @date: 2017/09/08
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/MessagePacker.h"
#include "xshare/net/ServerWorker.h"

class From_Gs_Session : public ServerWorker, public Singleton<From_Gs_Session>
{
protected:
	friend class Singleton<From_Gs_Session>;
	From_Gs_Session() {}
	virtual ~From_Gs_Session() {}
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

public:
	void HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleLoginAuth(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleRegisterService(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleUnregisterService(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};

#include "world/protos/meta.pb.h"

class From_Gs_Meta : public MessageMeta
{
public:
	From_Gs_Meta()
	{
	}

	explicit From_Gs_Meta(int64_t nPlayerGUID)
	{
		m_RpcMeta.mutable_resp()->set_guid(nPlayerGUID);
	}

	explicit From_Gs_Meta(const std::vector<int64_t>& vecPlayerGUID)
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
	const protos::GWMeta& Request() const { return m_RpcMeta.req(); }
	protos::GWMeta* MutableRequest() { return m_RpcMeta.mutable_req(); }
	const protos::WGMeta& Response() const { return m_RpcMeta.resp(); }
	protos::WGMeta* MutableResponse() { return m_RpcMeta.mutable_resp(); }
private:
	protos::GWRpcMeta m_RpcMeta;
};

class From_Gs_MessagePacker : public MessagePacker
{
public:
	virtual MessageMeta* NewMeta() override;
protected:
	virtual void PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta) override;
};
