/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         ping.cpp
 * Description  PING command handler
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

#include <cmd/ping.hpp>
#include <cmd/pong.hpp>

/** class instance */
UnrealCH_ping* handler = NULL;

/**
 * Unreal Command Handler for "PING" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_ping::UnrealCH_ping(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_PING, &UnrealCH_ping::exec);
}

/**
 * Unreal Command Handler for "PING" - Destructor.
 */
UnrealCH_ping::~UnrealCH_ping()
{
	delete command_;
}

/**
 * PING command handler for User connections.
 *
 * Usage:
 * PING <server1> [<server2>]
 *
 * Message example:
 * PING server.name
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_ping::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
				String::format(MSG_NEEDMOREPARAMS,
						CMD_PING));
		return;
	}

	// TODO: if <server2> is set, send that PING request to that server

	uptr->sendreply(CMD_PONG,
		String::format(":%s", argv->at(1).c_str()));
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_ping::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /PING command");
	inf->setName("UnrealCH_ping");
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
	handler = new UnrealCH_ping(mptr);
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
