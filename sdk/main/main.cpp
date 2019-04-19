
#include "SdkServer.h"

int main(int argc, char** argv)
{
	SdkServer::InitInstance();
	SdkServer::Me()->Init(argc, argv);
	SdkServer::Me()->Loop(50);
	SdkServer::DestroyInstance();
	return 0;
}
