#include "Server.h"

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Log.h>
#include <dirent.h>

#include "ServerConfig.h"

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

		unsigned int socketID = mp_CurrClient->getRawSocket();

		spawnWorker(socketID);

		delete mp_CurrClient;
	}
}


void Server::spawnWorker( const unsigned int& socketID )
{

}
