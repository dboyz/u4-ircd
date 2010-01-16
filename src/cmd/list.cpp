/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         list.cpp
 * Description  LIST command handler
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
#include <module.hpp>
#include <stringlist.hpp>

#include <cmd/list.hpp>

/** class instance */
UnrealCH_list* handler = NULL;

/**
 * Unreal Command Handler for "LIST" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_list::UnrealCH_list(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_LIST, &UnrealCH_list::exec);
}

/**
 * Unreal Command Handler for "LIST" - Destructor.
 */
UnrealCH_list::~UnrealCH_list()
{
	delete command_;
}

/**
 * LIST command handler for User connections.
 *
 * Usage:
 * LIST [<param>]
 *
 * Message example:
 * LIST >100		; Lists all channels with at more than 100 users in it
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_list::exec(UnrealUser* uptr, StringList* argv)
{
	/*
	 * RFC1459/RFC2812 do not provide any details about additional parameters
	 * on the LIST command. A few IRC clients use >123 or <123 to specify that
	 * they just want to get a list of channels with more or less than the
	 * specified amount of users in the channel.
	 */
	uint32_t min_users = 0, max_users = 0;

	if (argv->size() >= 2)
	{
		String& arg = argv->at(1);

		if (!arg.empty() && arg.at(0) == '<')
			min_users = arg.mid(1).toUInt();
		else if (!arg.empty() && arg.at(0) == '>')
			max_users = arg.mid(1).toUInt();
	}

	uptr->sendreply(RPL_LISTSTART, MSG_LISTSTART);

	for (Map<String, UnrealChannel*>::Iterator ci = unreal->channels.begin();
			ci != unreal->channels.end(); ++ci)
	{
		UnrealChannel* chptr = ci->second;

		if (chptr->isPrivate() || chptr->isSecret())
			continue; /* ignore these */
		else if ((min_users > 0 && chptr->members.size() > min_users)
				|| (max_users > 0 && chptr->members.size() < max_users))
			continue; /* gently ignore channels that don't fit */

		uptr->sendreply(RPL_LIST,
			String::format(MSG_LIST,
				chptr->name().c_str(),
				chptr->members.size(),
				chptr->modestr().c_str(),
				chptr->topic().c_str()));
	}

	uptr->sendreply(RPL_LISTEND, MSG_LISTEND);
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_list::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /LIST command");
	inf->setName("UnrealCH_list");
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
	handler = new UnrealCH_list(mptr);
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
