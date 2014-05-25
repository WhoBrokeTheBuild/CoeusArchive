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

#ifndef ARC_NET_SOCKET_TYPE_H
#define ARC_NET_SOCKET_TYPE_H

#include <string>

using std::string;

namespace Arc
{

enum SocketType
{
	INVALID_SOCKET_TYPE = -1,

	SOCKET_TYPE_TCP,
	SOCKET_TYPE_UDP,

	NUM_SOCKET_TYPES

}; // enum SocketType

const string SOCKET_TYPE_NAMES[NUM_SOCKET_TYPES] = 
{
	"TCP",
	"UDP"
};

} // namespace Arc

#endif // ARC_NET_SOCKET_TYPE_H