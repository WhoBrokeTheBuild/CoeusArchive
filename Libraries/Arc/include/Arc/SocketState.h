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

#ifndef ARC_NET_SOCKET_STATE_H
#define ARC_NET_SOCKET_STATE_H

#include <string>

using std::string;

namespace Arc
{

enum SocketState
{
	INVALID_SOCKET_STATE = -1,

	SOCKET_STATE_CLOSED,
	SOCKET_STATE_OPEN,
	SOCKET_STATE_ERROR,

	NUM_SOCKET_STATES

}; // enum SocketType

const string SOCKET_STATE_NAMES[NUM_SOCKET_STATES] = 
{
	"Closed",
	"Open",
	"Error"
};

} // namespace Arc

#endif // ARC_NET_SOCKET_STATE_H