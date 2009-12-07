/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         rmmod.cpp
 * Description  RMMOD command handler
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
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
#include "cmdmap.hpp"
#include "command.hpp"
#include "module.hpp"
#include "stringlist.hpp"

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"RMMOD command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

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
void uc_rmmod(UnrealUser* uptr, StringList* argv)
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
					mptr->info.name.c_str()));

			/* take off the module list */
			unreal->modules.remove(mptr);

			delete mptr;
		}
	}
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
	uc = new UnrealUserCommand(CMD_RMMOD, &uc_rmmod, true);

	return UnrealModule::Success;
}

/**
 * Module close function.
 * It's called before the Module is unloaded.
 */
UNREAL_DLL UnrealModule::Result unrClose(UnrealModule& module)
{
	delete uc;

	return UnrealModule::Success;
}
