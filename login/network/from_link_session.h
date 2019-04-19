//------------------------------------------------------------------------
// * @filename: from_client_session.h
// *
// * @brief: 来自客户端的连接
// *
// * @author: XGM
// * @date: 2018/12/27
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xshare/net/MessagePacker.h"
#include "xshare/net/ServerWorker.h"

class From_Link_Session : public ServerWorker, public Singleton<From_Link_Session>
{
protected:
	friend class Singleton<From_Link_Session>;
	From_Link_Session();
	virtual ~From_Link_Session();
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
	// 断开连接
	//------------------------------------------------------------------------
	virtual void OnDisconnect(int64_t nSessionID) override;

public:
	void HandlePing(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};

#include "login/protos/link2ls_meta.pb.h"

class From_Link_Meta : public MessageMeta
{
public:
	From_Link_Meta()
	{
	}

	explicit From_Link_Meta(int64_t nUserID)
	{
		m_RpcMeta.mutable_resp()->set_userid(nUserID);
	}

	virtual uint32_t GetByteSize() const override
	{
		return MessageMeta::GetByteSize() + m_RpcMeta.ByteSizeLong();
	}

public:
	virtual ::google::protobuf::Message* GetUserdata() override { return &m_RpcMeta; }
	virtual int64_t GetGUID() override { return m_RpcMeta.req().userid(); }
	virtual void SetGUID(int64_t nGUID) override { m_RpcMeta.mutable_resp()->set_userid(nGUID); }
public:
	const protos::Link2LSMeta& Request() const { return m_RpcMeta.req(); }
	protos::Link2LSMeta* MutableRequest() { return m_RpcMeta.mutable_req(); }
	const protos::LS2LinkMeta& Response() const { return m_RpcMeta.resp(); }
	protos::LS2LinkMeta* MutableResponse() { return m_RpcMeta.mutable_resp(); }
private:
	protos::Link2LSRpcMeta m_RpcMeta;
};

class From_Link_MessagePacker : public MessagePacker
{
public:
	virtual MessageMeta* NewMeta() override;
protected:
	virtual void PackBefore(uint32_t nMsgID, const ::google::protobuf::Message* pMsg, const MessageMeta* pMeta) override;
};
