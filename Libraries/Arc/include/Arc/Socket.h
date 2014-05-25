/* This file is part of ArcNet.
 * 
 * ArcNet is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * ArcNet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with ArcNet; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 * Author: Stephen Lane-Walsh
 * 
 */

#ifndef ARC_NET_SOCKET_H
#define ARC_NET_SOCKET_H

#include <Arc/ManagedObject.h>

#include <Arc/Buffer.h>

#include "SocketType.h"
#include "SocketState.h"
#include "IPAddress.h"

#if defined(ARC_OS_WINDOWS)

// Prevent conflict with namespace Arc and Arc()
#define NOGDI

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define ARC_INVALID_SOCKET INVALID_SOCKET

#elif defined(ARC_OS_LINUX)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ARC_INVALID_SOCKET -1

#endif

#include "ServerSocket.h"

namespace Arc
{

class Socket
	: public ManagedObject
{

	friend class ServerSocket;

public:

	inline Socket( void )
		: m_Socket(ARC_INVALID_SOCKET),
		  m_Address(),
		  m_Port(0),
		  m_Type(INVALID_SOCKET_TYPE),
		  m_State(SOCKET_STATE_CLOSED)
	{ }

	inline ~Socket( void ) { disconnect(); }

	virtual inline string getClassName( void ) const { return "Socket"; }

	bool connectTo( const IPAddress& addr, const unsigned int& port, const SocketType& type );
	bool connectTo( const string& hostname, const unsigned int& port, const SocketType& type );

	void disconnect( void );

	inline bool isOpen( void ) const { return (m_State == SOCKET_STATE_OPEN); }
	inline bool isClosed( void ) const { return (m_State == SOCKET_STATE_CLOSED); }
	inline bool hasError( void ) const { return (m_State == SOCKET_STATE_ERROR); }

	bool hasData( int timeoutMS = -1 );

	inline SocketState getState( void ) const { return m_State; }
	inline IPAddress getAddress( void ) const { return m_Address; }

	inline int sendString( const string& data, const bool& withNullTerm = true )
	{
		return sendBuffer(data.c_str(), data.length() + (withNullTerm ? 1 : 0));
	}

	inline int sendBuffer( const Buffer& buffer )
	{
		return sendBuffer(buffer.getRawBuffer(), buffer.getUsedSize());
	}

	inline int sendBuffer( const char* buffer, const unsigned int& length )
	{
		return send(m_Socket, buffer, length, 0);
	}

	inline bool sendBool  ( const bool& data )   { return sendData(data); }
	inline bool sendChar  ( const char& data )   { return sendData(data); }
	inline bool sendShort ( const short& data )  { return sendData(data); }
	inline bool sendInt   ( const int& data )    { return sendData(data); }
	inline bool sendLong  ( const long& data )   { return sendData(data); }
	inline bool sendFloat ( const float& data )  { return sendData(data); }
	inline bool sendDouble( const double& data ) { return sendData(data); }

	inline bool sendInt8  ( const Arc_int8_t& data )   { return sendData(data); }
	inline bool sendUInt8 ( const Arc_uint8_t& data )  { return sendData(data); }
	inline bool sendInt16 ( const Arc_int16_t& data )  { return sendData(data); }
	inline bool sendUInt16( const Arc_uint16_t& data ) { return sendData(data); }
	inline bool sendInt32 ( const Arc_int32_t& data )  { return sendData(data); }
	inline bool sendUInt32( const Arc_uint32_t& data ) { return sendData(data); }

	string recvString( void );
	string recvLine( void );

	int recvBuffer( const char* buffer, const int& length );
	int recvBuffer( const Buffer& buffer );
	
	bool   recvBool  ( void ) { return recvData<bool>(); }
	char   recvChar  ( void ) { return recvData<char>(); }
	short  recvShort ( void ) { return recvData<short>(); }
	int    recvInt   ( void ) { return recvData<int>(); }
	long   recvLong  ( void ) { return recvData<long>(); }
	float  recvFloat ( void ) { return recvData<float>(); }
	double recvDouble( void ) { return recvData<double>(); }
	
	Arc_int8_t   recvInt8  ( void ) { return recvData<Arc_int8_t>(); }
	Arc_uint8_t  recvUInt8 ( void ) { return recvData<Arc_uint8_t>(); }
	Arc_int16_t  recvInt16 ( void ) { return recvData<Arc_int16_t>(); }
	Arc_uint16_t recvUInt16( void ) { return recvData<Arc_uint16_t>(); }
	Arc_int32_t  recvInt32 ( void ) { return recvData<Arc_int32_t>(); }
	Arc_uint32_t recvUInt32( void ) { return recvData<Arc_uint32_t>(); }

protected:

#if defined(ARC_OS_WINDOWS)

	Socket( const SOCKET& socket, const SocketType& type );

#elif defined(ARC_OS_LINUX)

	Socket( const int& socket, const SocketType& type );

#endif;

	template <typename T>
	bool sendData( const T& data )
	{
		int bytes = send(m_Socket, (char*)&data, sizeof(T), 0);

		if (bytes == 0)
		{
			m_State = SOCKET_STATE_ERROR;
			//setError("recv() failed");
			disconnect();
			return false;
		}

		return true;
	}

	template <typename T>
	T recvData( void )
	{
		T buffer;

		int bytes = recv(m_Socket, (char*)&buffer, sizeof(T), 0);

		if (bytes == 0)
		{
			m_State = SOCKET_STATE_ERROR;
			//setError("recv() failed");
			disconnect();
			return T();
		}

		return buffer;
	}
	
#if defined(ARC_OS_WINDOWS)

	// The windows system reference to the socket
    SOCKET m_Socket;

	/* 
		* @returns: The windows system reference to the socket
		*/
    SOCKET getWinSocket( void ) const { return m_Socket; }

#elif defined(ARC_OS_LINUX)

	// The linux system reference to the socket
    int m_Socket;
		
	/* 
		* @returns: The linux system reference to the socket
		*/
    int getUnixSocket( void ) const { return m_Socket; }

#endif

	// The address of the destination host
	IPAddress			m_Address;

	// The port to connect to on the destination host
	unsigned int		m_Port;

	// The type of socket (TCP, UDP)
	SocketType			m_Type;

	// The state of the socket (Open, Closed, Error)
	SocketState			m_State;

}; // class Socket

}; // namespace Arc

#endif // ARC_NET_SOCKET_H