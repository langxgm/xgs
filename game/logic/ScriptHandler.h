//------------------------------------------------------------------------
// * @filename: ScriptHandler.h
// *
// * @brief: 脚本逻辑处理
// *
// * @author: XGM
// * @date: 2018/12/03
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"
#include "xdb/DBHandler.h"
#include "xshare/work/LogicHandler.h"

class ScriptHandler : public LogicHandler, public DBHandler, public Singleton<ScriptHandler>
{
protected:
	friend class Singleton<ScriptHandler>;
	ScriptHandler();
	virtual ~ScriptHandler();
public:
	//------------------------------------------------------------------------
	// 逻辑名称
	//------------------------------------------------------------------------
	virtual std::string GetLogicName() override { return LOGIC_NAME(ScriptHandler); }

	//------------------------------------------------------------------------
	// 初始化数据库
	//------------------------------------------------------------------------
	virtual void PreInitDatabase() override;

	//------------------------------------------------------------------------
	// 更新逻辑
	//------------------------------------------------------------------------
	virtual void UpdateLogic() override;

public:
};
