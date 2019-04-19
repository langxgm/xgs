//------------------------------------------------------------------------
// * @filename: HttpHandleQueue.h
// *
// * @brief: HTTP处理队列
// *
// * @author: XGM
// * @date: 2018/08/06
//------------------------------------------------------------------------
#pragma once

#include "xbase/Singleton.h"

#include <set>
#include <string>
#include <functional>
#include <atomic>

// 队列元素
struct HttpQueueElem
{
	typedef std::function<void(int64_t, void*) > HandleSuccess;
	typedef std::function<void(int32_t nError, const std::string& strErrMsg)> HandleFail;

	int32_t nType = 0; // 元素类型
	int64_t nSN = 0; // 流水号
	int64_t nExpire = 0; // 到期时间
	HandleSuccess handleSuccess; // 处理成功
	HandleFail handleFail; // 处理失败
};

inline bool operator==(const HttpQueueElem& lhs, const HttpQueueElem& rhs)
{
	return lhs.nSN == rhs.nSN;
}

inline bool operator<(const HttpQueueElem& lhs, const HttpQueueElem& rhs)
{
	return lhs.nSN < rhs.nSN;
}

class HttpHandleQueue : public Singleton<HttpHandleQueue>
{
public:
	typedef std::set<HttpQueueElem> Queue;
protected:
	friend class Singleton<HttpHandleQueue>;
	HttpHandleQueue();
	virtual ~HttpHandleQueue();
public:
	//------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------
	void Update();

	//------------------------------------------------------------------------
	// 添加元素
	//------------------------------------------------------------------------
	void AddElement(HttpQueueElem&& rElem);

	//------------------------------------------------------------------------
	// 取出元素
	//------------------------------------------------------------------------
	HttpQueueElem PopElement(int64_t nSN);

	//------------------------------------------------------------------------
	// 是否存在元素
	//------------------------------------------------------------------------
	bool IsExistElement(int64_t nSN);

	//------------------------------------------------------------------------
	// 获得元素数量
	//------------------------------------------------------------------------
	uint32_t GetSize();

	//------------------------------------------------------------------------
	// 生成序列号
	//------------------------------------------------------------------------
	int64_t GenSN();

	//------------------------------------------------------------------------
	// 超时时间
	//------------------------------------------------------------------------
	int64_t GetTimeout() const { return m_nTimeout; }
	void SetTimeout(int64_t nTimeout) { m_nTimeout = nTimeout; }

private:
	// 排队队列
	Queue m_Elements;
	// 生成序列号
	std::atomic<int64_t> m_nSerialNumber = { 0 };
	// 超时时间
	int64_t m_nTimeout = 0;
};
