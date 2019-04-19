
#include "GateServer.h"

int main(int argc, char** argv)
{
	GateServer::InitInstance();
	GateServer::Me()->Init(argc, argv);
	GateServer::Me()->Loop(20);
	GateServer::DestroyInstance();
	return 0;
}
