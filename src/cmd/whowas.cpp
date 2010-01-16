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

#include "base.hpp"
#include "command.hpp"
#include "module.hpp"
#include "stringlist.hpp"
#include "include/whowas.hpp"
#include <iostream>

/** WHOWAS entry mapping */
std::multimap<String, WhowasEntry_t> WHOWAS::entries;

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"WHOWAS command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/** Command name */
const char* WHOWAS::COMMAND = "WHOWAS";

/** Command token */
const char* WHOWAS::TOKEN   = "WW";

/**
 * WHOWAS command handler for User connections.
 *
 * Usage:
 * WHOWAS <nickname> [<count> [<server>]]
 *
 * Message example:
 * WHOWAS codemastr
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void WHOWAS::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				WHOWAS::COMMAND));
	}

	size_t count = -1, current = 0;

	if (argv->size() > 2)
		count = argv->at(2).toSize();

	/* use lowercase nick to search through the history */
	String ln = argv->at(1).toLower();

	std::multimap<String, WhowasEntry_t>* history = &WHOWAS::entries;
	std::multimap<String, WhowasEntry_t>::const_iterator wi;
	
	for (wi = history->find(ln); wi != history->end(); ++wi)
	{
		if (wi->first != ln)
			continue;
		else if (current++ >= count)
			break;

		const WhowasEntry_t* e = &(wi->second);
		
		uptr->sendreply(RPL_WHOWASUSER,
			String::format(MSG_WHOWASUSER,
				e->nick.c_str(),
				e->user.c_str(),
				e->hostname.c_str(),
				e->realname.c_str()));
	}

	uptr->sendreply(RPL_ENDOFWHOWAS, MSG_ENDOFWHOWAS);
}

/**
 * Event handler function that is called when a user object is about to be
 * destroyed. We use that to catch the nick for the WHOWAS entries map.
 *
 * @param uptr User pointer of leaving user
 */
void WHOWAS::handleLeavingUser(UnrealUser* uptr)
{
	static size_t history_limit = unreal->config.get("Limits::WhowasHistory",
		"600").toSize();
	String ln = uptr->lowerNick();
	std::multimap<String, WhowasEntry_t>* history = &WHOWAS::entries;
	
	/* limit the number of entries in the WHOWAS history list.
	 * if that limit exceeds, remove the last entry from the list. */
	if (history->size() >= history_limit)
	{
		std::multimap<String, WhowasEntry_t>::iterator ent_iter=history->end();
		
		if (ent_iter != history->begin())
			history->erase(ent_iter);
	}

	WhowasEntry_t wwent;

	wwent.nick = uptr->nick();
	wwent.user = uptr->ident();
	wwent.hostname = uptr->hostname();
	wwent.realname = uptr->realname();

	/* add into the history map */
	history->insert(std::pair<String, WhowasEntry_t>(ln, wwent));
}

/**
 * Module initialization function.
 * Called when the Module is loaded.
 *
 * @param module Reference to Module
 */
UNREAL_DLL UnrealModule::Result unrInit(UnrealModule& module)
{
	/* update module info */
	module.info = modinf;

	/* register command */
	uc = new UnrealUserCommand(WHOWAS::COMMAND, &WHOWAS::exec);

	UnrealUser::onDestroy.connect(&WHOWAS::handleLeavingUser);

	return UnrealModule::Success;
}

/**
 * Module close function.
 * It's called before the Module is unloaded.
 */
UNREAL_DLL UnrealModule::Result unrClose(UnrealModule& module)
{
	delete uc;

	UnrealUser::onDestroy.disconnect(&WHOWAS::handleLeavingUser);

	return UnrealModule::Success;
}
