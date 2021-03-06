/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         admin.cpp
 * Description  ADMIN command handler
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

#include <cmd/admin.hpp>

/** class instance */
static UnrealCH_admin* handler = NULL;

/**
 * Unreal Command Handler for "ADMIN" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_admin::UnrealCH_admin(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_ADMIN, &UnrealCH_admin::exec);
}

/**
 * Unreal Command Handler for "ADMIN" - Destructor.
 */
UnrealCH_admin::~UnrealCH_admin()
{
	delete command_;
}

/**
 * ADMIN command handler for User connections.
 *
 * Usage:
 * ADMIN [<server>]
 *
 * Message example:
 * ADMIN playground.commx.ws
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_admin::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() > 1)
	{
		//TODO: specify server name
	}
	else if (unreal->config.get("Admin::Location", "").empty())
	{
		uptr->sendreply(ERR_NOADMININFO,
			String::format(MSG_NOADMININFO,
				unreal->me->name().c_str()));
	}
	else
	{
		String buf;

		uptr->sendreply(RPL_ADMINME, String::format(MSG_ADMINME,
			unreal->me->name().c_str()));

		buf = ":" + unreal->config.get("Admin::Location");
		uptr->sendreply(RPL_ADMINLOC1, buf);

		buf = ":" + unreal->config.get("Admin::Description");
		uptr->sendreply(RPL_ADMINLOC2, buf);

		buf = ":" + unreal->config.get("Admin::Contact");
		uptr->sendreply(RPL_ADMINEMAIL, buf);
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_admin::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /ADMIN command");
	inf->setName("UnrealCH_admin");
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
	handler = new UnrealCH_admin(mptr);
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
