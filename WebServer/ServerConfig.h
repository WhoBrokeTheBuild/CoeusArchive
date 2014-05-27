#ifndef COEUS_WEB_SERVER_SERVER_CONFIG_H
#define COEUS_WEB_SERVER_SERVER_CONFIG_H

#include <Arc/Types.h>
#include <Arc/ManagedObject.h>

#include <Arc/Map.h>
#include <Arc/ArrayList.h>

using namespace Arc;

class ServerConfig :
	public ManagedObject
{
public:

	inline ServerConfig( void ) 
		: m_MimeTypes(),
		  m_WebRoot(),
		  m_Port()
	{ }

	virtual inline ~ServerConfig( void ) { }

	virtual inline string getClassName( void ) const { return "Server Config"; }

	bool loadMainConfig( const string& filename );
	bool loadMIMETypes( const string& filename );

	string getMIMEType( const string& extension ) const;

	ArrayList<string> getDefaults( void ) const { return m_Defaults; }

	inline string getWebRoot( void ) const { return m_WebRoot; }
	inline unsigned int getPort( void ) const { return m_Port; }
	inline string getErrorPage404( void ) const { return m_ErrorPage404; }

protected:

	string cleanWhitespace( const string& str );
	
	Map<string, ArrayList<string>>	m_MimeTypes;

	ArrayList<string>				m_Defaults;

	string							m_WebRoot,
									m_ErrorPage404;

	unsigned int					m_Port;

};

#endif // COEUS_WEB_SERVER_SERVER_CONFIG_H