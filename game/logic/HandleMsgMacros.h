//------------------------------------------------------------------------
// * @filename: HandleMsgMacros.h
// *
// * @brief: 处理消息的宏定义
// *
// * @author: XGM
// * @date: 2018/08/15
//------------------------------------------------------------------------
#pragma once

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
	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID(nSessionID);\
	if (!pPlayer)\
	{\
		VLOG(0) << "Handle"#name"/fail sid=" << nSessionID << " not logged in";\
		return; /*没登录*/\
	}

// 消息转换并检测Player
#define MSG_PLAYER_CHECK_CAST_OF(ns, name, player_type)  MSG_CHECK_OF(ns::name); \
	auto pPlayer = PlayerManager::Me()->GetPlayerBySessionID_cast<player_type>(nSessionID);\
	if (!pPlayer)\
	{\
		VLOG(0) << "Handle"#name"/fail sid=" << nSessionID << " not logged in";\
		return; /*没登录*/\
	}

// 游戏CG错误消息发给玩家
#define CG_MSG_ERR_TO(ns, shortname, error, errmsg, log_args, meta) \
	{\
		ns::GC##shortname send;\
		send.set_error(error);\
		send.set_errmsg(errmsg);\
		From_Client_WebSession::Me()->Send(pPlayer->GetSessionID(), &send);\
		VLOG(0) << "HandleCG"#shortname"/fail guid=" << pPlayer->GetPlayerGUID() << " " << errmsg << log_args;\
	}

// 游戏CG错误消息发给玩家
#define CG_MSG_ERR_TO_PLAYER(ns, shortname, error, errmsg, log_args) \
	CG_MSG_ERR_TO(ns, shortname, error, errmsg, log_args, void)

// 房间CG消息转换并检测
#define CG_MSG_INROOM_CHECK_OF(ns, shortname, room_type) \
	auto pRoomUnit = static_cast<room_type*>(pPlayer->GetRoomUnit());\
	if (!pRoomUnit)\
	{\
		CG_MSG_ERR_TO_PLAYER(ns, shortname, 200, "not in the room", "");/*不在房间里*/\
		return;\
	}

// 房间CG消息转换并检测
#define CG_MSG_INROOM_PLAYING_CHECK_OF(ns, shortname, room_type) \
	CG_MSG_INROOM_CHECK_OF(ns, shortname, room_type);\
	if (pRoomUnit->IsPlaying() == false)\
	{\
		CG_MSG_ERR_TO_PLAYER(ns, shortname, 300, "not playing", " roomUnitSN=" << pRoomUnit->GetSN());/*不是游戏中*/\
		return; \
	}

// 游戏CG错误消息发给玩家
#define CG_MSG_COMMONERR_TO(ns, shortname, error, errmsg, log_args, meta) \
	{\
		ns::GC##CommonError send;\
		send.set_err_id(error);\
		send.set_err_desc(errmsg);\
		From_Client_WebSession::Me()->Send(pPlayer->GetSessionID(), &send);\
		VLOG(0) << "HandleCG"#shortname"/fail guid=" << pPlayer->GetPlayerGUID() << " " << errmsg << log_args;\
	}

// 游戏CG错误消息发给玩家
#define CG_MSG_COMMONERR_TO_PLAYER(ns, shortname, error, errmsg, log_args) \
	CG_MSG_COMMONERR_TO(ns, shortname, error, errmsg, log_args, void)

// 房间CG消息转换并检测
#define CG_MSG_INROOM_COMMONERR_CHECK_OF(ns, shortname, room_type) \
	auto pRoomUnit = static_cast<room_type*>(pPlayer->GetRoomUnit());\
	if (!pRoomUnit)\
	{\
		CG_MSG_COMMONERR_TO_PLAYER(ns, shortname, 200, "not in the room", "");/*不在房间里*/\
		return;\
	}

// 房间CG消息转换并检测
#define CG_MSG_INROOM_PLAYING_COMMONERR_CHECK_OF(ns, shortname, room_type) \
	CG_MSG_INROOM_COMMONERR_CHECK_OF(ns, shortname, room_type);\
	if (pRoomUnit->IsPlaying() == false)\
	{\
		CG_MSG_COMMONERR_TO_PLAYER(ns, shortname, 300, "not playing", " roomUnitSN=" << pRoomUnit->GetSN());/*不是游戏中*/\
		return; \
	}
