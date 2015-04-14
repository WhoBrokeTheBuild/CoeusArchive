#ifndef COEUS_SQL_SERVER_WORKER_H
#define COEUS_SQL_SERVER_WORKER_H

#include <Arc/ManagedObject.h>

#include <Arc/Socket.h>

using Arc::Socket;

class SQLServer;

class Worker :
	public Arc::ManagedObject
{
public:
	
	Worker(SQLServer* pServer, Socket* pSocket) :
		mp_CurrClient(pSocket),
		mp_Server(pServer)
	{ }
	virtual ~Worker() { }

	virtual inline string getClassName( void ) const { return "Worker"; }

	bool run( void );

protected:

	Socket*			mp_CurrClient;
	SQLServer*		mp_Server;

};

#endif // COEUS_SQL_SERVER_WORKER_H