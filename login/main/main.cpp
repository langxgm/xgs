
#include "LoginServer.h"

int main(int argc, char** argv)
{
	LoginServer::InitInstance();
	LoginServer::Me()->Init(argc, argv);
	LoginServer::Me()->Loop(20);
	LoginServer::DestroyInstance();
	return 0;
}
