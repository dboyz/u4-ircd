/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         stats.hpp
 * Description  Statistical stuff
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
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

#ifndef _UNREALIRCD_STATS_HPP
#define _UNREALIRCD_STATS_HPP

#include "platform.hpp"
#include <cstring>

/**
 * Local statistics that allows us to query statistical stuff quickly.
 */
struct UnrealLocalStat
{
	UnrealLocalStat()
	{
		std::memset(this, 0, sizeof(UnrealLocalStat));
	}

	/** invisible user count */
	uint32_t users_inv;

	/** max global user count */
	uint32_t users_max;

	/** local user count */
	uint32_t users_local_cur;

	/** max local user count */
	uint32_t users_local_max;

	/** unknown connection count */
	uint32_t connections_unk;

	/** current total connection count */
	uint32_t connections_cur;

	/** max connection count */
	uint32_t connections_max;

	/** server count on the network */
	uint32_t servers;

	/** local server count */
	uint32_t servers_local;

	/** IRC operator count */
	uint32_t operators;

	/** total connection count */
	uint32_t connections_total;
};

#endif /* _UNREALIRCD_STATS_HPP */
