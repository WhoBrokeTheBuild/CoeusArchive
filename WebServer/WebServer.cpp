#include "WebServer.h"

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Log.h>
#include <dirent.h>
#include <thread>

#include "ServerConfig.h"
#include "Worker.h"

using namespace std;

WebServer::WebServer( void )
{
	mp_ServerConfig = New ServerConfig();

	mp_ServerConfig->loadMainConfig("conf/main.cfg");
	mp_ServerConfig->loadMIMETypes("conf/mime.cfg");
}

WebServer::~WebServer( void )
{
	Log::Info(getClassName(), "Shutting down");
	delete mp_ServerConfig;

	if (mp_ServerSocket)
		mp_ServerSocket->disconnect();
	delete mp_ServerSocket;
}

ServerConfig* WebServer::getServerConfig(void)
{
	return mp_ServerConfig;
}

bool WebServer::run( void )
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
		Socket* pSocket = mp_ServerSocket->acceptClient();
		Log::InfoFmt(getClassName(), "Connection from %s", pSocket->getAddress().toString().c_str());

		thread workerThread(WebServer::spawnWorker, this, pSocket);
		workerThread.detach();
	}

	return true;
}


void WebServer::spawnWorker( WebServer* pServer, Socket* pSocket )
{
	Worker worker(pServer, pSocket);
	worker.run();
	delete pSocket;
}
