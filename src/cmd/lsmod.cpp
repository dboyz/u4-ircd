/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         lsmod.cpp
 * Description  LSMOD command handler
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
	"LSMOD command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * LSMOD command handler for User connections.
 *
 * LSMOD lists all loaded modules that are loaded with the server.
 *
 * Usage:
 * LSMOD
 *
 * Message example:
 * RMMOD
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_lsmod(UnrealUser* uptr, StringList* argv)
{
	bool is_oper_only = !unreal->config.get("Features/UserLsmod", "false").toBool();

	/* first, look if the command is available to operators only */
	if (is_oper_only && !uptr->isOper())
	{
		uptr->sendreply(ERR_NOPRIVILEGES, MSG_NOPRIVILEGES);
		return;
	}
	else
	{
		uptr->sendreply(CMD_NOTICE,
			String::format(MSG_LSMODSTART,
				unreal->modules.size()));

		for (List<UnrealModule*>::Iterator mi = unreal->modules.begin();
				mi != unreal->modules.end(); mi++)
		{
			UnrealModule* mptr = *mi;

			uptr->sendreply(CMD_NOTICE,
				String::format(MSG_LSMOD,
					mptr->fileName().c_str(),
					mptr->info.name.c_str(),
					mptr->info.version.c_str(),
					mptr->info.author.c_str()));
		}

		uptr->sendreply(CMD_NOTICE, MSG_LSMODEND);
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
	uc = new UnrealUserCommand(CMD_LSMOD, &uc_lsmod);

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
