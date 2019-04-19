
#include "HttpHandleQueue.h"

#include "xbase/TimeUtil.h"

HttpHandleQueue::HttpHandleQueue()
{

}

HttpHandleQueue::~HttpHandleQueue()
{

}

void HttpHandleQueue::Update()
{
	int64_t nNow = TimeUtil::GetCurrentTimeMillis();
	for (auto it = m_Elements.begin(); it != m_Elements.end();)
	{
		auto& rElem = *it;
		if (nNow > rElem.nExpire)
		{
			rElem.handleFail(2001, "handle timeout");
			it = m_Elements.erase(it);
		}
		else
		{
			break;
		}
	}
}

void HttpHandleQueue::AddElement(HttpQueueElem&& rElem)
{
	m_Elements.insert(std::move(rElem));
}

HttpQueueElem HttpHandleQueue::PopElement(int64_t nSN)
{
	HttpQueueElem findElem;
	{
		findElem.nSN = nSN;
		auto it = m_Elements.find(findElem);
		if (it != m_Elements.end())
		{
			auto elem = std::move(*it);
			m_Elements.erase(it);
			return std::move(elem);
		}
	}

	findElem.nSN = 0;
	return std::move(findElem);
}

bool HttpHandleQueue::IsExistElement(int64_t nSN)
{
	HttpQueueElem findElem;
	findElem.nSN = nSN;
	return m_Elements.find(findElem) != m_Elements.end();
}

uint32_t HttpHandleQueue::GetSize()
{
	return m_Elements.size();
}

int64_t HttpHandleQueue::GenSN()
{
	int64_t nSN = ++m_nSerialNumber;
	int64_t nNowSeconds = TimeUtil::GetCurrentTimeSeconds();

	const int64_t nMod = 100000;
	nSN = nNowSeconds * nMod + nSN % nMod;
	return nSN;
}
