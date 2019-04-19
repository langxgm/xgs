//------------------------------------------------------------------------
// * @filename: MailHandler.h
// *
// * @brief: 邮件逻辑处理
// *
// * @author: XGM
// * @date: 2017/09/29
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"
#include "xshare/net/MessagePacket.h"

class ServerWorker;
namespace protos {
	class MailData;
}

class MailHandler : public LogicHandler, public DBHandler, public Singleton<MailHandler>
{
protected:
	friend class Singleton<MailHandler>;
	MailHandler() {}
	virtual ~MailHandler() {}
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(MailHandler); }

	//------------------------------------------------------------------------
	// 初始化数据库
	//------------------------------------------------------------------------
	virtual void PreInitDatabase() override;

public:
	//------------------------------------------------------------------------
	// 监听Gws的消息
	//------------------------------------------------------------------------
	void ListenFromGws(ServerWorker* pServer);

public:
	//------------------------------------------------------------------------
	// 添加邮件
	//------------------------------------------------------------------------
	static bool AddMail(int64_t nPlayerGUID, int64_t nSenderGUID, int32_t nType, protos::MailData* pMailData);

	//------------------------------------------------------------------------
	// 删除邮件
	//------------------------------------------------------------------------
	static bool DelMail(int64_t nPlayerGUID, const std::string& strOID);

	//------------------------------------------------------------------------
	// 修改邮件标记
	//------------------------------------------------------------------------
	static bool ModifyMailFlag(int64_t nPlayerGUID, const std::string& strOID, bool isRead, bool isFetch);

public:
	void HandleCGMailList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleCGMailDel(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
	void HandleCGMailModifyFlag(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta);
};
