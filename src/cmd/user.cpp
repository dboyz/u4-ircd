/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         user.cpp
 * Description  USER command handler
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
#include <limits.hpp>
#include <stringlist.hpp>

#include <cmd/user.hpp>

/** class instance */
static UnrealCH_user* handler = NULL;

/**
 * Unreal Command Handler for "USER" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_user::UnrealCH_user(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_USER, &UnrealCH_user::exec,
		false, false);
}

/**
 * Unreal Command Handler for "USER" - Destructor.
 */
UnrealCH_user::~UnrealCH_user()
{
	delete command_;
}

/**
 * USER command handler for User connections.
 *
 * Message example:
 * USER commx "commx" "commx.ws" :da real world
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_user::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 5)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
				String::format(MSG_NEEDMOREPARAMS,
						CMD_USER));
	}
	else if (uptr->authflags().isset(UnrealUser::AFUser))
	{
		if (uptr->ident().empty())
		{
			uptr->setIdent("~" + argv->at(1));
		}

		/* trim the ident if it's too long */
		if (uptr->ident().length() > U4_USERLEN)
		{
			String tmp = uptr->ident();
			uptr->setIdent(tmp.left(U4_USERLEN));
		}

		uptr->setRealname(argv->at(argv->size() - 1));
		uptr->authflags().revoke(UnrealUser::AFUser);

		if (uptr->authflags().value() == 0)
			uptr->sendPing();
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_user::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /USER command");
	inf->setName("UnrealCH_user");
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
	handler = new UnrealCH_user(mptr);
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
