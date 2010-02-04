/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         userhost.cpp
 * Description  USERHOST command handler
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

#include <cmd/userhost.hpp>

/** class instance */
static UnrealCH_userhost* handler = NULL;

/**
 * Unreal Command Handler for "USERHOST" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_userhost::UnrealCH_userhost(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_USERHOST, &UnrealCH_userhost::exec);
}

/**
 * Unreal Command Handler for "USERHOST" - Destructor.
 */
UnrealCH_userhost::~UnrealCH_userhost()
{
	delete command_;
}

/**
 * USERHOST command handler for User connections.
 *
 * Message example:
 * USERHOST commx Stealth ohnobinki
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_userhost::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_USERHOST));
	}
	else
	{
		for (StringList::Iterator sli = argv->begin() + 1; sli != argv->end();
				++sli)
		{
			UnrealUser* tuptr = UnrealUser::find(*sli);

			if (!tuptr)
			{
				uptr->sendreply(ERR_NOSUCHNICK,
					String::format(MSG_NOSUCHNICK,
						(*sli).c_str()));
			}
			else
			{
				uptr->sendreply(RPL_USERHOST,
					String::format(MSG_USERHOST,
						tuptr->nick().c_str(),
						tuptr->isOper() ? "*" : "",
						tuptr->isAway() ? "-" : "+",
						tuptr->ident().c_str(),
						tuptr->hostname().c_str()));
			}
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_userhost::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /USERHOST command");
	inf->setName("UnrealCH_userhost");
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
	handler = new UnrealCH_userhost(mptr);
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
