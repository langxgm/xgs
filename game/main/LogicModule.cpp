
#include "LogicModule.h"
#include "GameServer.h"
#include "game/logic/LoginHandler.h"
#include "game/logic/GameDataHandler.h"
#include "game/logic/MailHandler.h"
#include "game/logic/FeedbackHandler.h"
#include "game/logic/ScriptHandler.h"

#include "game/config/SystemConfig.h"
#include "game/pbconfig/game.conf.pb.h"

#include "xshare/work/LogicManager.h"
#include "xshare/work/WorkerPool.h"
#include "xshare/work/WorkDispatcher.h"

#include <libgo/coroutine.h>

std::string LogicModule::GetName()
{
	return "LogicModule";
}

bool LogicModule::Init()
{
	auto& logicConf = SystemConfig::Me()->GetGameConfig()->logicmodule();

	// 设置协程配置
#if _DEBUG
	//co_sched.GetOptions().debug = co::dbg_task | co::dbg_ioblock | co::dbg_wait;
	co_sched.GetOptions().stack_size = 2048;
	co_sched.GetOptions().enable_coro_stat = true;
#endif
	// 窃取其他线程的任务,防止线程饥饿
	//co_sched.GetOptions().enable_work_steal = true;

	// 初始化工作池
#if _DEBUG && _WIN32
	uint32_t work_thread_num = 2;
#else
	uint32_t work_thread_num = logicConf.work_thread_num()/*10*/;
#endif
	WorkerPool::InitInstance(GameServer::Me()->GetMainEventLoop(), work_thread_num);
	WorkerPool::Me()->Start();
	for (uint32_t i = 0; i < work_thread_num; ++i)
	{
		WorkDispatcherAsync(i).RunEvery(evpp::Duration(1), []() {
			co_sched.Run();
		});
	}

	// 初始化逻辑管理
	LogicManager::InitInstance();

	// 初始化逻辑处理者
	LogicManager::Me()->RegLogic(LOGIC_NAME(LoginHandler), LoginHandler::InitInstance());
	LogicManager::Me()->RegLogic(LOGIC_NAME(GameDataHandler), GameDataHandler::InitInstance());
	LogicManager::Me()->RegLogic(LOGIC_NAME(MailHandler), MailHandler::InitInstance());
	LogicManager::Me()->RegLogic(LOGIC_NAME(FeedbackHandler), FeedbackHandler::InitInstance());
	LogicManager::Me()->RegLogic(LOGIC_NAME(ScriptHandler), ScriptHandler::InitInstance());
	LogicManager::Me()->PrintAll(std::cout);
	return true;
}

void LogicModule::Exit()
{
	// 销毁逻辑管理
	LogicManager::DestroyInstance();

	// 销毁工作池
	WorkerPool::Me()->Stop();
	WorkerPool::DestroyInstance();
}

void LogicModule::RunOnce()
{
	// 主线程的协程调度
	co_sched.Run(co::Scheduler::erf_all ^ co::Scheduler::erf_idle_cpu);

	// 更新逻辑
	for (auto pLogic : *LogicManager::Me())
	{
		pLogic->UpdateLogic();
	}
}
