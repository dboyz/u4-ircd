/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         help.cpp
 * Description  HELP command handler
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

#include <cmd/help.hpp>
#include <cmd/notice.hpp>

/** class instance */
static UnrealCH_help* handler = NULL;

/**
 * Unreal Command Handler for "HELP" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_help::UnrealCH_help(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_HELP, &UnrealCH_help::exec);
}

/**
 * Unreal Command Handler for "HELP" - Destructor.
 */
UnrealCH_help::~UnrealCH_help()
{
	delete command_;
}

/**
 * HELP command handler for User connections.
 *
 * Usage:
 * HELP [<target>]
 *
 * Message example:
 * HELP
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_help::exec(UnrealUser* uptr, StringList* argv)
{
	String target = unreal->me->name();

	if (argv->size() >= 2)
		target = argv->at(1);

	if (target == unreal->me->name())
	{
		/* print all commands available on this server */
		Map<String, UnrealUserCommand*>::Iterator cmd_iter;

		for (cmd_iter = unreal->user_commands.begin();
				cmd_iter != unreal->user_commands.end(); ++cmd_iter)
		{
			uptr->sendreply(CMD_NOTICE, (cmd_iter->first).c_str());
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_help::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /HELP command");
	inf->setName("UnrealCH_help");
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
	handler = new UnrealCH_help(mptr);
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
