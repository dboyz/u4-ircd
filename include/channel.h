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

#ifndef _UNREALIRCD_CHANNEL_H
#define _UNREALIRCD_CHANNEL_H

#include "user.h"

#include <string>

class Channel
{
public:
	/*
	 * returns true if this channel name is allowed on this
	 * server. Uses I_OnValidChannelCheck hook.
	 */
	bool isValidChannel(const std::string& channame);
	
	/*
	 * kicks user from channel. Uses I_OnUserKick hook.
	 */
	int KickUser(User *from, User *to, const std::string& reason);
};

#endif
