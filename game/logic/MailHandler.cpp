
#include "MailHandler.h"
#include "game/network/from_gws_session.h"
#include "game/logic/HandleMsgMacros.h"
#include "game/logic/t_player_mail.h"

#include "game/protos/mail.pb.h"

#include "xbase/TimeUtil.h"
#include "xdb/mongo/MongoExecutor.h"
#include "xdb/mongo/MongoElement.h"
#include "xshare/work/WorkDispatcher.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <libgo/coroutine.h>

void MailHandler::PreInitDatabase()
{
	MongoExecutor dbExecutor;
	auto coll = dbExecutor.GetCollection(t_mail_box::t_name);

	bsoncxx::builder::stream::document indexBuilder;
	indexBuilder << t_mail_box::f_player_guid << 1;
	auto result = coll.create_index(indexBuilder.view());
	LOG(WARNING) << "create_index " << t_mail_box::t_name << " " << bsoncxx::to_json(result);
}

void MailHandler::ListenFromGws(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &MailHandler::HandleCGMailList, this, protos::CGMailList, "邮件列表请求");
	REG_MSG_TO(pServer, &MailHandler::HandleCGMailDel, this, protos::CGMailDel, "删除邮件请求");
	REG_MSG_TO(pServer, &MailHandler::HandleCGMailModifyFlag, this, protos::CGMailModifyFlag, "修改邮件标记请求");

	//// 测试邮件数据
	//int64_t nPlayerGUID = 16777219;
	//// 1: 添加体力邮件
	//{
	//	protos::MailData data;
	//	data.set_title("test tilte");
	//	data.set_content("test content");
	//	auto pAttachment = data.add_attachments();
	//	pAttachment->set_type(protos::MailAttachment_Type_TYPE_POWER);
	//	pAttachment->set_id(0);
	//	pAttachment->set_num(10);
	//	AddMail(nPlayerGUID, -1, 0, &data);
	//}
	//// 2: 添加道具邮件
	//{
	//	protos::MailData data;
	//	data.set_title("test tilte2");
	//	data.set_content("test content2");
	//	auto pAttachment = data.add_attachments();
	//	pAttachment->set_type(protos::MailAttachment_Type_TYPE_ITEM);
	//	pAttachment->set_id(1);
	//	pAttachment->set_num(1);
	//	auto pAttachment2 = data.add_attachments();
	//	pAttachment2->set_type(protos::MailAttachment_Type_TYPE_ITEM);
	//	pAttachment2->set_id(2);
	//	pAttachment2->set_num(2);
	//	auto pAttachment3 = data.add_attachments();
	//	pAttachment3->set_type(protos::MailAttachment_Type_TYPE_ITEM);
	//	pAttachment3->set_id(3);
	//	pAttachment3->set_num(3);
	//	auto pAttachment4 = data.add_attachments();
	//	pAttachment4->set_type(protos::MailAttachment_Type_TYPE_ITEM);
	//	pAttachment4->set_id(4);
	//	pAttachment4->set_num(4);
	//	auto pAttachment5 = data.add_attachments();
	//	pAttachment5->set_type(protos::MailAttachment_Type_TYPE_ITEM);
	//	pAttachment5->set_id(5);
	//	pAttachment5->set_num(5);
	//	AddMail(nPlayerGUID, -1, 0, &data);
	//}
}

bool MailHandler::AddMail(int64_t nPlayerGUID, int64_t nSenderGUID, int32_t nType, protos::MailData* pMailData)
{
	bsoncxx::types::b_date date{ std::chrono::milliseconds(TimeUtil::GetCurrentTimeMillis()) };

	// 创建
	bsoncxx::builder::stream::document createBuilder;
	createBuilder << t_mail_box::f_player_guid << nPlayerGUID
		<< t_mail_box::f_type << nType
		<< t_mail_box::f_sender_guid << nSenderGUID
		<< t_mail_box::f_send_time << date
		<< t_mail_box::f_title << pMailData->title()
		<< t_mail_box::f_content << pMailData->content()
		<< t_mail_box::f_isread << (int32_t)0
		<< t_mail_box::f_isfetch << (int32_t)0;

	auto arrayBuilder = createBuilder << t_mail_box::f_attachment << open_array;
	for (int i = 0; i < pMailData->attachments_size(); ++i)
	{
		auto& attachment = pMailData->attachments(i);
		arrayBuilder << open_document
			<< t_mail_box::attachment::f_type << (int32_t)attachment.type()
			<< t_mail_box::attachment::f_id << attachment.id()
			<< t_mail_box::attachment::f_num << attachment.num()
			<< close_document;
	}
	arrayBuilder << close_array;

	try
	{
		MongoExecutor dbExecutor;
		auto result = dbExecutor.InsertOne(t_mail_box::t_name, createBuilder.view());
		LOG_IF(INFO, result) << "create " << t_mail_box::t_name << " guid=" << nPlayerGUID
			<< ",result inserted_count=" << result->result().inserted_count();
	}
	catch (const mongocxx::bulk_write_exception& e)
	{
		LOG(ERROR) << "create " << t_mail_box::t_name << " guid=" << nPlayerGUID
			<< ",mongocxx::bulk_write_exception e=" << e.what();
		return false;
	}

	return true;
}

