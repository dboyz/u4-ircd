/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         stringlist.cpp
 * Description  Extension of std::list<String> for string concat
 *
 * Copyright(C) 2009, 2010
 * The UnrealIRCd development team and contributors
 * http://www.unrealircd.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 ******************************************************************/

#include "string.hpp"
#include "stringlist.hpp"

/**
 * Concatenate the Strings in List into a single String.
 *
 * @param glue If specified, glue is inserted between entries
 * @return Concatenated string
 */
String StringList::join(const String& glue)
{
	String result;

	for (Iterator i = begin(); i != end(); ++i)
	{
		result << *i;

		/* if glue has been specified, add them between entries */
		if (!glue.empty() && (i + 1) != end())
			result << glue;
	}

	return result;
}
