/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         kill.cpp
 * Description  KILL command handler
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

#include <cmd/kill.hpp>

/** class instance */
UnrealCH_kill* handler = NULL;

/**
 * Unreal Command Handler for "KILL" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_kill::UnrealCH_kill(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_KILL, &UnrealCH_kill::exec, true);
}

/**
 * Unreal Command Handler for "KILL" - Destructor.
 */
UnrealCH_kill::~UnrealCH_kill()
{
	delete command_;
}

/**
 * KILL command handler for User connections.
 *
 * Usage:
 * KILL <nickname> <comment>
 *
 * Message example:
 * KILL abuser Use another server
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_kill::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_KILL));
	}

	UnrealUser* victim = UnrealUser::find(argv->at(1));
	
	if (!victim)
	{
		uptr->sendreply(ERR_NOSUCHNICK,
			String::format(MSG_NOSUCHNICK,
				argv->at(1).c_str()));
	}
	else
	{
		String message = "Killed (" + argv->at(2) + ")";
		UnrealSocket::ErrorCode ec;

		/* pass quit message to all channels the user is on */
		victim->exit(ec, message);

		/* close connection with client */
		victim->exit(message);
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_kill::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /KILL command");
	inf->setName("UnrealCH_kill");
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
	handler = new UnrealCH_kill(mptr);
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
