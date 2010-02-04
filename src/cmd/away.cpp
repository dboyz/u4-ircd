/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         away.cpp
 * Description  AWAY command handler
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

#include <cmd/away.hpp>

/** class instance */
static UnrealCH_away* handler = NULL;

/**
 * Unreal Command Handler for "AWAY" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_away::UnrealCH_away(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_AWAY, &UnrealCH_away::exec);
}

/**
 * Unreal Command Handler for "AWAY" - Destructor.
 */
UnrealCH_away::~UnrealCH_away()
{
	delete command_;
}

/**
 * AWAY command handler for User connections.
 *
 * Usage:
 * AWAY [:<message>]
 *
 * Message example:
 * AWAY :brb
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_away::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() == 1)
	{
		uptr->setAwayMessage("");
		uptr->sendreply(RPL_UNAWAY, MSG_UNAWAY);
	}
	else if (argv->size() >= 2)
	{
		String awayMessage = argv->at(1);
		size_t awaylen = unreal->config.get("Limits::Awaylen", "250")
			.toUInt();

		if (awayMessage.length() > awaylen)
			awayMessage = awayMessage.left(awaylen);

		/* update away message */
		uptr->setAwayMessage(awayMessage);
		uptr->sendreply(RPL_NOWAWAY,
			String::format(MSG_NOWAWAY,
				uptr->awayMessage().c_str()));
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_away::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /AWAY command");
	inf->setName("UnrealCH_away");
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
	handler = new UnrealCH_away(mptr);
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
