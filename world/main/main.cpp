
#include "WorldServer.h"

int main(int argc, char** argv)
{
	WorldServer::InitInstance();
	WorldServer::Me()->Init(argc, argv);
	WorldServer::Me()->Loop(50);
	WorldServer::DestroyInstance();
	return 0;
}