bool MailHandler::DelMail(int64_t nPlayerGUID, const std::string& strOID)
{
	auto oid = bsoncxx::types::b_oid{ bsoncxx::oid(strOID) };

	// 删除
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << "_id" << oid
		<< t_mail_box::f_player_guid << nPlayerGUID;

	MongoExecutor dbExecutor;
	auto result = dbExecutor.DeleteOne(t_mail_box::t_name, filterBuilder.view());
	LOG_IF(INFO, result) << "delete " << t_mail_box::t_name << " guid=" << nPlayerGUID
		<< ",result deleted_count=" << result->result().deleted_count();

	return true;
}

bool MailHandler::ModifyMailFlag(int64_t nPlayerGUID, const std::string& strOID, bool isRead, bool isFetch)
{
	auto oid = bsoncxx::types::b_oid{ bsoncxx::oid(strOID) };

	// 修改
	bsoncxx::builder::stream::document filterBuilder;
	filterBuilder << "_id" << oid
		<< t_mail_box::f_player_guid << nPlayerGUID;

	bsoncxx::builder::stream::document updateBuilder;
	updateBuilder << "$set"
		<< open_document
		<< t_mail_box::f_isread << (int32_t)isRead
		<< t_mail_box::f_isfetch << (int32_t)isFetch
		<< close_document;

	MongoExecutor dbExecutor;
	auto result = dbExecutor.UpdateOne(t_mail_box::t_name, filterBuilder.view(), updateBuilder.view());
	LOG_IF(INFO, result) << "update " << t_mail_box::t_name << " guid=" << nPlayerGUID
		<< ",result matched_count=" << result->matched_count()
		<< " modified_count=" << result->modified_count();

	return true;
}

void MailHandler::HandleCGMailList(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGMailList);

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			bsoncxx::builder::stream::document filterBuilder;
			filterBuilder << t_mail_box::f_player_guid << pHandleMsg->guid();

			// 发送邮件列表
			protos::GCMailList send;
			send.set_error(0);
			send.set_guid(pHandleMsg->guid());

			MongoExecutor dbExecutor;
			auto cursor = dbExecutor.Find(t_mail_box::t_name, filterBuilder.view());
			for (auto&& doc : cursor)
			{
				protos::MailData* pNew = send.add_maildatas();
				pNew->set_oid(MongoElement<b_oid>::GetValue(doc["_id"]).to_string());
				pNew->set_title(MongoElement<b_utf8>::GetValue(doc[t_mail_box::f_title]).to_string());
				pNew->set_content(MongoElement<b_utf8>::GetValue(doc[t_mail_box::f_content]).to_string());
				pNew->set_isread(MongoElement<b_int32>::GetValue(doc[t_mail_box::f_isread]));
				pNew->set_isfetch(MongoElement<b_int32>::GetValue(doc[t_mail_box::f_isfetch]));

				bsoncxx::array::view attachmentArrView = MongoElement<b_array>::GetValue(doc[t_mail_box::f_attachment]);
				for (const bsoncxx::array::element& e : attachmentArrView)
				{
					bsoncxx::document::view doc = MongoElement<b_document>::GetValue(e);

					protos::MailAttachment* pNewAttachment = pNew->add_attachments();
					pNewAttachment->set_type((protos::MailAttachment_Type)MongoElement<b_int32>::GetValue(doc[t_mail_box::attachment::f_type]));
					pNewAttachment->set_id(MongoElement<b_int32>::GetValue(doc[t_mail_box::attachment::f_id]));
					pNewAttachment->set_num(MongoElement<b_int32>::GetValue(doc[t_mail_box::attachment::f_num]));
				}
			}

			From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);
		};
	});
}

void MailHandler::HandleCGMailDel(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGMailDel);

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			protos::GCMailDel send;
			send.set_error(0);

			// 删除邮件
			if (DelMail(pHandleMsg->guid(), pHandleMsg->oid()) == false)
			{
				send.set_error(1);
			}

			send.set_guid(pHandleMsg->guid());
			send.set_allocated_oid(pHandleMsg->release_oid());
			From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);
		};
	});
}

void MailHandler::HandleCGMailModifyFlag(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::CGMailModifyFlag);

	WorkDispatcherAsync(pHandleMsg->guid()).RunInLoop([=]() {
		go[=]() {

			protos::GCMailModifyFlag send;
			send.set_error(0);

			// 修改邮件标记
			if (ModifyMailFlag(pHandleMsg->guid(), pHandleMsg->oid(),
				pHandleMsg->isread(), pHandleMsg->isfetch()) == false)
			{
				send.set_error(1);
			}

			send.set_guid(pHandleMsg->guid());
			send.set_allocated_oid(pHandleMsg->release_oid());
			send.set_isread(pHandleMsg->isread());
			send.set_isfetch(pHandleMsg->isfetch());
			From_Gws_Session::Me()->Send(nSessionID, &send, *pMeta);
		};
	});
}
