
#include "LinkServer.h"

int main(int argc, char** argv)
{
	LinkServer::InitInstance();
	LinkServer::Me()->Init(argc, argv);
	LinkServer::Me()->Loop(20);
	LinkServer::DestroyInstance();
	return 0;
}
