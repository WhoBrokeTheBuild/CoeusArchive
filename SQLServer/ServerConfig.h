#ifndef COEUS_SQL_SERVER_SERVER_CONFIG_H
#define COEUS_SQL_SERVER_SERVER_CONFIG_H

#include <Arc/Types.h>
#include <Arc/ManagedObject.h>

#include <Arc/Map.h>
#include <Arc/ArrayList.h>

#include <Arc/Buffer.h>

using namespace Arc;

class ServerConfig :
	public ManagedObject
{
public:

	inline ServerConfig( void ) 
		: m_DataRoot(),
		  m_Port()
	{ }

	virtual inline ~ServerConfig( void ) { }

	virtual inline string getClassName( void ) const { return "Server Config"; }

	bool loadMainConfig( const string& filename );
	
	inline string getDataRoot( void ) const { return m_DataRoot; }
	inline unsigned int getPort( void ) const { return m_Port; }

protected:

	string cleanWhitespace( const string& str );

	string							m_DataRoot;

	unsigned int					m_Port;

};

#endif // COEUS_SQL_SERVER_SERVER_CONFIG_H