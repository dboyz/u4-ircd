/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         insmod.cpp
 * Description  INSMOD command handler
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
	"INSMOD command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * INSMOD command handler for User connections.
 *
 * INSMOD allows IRC operators to load modules dynamically while
 * the server is running.
 *
 * Usage:
 * INSMOD <filename>
 *
 * Message example:
 * INSMOD /home/chris/lib/cmd/help.so
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_insmod(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_INSMOD));
	}
	else
	{
		UnrealModule* mptr = new UnrealModule(argv->at(1));

		if (!mptr->isLoaded())
		{
			String errStr = mptr->errorString();

			uptr->sendreply(CMD_NOTICE,
				String::format(MSG_INSMODFAILED,
					errStr.c_str()));

			unreal->log.write(UnrealLog::Normal, "Warning: Loading module "
					"failed: %s (INSMOD from %s)",
					errStr.c_str(),
					uptr->nick().c_str());

			delete mptr;
		}
		else
		{
			uptr->sendreply(CMD_NOTICE,
				String::format(MSG_INSMODOK,
					mptr->info.name.c_str(),
					mptr->info.version.c_str()));

			unreal->log.write(UnrealLog::Debug, "Loading Module \"%s\" "
				"(INSMOD from %s)",
				mptr->fileName().c_str(),
				uptr->nick().c_str());

			/* add to module list */
			unreal->modules << mptr;
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
	uc = new UnrealUserCommand(CMD_INSMOD, &uc_insmod, true);

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
