
#include "GameServer.h"

int main(int argc, char** argv)
{
	GameServer::InitInstance();
	GameServer::Me()->Init(argc, argv);
	GameServer::Me()->Loop(20);
	GameServer::DestroyInstance();
	return 0;
}
