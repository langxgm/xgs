//------------------------------------------------------------------------
// * @filename: HandleMsgMacros.h
// *
// * @brief: 处理消息的宏定义
// *
// * @author: XGM
// * @date: 2018/08/15
//------------------------------------------------------------------------
#pragma once

#include "xbase/TimeUtil.h"

#include <evpp/logging.h>

#include <assert.h>

// 消息转换
#ifdef _DEBUG
#define MSG_OF(fullname) auto pHandleMsg = std::dynamic_pointer_cast<fullname>(pMsg)
#else
#define MSG_OF(fullname) auto pHandleMsg = std::static_pointer_cast<fullname>(pMsg)
#endif

// 消息转换并检测
#define MSG_CHECK_OF(fullname) MSG_OF(fullname); \
	assert(pHandleMsg && #fullname);\
	if (!pHandleMsg)\
		return

// 消息转换并检测Player
#define MSG_PLAYER_CHECK_OF(ns, name) MSG_CHECK_OF(ns::name); \
	auto __t0__ = TimeUtil::GetCurrentTimeMicros();\
	auto pPlayer = PlayerManager::Me()->GetPlayer(pMeta->GetGUID());\
	if (!pPlayer)\
	{\
		VLOG(0) << __func__ << "/fail guid=" << pMeta->GetGUID() << " not logged in";\
		return; /*没登录*/\
	}

// 消息执行成功
#define MSG_EXEC_OK(log_args) \
	{\
		auto __t1__ = TimeUtil::GetCurrentTimeMicros();\
		auto __show_t__ = (__t1__ - __t0__) / 100; (__t1__ - __show_t__ * 100) ? __show_t__ += 1 : 0;\
		VLOG(0) << __func__ << "/ok guid=" << pPlayer->GetPlayerGUID() << log_args << " T=" << __show_t__ / 10000.0; \
	}

// 消息执行失败
#define MSG_EXEC_FAIL(errmsg, log_args) \
	{\
		auto __t1__ = TimeUtil::GetCurrentTimeMicros();\
		auto __show_t__ = (__t1__ - __t0__) / 100; (__t1__ - __show_t__ * 100) ? __show_t__ += 1 : 0;\
		VLOG(0) << __func__ << "/fail guid=" << pPlayer->GetPlayerGUID() << " " << errmsg << log_args << " T=" << __show_t__ / 10000.0; \
	}

// 发送错误消息给玩家
#define MSG_ERR_TO(ns, name, error, errmsg, meta) \
	{\
		ns::name send;\
		send.set_error(error);\
		send.set_errmsg(errmsg);\
		From_Gws_Session::Me()->Send(pPlayer->GetSessionID(), &send, meta);\
	}

// 消息执行失败,发送错误消息给玩家
#define MSG_EXEC_FAIL_2P(ns, name, error, errmsg, log_args) \
	MSG_ERR_TO(ns, name, error, errmsg, From_Gws_Meta(pPlayer->GetPlayerGUID()));\
	MSG_EXEC_FAIL(errmsg, log_args);

// 消息执行失败,发送错误消息给玩家
#define MSG_EXEC_FAIL_2PM(ns, name, error, errmsg, log_args) \
	MSG_ERR_TO(ns, name, error, errmsg, *pMeta);\
	MSG_EXEC_FAIL(errmsg, log_args);
