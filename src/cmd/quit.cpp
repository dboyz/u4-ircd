/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         quit.cpp
 * Description  QUIT command handler
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

#include <cmd/quit.hpp>

/** class instance */
UnrealCH_quit* handler = NULL;

/**
 * Unreal Command Handler for "QUIT" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_quit::UnrealCH_quit(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_QUIT, &UnrealCH_quit::exec);
}

/**
 * Unreal Command Handler for "QUIT" - Destructor.
 */
UnrealCH_quit::~UnrealCH_quit()
{
	delete command_;
}

/**
 * QUIT command handler for User connections.
 *
 * Message example:
 * QUIT :bye
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_quit::exec(UnrealUser* uptr, StringList* argv)
{
	String quitMessage = "Exiting";

	if (argv->size() > 1)
		quitMessage = argv->at(1);

	/* send quit message to all channels the user is on */
	while (uptr->channels.size() > 0)
	{
		UnrealChannel* chptr = uptr->channels.takeFirst();

		/* remove from channel */
		uptr->leaveChannel(chptr->name(), quitMessage, CMD_QUIT);
	}

	uptr->socket()->close();
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_quit::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /QUIT command");
	inf->setName("UnrealCH_quit");
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
	handler = new UnrealCH_quit(mptr);
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
