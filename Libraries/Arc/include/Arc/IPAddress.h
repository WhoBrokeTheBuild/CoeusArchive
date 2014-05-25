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

#ifndef ARC_NET_IP_ADDRESS_H
#define ARC_NET_IP_ADDRESS_H

#include <Arc/ManagedObject.h>

#include <Arc/MathFunctions.h>

namespace Arc
{

class Socket;

struct IPAddress
	: public ManagedObject
{

	friend class Socket;

public:

	// Constant value of 0.0.0.0
	const static IPAddress ZERO;

	// Constant value of 255.255.255.255
	const static IPAddress MAX;

	inline IPAddress( void )
	{
		m_Quads[0] = m_Quads[1] = m_Quads[2] = m_Quads[3] = 0;
	}

	IPAddress( const string& addr );

	inline IPAddress( const Arc_uint8_t& a, const Arc_uint8_t& b, const Arc_uint8_t& c, const Arc_uint8_t& d )
	{
		m_Quads[0] = Arc_Clamp(a, (Arc_uint8_t)0, (Arc_uint8_t)255);
		m_Quads[1] = Arc_Clamp(b, (Arc_uint8_t)0, (Arc_uint8_t)255);
		m_Quads[2] = Arc_Clamp(c, (Arc_uint8_t)0, (Arc_uint8_t)255);
		m_Quads[3] = Arc_Clamp(d, (Arc_uint8_t)0, (Arc_uint8_t)255);
	}

	inline IPAddress( const IPAddress& rhs )
	{
		memcpy(m_Quads, rhs.m_Quads, sizeof(Arc_uint8_t) * 4);
	}

	virtual inline ~IPAddress( void ) { }

	virtual inline string getClassName( void ) const { return "IP Address"; }

	string toString( void ) const;

	inline bool operator==( const IPAddress& rhs ) const
	{
		return ( m_Quads[0] == rhs.m_Quads[0] &&
			     m_Quads[1] == rhs.m_Quads[1] &&
				 m_Quads[2] == rhs.m_Quads[2] &&
				 m_Quads[3] == rhs.m_Quads[3] );
	}

	inline bool operator!=( const IPAddress& rhs ) const
	{
		return !(*this == rhs);
	}

	inline Arc_uint8_t getA( void ) const { return m_Quads[0]; }
	inline Arc_uint8_t getB( void ) const { return m_Quads[1]; }
	inline Arc_uint8_t getC( void ) const { return m_Quads[2]; }
	inline Arc_uint8_t getD( void ) const { return m_Quads[3]; }

	inline void setA( Arc_uint8_t a ) { m_Quads[0] = a; }
	inline void setB( Arc_uint8_t b ) { m_Quads[1] = b; }
	inline void setC( Arc_uint8_t c ) { m_Quads[2] = c; }
	inline void setD( Arc_uint8_t d ) { m_Quads[3] = d; }

protected:

	Arc_uint8_t m_Quads[4];

	inline Arc_uint8_t* getRawData( void ) { return m_Quads; }

}; // class IPAddress

IPAddress Arc_HostnameLookup( const string& hostname );

}; // namespace Arc

#endif // ARC_NET_IP_ADDRESS_H