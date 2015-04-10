#include "Server.h"

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Log.h>
#include <dirent.h>
#include <thread>

#include "ServerConfig.h"
#include "Worker.h"

using namespace std;

Server::Server( void )
{
	mp_ServerConfig = New ServerConfig();

	mp_ServerConfig->loadMainConfig("conf/main.cfg");
	mp_ServerConfig->loadMIMETypes("conf/mime.cfg");
}

Server::~Server( void )
{
	Log::Info(getClassName(), "Shutting down");
	delete mp_ServerConfig;

	if (mp_CurrClient)
		mp_CurrClient->disconnect();
	delete mp_CurrClient;

	if (mp_ServerSocket)
		mp_ServerSocket->disconnect();
	delete mp_ServerSocket;
}

ServerConfig* Server::getServerConfig(void)
{
	return mp_ServerConfig;
}

bool Server::run( void )
{
	mp_ServerSocket = New ServerSocket();
	mp_ServerSocket->bindLocal(mp_ServerConfig->getPort(), SOCKET_TYPE_TCP);

	if (mp_ServerSocket->hasError())
	{
		Log::Error(getClassName(), "Server Socket failed to initialize");
		mp_ServerSocket->disconnect();
		delete mp_ServerSocket;
		return false;
	}

	Log::InfoFmt(getClassName(), "Listening on port %d", mp_ServerConfig->getPort());

	while (true)
	{
		mp_CurrClient = mp_ServerSocket->acceptClient();
		Log::InfoFmt(getClassName(), "Connection from %s", mp_CurrClient->getAddress().toString().c_str());

		thread workerThread(Server::spawnWorker, this, mp_CurrClient);
		workerThread.detach();
	}
}


void Server::spawnWorker( Server* pServer, Socket* pSocket )
{
	Worker worker(pServer, pSocket);
	worker.run();
	delete pSocket;
}
