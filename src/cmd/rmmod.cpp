/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         rmmod.cpp
 * Description  RMMOD command handler
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

#include <cmd/notice.hpp>
#include <cmd/rmmod.hpp>

/** class instance */
UnrealCH_rmmod* handler = NULL;

/**
 * Unreal Command Handler for "RMMOD" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_rmmod::UnrealCH_rmmod(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_RMMOD, &UnrealCH_rmmod::exec, true);
}

/**
 * Unreal Command Handler for "RMMOD" - Destructor.
 */
UnrealCH_rmmod::~UnrealCH_rmmod()
{
	delete command_;
}

/**
 * RMMOD command handler for User connections.
 *
 * RMMOD allows IRC operators to unload modules dynamically while
 * the server is running.
 *
 * Usage:
 * RMMOD <filename>
 *
 * Message example:
 * RMMOD /home/chris/lib/cmd/help.so
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_rmmod::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_RMMOD));
	}
	else
	{
		UnrealModule* mptr = UnrealModule::find(argv->at(1));

		if (!mptr)
		{
			uptr->sendreply(CMD_NOTICE,
				String::format(MSG_RMMODNOTFOUND,
					argv->at(1).c_str()));
		}
		else
		{
			uptr->sendreply(CMD_NOTICE,
				String::format(MSG_RMMODOK,
					mptr->inf.name().c_str()));

			/* take off the module list */
			unreal->modules.remove(mptr);

			delete mptr;
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_rmmod::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /RMMOD command");
	inf->setName("UnrealCH_rmmod");
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
	handler = new UnrealCH_rmmod(mptr);
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
