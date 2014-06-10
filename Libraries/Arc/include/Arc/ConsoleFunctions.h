/* This file is part of ArcCore.
 *
 * ArcCore is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ArcCore is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ArcCore; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author: Stephen Lane-Walsh
 *
 */

#ifndef ARC_CORE_CONSOLE_FUNCTIONS_H
#define ARC_CORE_CONSOLE_FUNCTIONS_H

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::cin;

namespace Arc
{

inline void Arc_Pause( const string& msg = "Press enter to continue" )
{
	static string tmp;

	cout << msg;
	cin.clear();
	getline(cin, tmp);
	cout << "\n";
}

}

#endif // ARC_CORE_CONSOLE_FUNCTIONS_H