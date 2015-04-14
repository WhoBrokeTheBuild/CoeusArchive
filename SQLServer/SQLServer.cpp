#include "SQLServer.h"

#include <Arc/Log.h>
#include "ServerConfig.h"

#include "Worker.h"

#include <thread>

SQLServer::SQLServer() :
	mp_ServerConfig(nullptr)
{
	mp_ServerConfig = New ServerConfig();
	mp_ServerConfig->loadMainConfig("conf/main.cfg");
}

SQLServer::~SQLServer()
{
	Log::Info(getClassName(), "Shutting down");
	delete mp_ServerConfig;
	mp_ServerConfig = nullptr;
}

ServerConfig* SQLServer::getServerConfig( void )
	{ return mp_ServerConfig; }

bool SQLServer::run(void)
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

		std::thread workerThread(SQLServer::spawnWorker, this, pSocket);
		workerThread.detach();
	}

	return true;
}

void SQLServer::spawnWorker(SQLServer* pServer, Socket* pSocket)
{
	Worker worker(pServer, pSocket);
	worker.run();
	delete pSocket;
}
