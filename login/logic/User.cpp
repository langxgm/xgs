
#include "User.h"

User::User()
{

}

User::~User()
{

}

void User::Init(int64_t nUserID, int64_t nLinkSessionID)
{
	m_nUserID = nUserID;
	m_nLinkSessionID = nLinkSessionID;
}

void User::Reconnect(int64_t nLinkSessionID)
{
	m_nLinkSessionID = nLinkSessionID;
}

void User::Online()
{
	m_bOnline = true;
}

void User::Offline()
{
	m_bOnline = false;
}
