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

#include "channel.h"
#include "modules.h"

bool Channel::isValidChannel(const std::string& channame)
{
	if (channame.empty())
	{
		return false;
	}
	MODULARIZE_FUNCTION(I_OnValidChannelCheck, OnValidChannelCheck(channame.c_str()) );
	return true;
}

int Channel::KickUser(User * from, User * to, const std::string& reason)
{
	if (!from || !to || reason.empty())
	{
		return -1;
	}
	MODULARIZE_FUNCTION(I_OnUserKick, OnUserKick(from, to, reason.c_str()));
}
