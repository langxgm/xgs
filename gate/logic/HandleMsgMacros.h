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
#define MSG_GC_PLAYER_CHECK_OF(ns, name) MSG_CHECK_OF(ns::name); \
	auto pPlayer = PlayerManager::Me()->GetPlayer(pMeta->GetGUID());\
	if (!pPlayer)\
	{\
		VLOG(0) << "Handle"#name"/fail guid=" << pMeta->GetGUID() << " not logged in";\
		return; /*没登录*/\
	}
