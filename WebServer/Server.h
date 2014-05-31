#ifndef COEUS_WEB_SERVER_SERVER_H
#define COEUS_WEB_SERVER_SERVER_H

#include <Arc/ManagedObject.h>

#include <Arc/Socket.h>
#include <Arc/ServerSocket.h>

#include <Arc/Map.h>

class ServerConfig;

using namespace std;
using namespace Arc;

class Server :
	public Arc::ManagedObject
{
public:

	Server( void );
	virtual ~Server( void );

	virtual inline string getClassName( void ) const { return "Server"; }

	inline string getVersionString( void ) const { return "0.2"; }

	inline int getVersionMajor( void ) const { return 0; }
	inline int getVersionMinor( void ) const { return 2; }

	bool run( void );

protected:

	Map<string, string> getHeaders( void );

	std::streamsize getFileSize( std::ifstream& file );

	string recvLine( void );

	bool sendString( const string& str );
	bool sendLine( const string& line );
	bool sendFile( std::ifstream& file );

	ServerConfig*		mp_ServerConfig;

	ServerSocket*		mp_ServerSocket;

	Socket*				mp_CurrClient;

};

#endif // COEUS_WEB_SERVER_SERVER_H