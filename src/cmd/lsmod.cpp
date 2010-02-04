/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         lsmod.cpp
 * Description  LSMOD command handler
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

#include <cmd/lsmod.hpp>
#include <cmd/notice.hpp>

/** class instance */
static UnrealCH_lsmod* handler = NULL;

/**
 * Unreal Command Handler for "LSMOD" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_lsmod::UnrealCH_lsmod(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_LSMOD, &UnrealCH_lsmod::exec);
}

/**
 * Unreal Command Handler for "LSMOD" - Destructor.
 */
UnrealCH_lsmod::~UnrealCH_lsmod()
{
	delete command_;
}

/**
 * LSMOD command handler for User connections.
 *
 * LSMOD lists all loaded modules that are loaded with the server.
 *
 * Usage:
 * LSMOD [<server>]
 *
 * Message example:
 * LSMOD
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_lsmod::exec(UnrealUser* uptr, StringList* argv)
{
	bool is_oper_only = !unreal->config.get("Features::UserLsmod", "false")
		.toBool();

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
				mi != unreal->modules.end(); ++mi)
		{
			UnrealModule* mptr = *mi;

			uptr->sendreply(CMD_NOTICE,
				String::format(MSG_LSMOD,
					mptr->fileName().c_str(),
					mptr->inf.name().c_str(),
					mptr->inf.version().c_str(),
					mptr->inf.author().c_str()));
		}

		uptr->sendreply(CMD_NOTICE, MSG_LSMODEND);
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_lsmod::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /LSMOD command");
	inf->setName("UnrealCH_lsmod");
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
	handler = new UnrealCH_lsmod(mptr);
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
