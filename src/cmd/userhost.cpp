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

#include "base.hpp"
#include "cmdmap.hpp"
#include "command.hpp"
#include "module.hpp"
#include "stringlist.hpp"

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"USERHOST command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * USERHOST command handler for User connections.
 *
 * Message example:
 * USERHOST commx Stealth ohnobinki
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_userhost(UnrealUser* uptr, StringList* argv)
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
				sli++)
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
	uc = new UnrealUserCommand(CMD_USERHOST, &uc_userhost);

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
