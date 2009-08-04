/*  UnrealIRCd -- The Next Generation IRCd.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 1, or (at your option)
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

enum ModuleTypes
{
	/* If a module type is STATIC, it CANNOT be unloaded by ANY
	*  means.
	*/
	STATIC,
	/* If a module type is NOTVENDOR, it is treated as any other
	*  module, but it is defined as 3rd party. (i.e not from us)
	*/
	NOTVENDOR,
	/* If a module type is VENDOR, it is treated as any other
	*  module, but it is defined as it came with this
	*  distribution.
	*/
	VENDOR
};
