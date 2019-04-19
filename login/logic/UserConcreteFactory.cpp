
#include "UserConcreteFactory.h"
#include "User.h"

User* UserDefaultFactory::NewUser()
{
	return new User();
}
