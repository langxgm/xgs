
#include "NetHeartbeat.h"

#include "xbase/TimeUtil.h"

void NetHeartbeat::OnSendAction()
{
	++m_nSendCount;
	m_nLastSendTime = TimeUtil::GetCurrentTimeMillis();
}

void NetHeartbeat::OnRecvAction()
{
	m_nSendCount = 0;
	m_nLastSendTime = 0;
}

void NetHeartbeat::Reset()
{
	m_nSendCount = 0;
	m_nLastSendTime = 0;
}

int32_t NetHeartbeat::GetCount()
{
	return m_nSendCount;
}

int64_t NetHeartbeat::GetLastSendTime()
{
	return m_nLastSendTime;
}
