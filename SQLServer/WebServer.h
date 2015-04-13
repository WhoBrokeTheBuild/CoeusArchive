#ifndef COEUS_SQL_SERVER_SERVER_H
#define COEUS_SQL_SERVER_SERVER_H

#include <Arc/ManagedObject.h>

class WebServer :
	public Arc::ManagedObject
{
public:

	WebServer();
	virtual ~WebServer();

	ServerConfig* getServerConfig();

	virtual inline string getClassName(void) const { return "Web Server"; }

	bool run(void);

private:

	ServerConfig*		mp_ServerConfig;

	ServerSocket*		mp_ServerSocket;


};

#endif // COEUS_SQL_SERVER_SERVER_H