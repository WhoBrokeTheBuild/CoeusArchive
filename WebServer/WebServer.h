#ifndef COEUS_WEB_SERVER_WEB_SERVER_H
#define COEUS_WEB_SERVER_WEB_SERVER_H

#include <Arc/ManagedObject.h>

#include <Arc/Socket.h>
#include <Arc/ServerSocket.h>

#include <Arc/Map.h>

class ServerConfig;

using namespace std;
using namespace Arc;

class WebServer :
	public Arc::ManagedObject
{
public:

	WebServer( void );
	virtual ~WebServer( void );

	virtual inline string getClassName( void ) const { return "Server"; }

	inline string getVersionString( void ) const { return "0.3"; }

	inline int getVersionMajor( void ) const { return 0; }
	inline int getVersionMinor( void ) const { return 3; }

	ServerConfig* getServerConfig();

	bool run( void );

protected:

	Map<string, string> getHeaders( void );

	std::streamsize getFileSize( std::ifstream& file );

	string recvLine( void );

	bool sendString( const string& str );
	bool sendLine( const string& line );
	bool sendFile( std::ifstream& file );
	void sendDirectoryList( const string& path, const string& realPath );

	static void spawnWorker( WebServer* pServer, Socket* pSocket );

	ServerConfig*		mp_ServerConfig;

	ServerSocket*		mp_ServerSocket;

};

#endif // COEUS_WEB_SERVER_WEB_SERVER_H