/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         whowas.cpp
 * Description  WHOWAS command handler
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
 
#ifndef _UNREALIRCD_CMD_WHOWAS_HPP
#define _UNREALIRCD_CMD_WHOWAS_HPP
 
#include "base.hpp"
#include "map.hpp"
#include "string.hpp"
#include "stringlist.hpp"

#define CMD_WHOWAS	"WHOWAS"
#define TOK_WHOWAS	"WW"

/**
 * WHOWAS entry type.
 */
struct WhowasEntry_t
{
	/** nickname */
	String nick;

	/** username */
	String user;

	/** hostname */
	String hostname;

	/** real name */
	String realname;
};

/**
 * WHOWAS command class.
 */
class WHOWAS
{
public:
	static void exec(UnrealUser* uptr, StringList* argv);
	static void handleLeavingUser(UnrealUser* uptr);

public:
	static std::multimap<String, WhowasEntry_t> entries;

public:
	/** command name */
	static const char* COMMAND;

	/** protocol token name */
	static const char* TOKEN;
};
 
#endif /* _UNREALIRCD_CMD_WHOWAS_HPP */
 