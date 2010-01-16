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

#include <base.hpp>
#include <command.hpp>
#include <stringlist.hpp>

#include <cmd/whowas.hpp>

/** WHOWAS entry mapping */
std::multimap<String, WhowasEntry_t> UnrealCH_whowas::entries;

/** class instance */
UnrealCH_whowas* handler = NULL;

/**
 * Unreal Command Handler for "WHOWAS" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_whowas::UnrealCH_whowas(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_WHOWAS, &UnrealCH_whowas::exec);

	/* connect signals we're interested in */
	UnrealUser::onDestroy.connect(&UnrealCH_whowas::handleLeavingUser);
}

/**
 * Unreal Command Handler for "WHOWAS" - Destructor.
 */
UnrealCH_whowas::~UnrealCH_whowas()
{
	delete command_;

	/* disconnect signals */
	UnrealUser::onDestroy.disconnect(&UnrealCH_whowas::handleLeavingUser);
}

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
void UnrealCH_whowas::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_WHOWAS));
	}

	size_t count = -1, current = 0;

	if (argv->size() > 2)
		count = argv->at(2).toSize();

	/* use lowercase nick to search through the history */
	String ln = argv->at(1).toLower();

	std::multimap<String, WhowasEntry_t>* history = &UnrealCH_whowas::entries;
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
void UnrealCH_whowas::handleLeavingUser(UnrealUser* uptr)
{
	static size_t history_limit = unreal->config.get("Limits::WhowasHistory",
		"600").toSize();
	String ln = uptr->lowerNick();
	std::multimap<String, WhowasEntry_t>* history = &UnrealCH_whowas::entries;
	
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
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_whowas::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /WHOWAS command");
	inf->setName("UnrealCH_whowas");
	inf->setVersion("1.0.0");
}

/**
 * Module initialization function.
 * Called when the Module is loaded.
 *
 * @param module Reference to Module
 */
UNREAL_DLL UnrealModule::Result unrInit(UnrealModule* mptr)
{
	handler = new UnrealCH_whowas(mptr);
	return UnrealModule::Success;
}

/**
 * Module close function.
 * It's called before the Module is unloaded.
 */
UNREAL_DLL UnrealModule::Result unrClose(UnrealModule* mptr)
{
	delete handler;
	return UnrealModule::Success;
}
