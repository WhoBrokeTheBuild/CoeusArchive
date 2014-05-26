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

#ifndef ARC_NET_ARC_NET_VERSION_H
#define ARC_NET_ARC_NET_VERSION_H

#include <Arc/ArcCore.h>
#include <Arc/Types.h>

namespace Arc
{

#define ARC_NET_MAJOR_VERSION 0
#define ARC_NET_MINOR_VERSION 1
#define ARC_NET_PATCH_VERSION 5

inline Arc_uint8_t ArcNet_GetMajorVersion( void )
{
	return ARC_NET_MAJOR_VERSION;
}

inline Arc_uint8_t ArcNet_GetMinorVersion( void )
{
	return ARC_NET_MINOR_VERSION;
}

inline Arc_uint8_t ArcNet_GetPatchVersion( void )
{
	return ARC_NET_PATCH_VERSION;
}

inline Arc_Version ArcNet_GetVersion( void )
{
	Arc_Version current;
	current.Major = ARC_NET_MAJOR_VERSION;
	current.Minor = ARC_NET_MINOR_VERSION;
	current.Patch = ARC_NET_PATCH_VERSION;
	return current;
}

inline string ArcNet_GetVersionString( void )
{
	char version[15];

#if defined(ARC_OS_WINDOWS)

	sprintf_s(version, "%d.%d.%d", ARC_NET_MAJOR_VERSION, ARC_NET_MINOR_VERSION, ARC_NET_PATCH_VERSION);

#else

	sprintf(version, "%d.%d.%d", ARC_NET_MAJOR_VERSION, ARC_NET_MINOR_VERSION, ARC_NET_PATCH_VERSION);

#endif

	return string(version);
}

inline bool ArcNet_AboveVersion( Arc_uint8_t major, Arc_uint8_t minor, Arc_uint8_t patch )
{
	return ( (major >= ARC_NET_MAJOR_VERSION) && (minor >= ARC_NET_MINOR_VERSION) && (patch >= ARC_NET_PATCH_VERSION) );
}

inline bool ArcNet_AboveVersion( const Arc_Version& version )
{
	return ArcNet_AboveVersion(version.Major, version.Minor, version.Patch);
}

} // namespace Arc

#endif // ARC_NET_ARC_NET_VERSION_H
