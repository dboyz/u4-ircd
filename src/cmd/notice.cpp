/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         notice.cpp
 * Description  NOTICE command handler
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
	"NOTICE command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * NOTICE command handler for User connections.
 *
 * Note: As RFC 1459 says, the difference between NOTICE and PRIVMSG
 * is that NOTICE never sends errors back to the sender.
 *
 * Usage:
 * NOTICE <target> :<message>
 *
 * Message example:
 * NOTICE #channel :hi world!
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_notice(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_NOTICE));
	}
	else if (argv->at(2).length() > 0)
	{
		String target = argv->at(1);

		if (target.at(0) == '#')
		{
			// TODO: channel notice handling
		}
		else
		{
			UnrealUser* tuptr = UnrealUser::find(target);

			if (!tuptr)
			{
				uptr->sendreply(ERR_NOSUCHNICK,
					String::format(MSG_NOSUCHNICK,
						target.c_str()));
			}
			else
			{
				uptr->sendreply(tuptr, CMD_NOTICE,
					String::format(":%s",
						argv->at(2).c_str()));
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
	uc = new UnrealUserCommand(CMD_NOTICE, &uc_notice);

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
