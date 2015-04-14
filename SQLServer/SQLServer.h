#ifndef COEUS_SQL_SERVER_SQL_SERVER_H
#define COEUS_SQL_SERVER_SQL_SERVER_H

#include <Arc/ManagedObject.h>

#include <Arc/Socket.h>
#include <Arc/ServerSocket.h>

using Arc::Socket;
using Arc::ServerSocket;

class ServerConfig;

class SQLServer :
	public Arc::ManagedObject
{
public:

	SQLServer();
	virtual ~SQLServer();

	virtual inline string getClassName( void ) const { return "SQL Server"; }

	inline string getVersionString( void ) const { return "0.1"; }

	inline int getVersionMajor( void ) const { return 0; }
	inline int getVersionMinor( void ) const { return 1; }

	ServerConfig* getServerConfig( void );

	bool run(void);

private:

	static void spawnWorker(SQLServer* pServer, Socket* pSocket);

	ServerConfig*	mp_ServerConfig;

	ServerSocket*	mp_ServerSocket;
};

#endif // COEUS_SQL_SERVER_SQL_SERVER_H