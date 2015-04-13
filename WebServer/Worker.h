#ifndef COEUS_WEB_SERVER_WORKER_H
#define COEUS_WEB_SERVER_WORKER_H

#include <Arc/ManagedObject.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <Arc/ArcCore.h>
#include <Arc/Log.h>
#include <Arc/Socket.h>

using namespace std;
using namespace Arc;

class WebServer;

class Worker :
	public Arc::ManagedObject
{
public:
	Worker(WebServer* pServer, Socket* pSocket) :
		mp_CurrClient(pSocket),
		mp_Server(pServer)
	{ }
	virtual ~Worker();

	virtual inline string getClassName( void ) const { return "Worker"; }

	bool run();

	Map<string, string> getHeaders();
	std::streamsize getFileSize(std::ifstream& file);
	string recvLine();
	bool sendString(const string& str);
	bool sendLine(const string& line);
	bool sendFile(std::ifstream& file);
	void sendDirectoryList(const string& path, const string& realPath);

protected:

	Socket* mp_CurrClient;
	WebServer* mp_Server;
};

#endif // COEUS_WEB_SERVER_WORKER_H