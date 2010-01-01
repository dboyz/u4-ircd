/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         kick.cpp
 * Description  KICK command handler
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
	"KICK command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * KICK command handler for User connections.
 *
 * Usage:
 * KICK <channel> <nick> [:<comment>]
 *
 * Message example:
 * KICK #test WiZ :get out of here
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_kick(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_KICK));
	}
	else
	{
		UnrealChannel* chptr = UnrealChannel::find(argv->at(1));
		UnrealChannel::Member* cmptr = 0;
		UnrealUser* tuptr = UnrealUser::find(argv->at(2));

		if (!chptr)
		{
			uptr->sendreply(ERR_NOSUCHCHANNEL,
				String::format(MSG_NOSUCHCHANNEL,
					argv->at(1).c_str()));
		}
		else if (!tuptr)
		{
			uptr->sendreply(ERR_NOSUCHNICK,
				String::format(MSG_NOSUCHNICK,
					argv->at(2).c_str()));
		}
		else if (!(cmptr = chptr->findMember(uptr)))
		{
			chptr->sendreply(uptr, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL);
		}
		else if (!cmptr->isChanOp() && !uptr->isOper())
		{
			chptr->sendreply(uptr, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED);
		}
		else if (!(cmptr = chptr->findMember(tuptr)))
		{
			chptr->sendreply(uptr, ERR_USERNOTINCHANNEL,
				String::format(MSG_USERNOTINCHANNEL,
					tuptr->nick().c_str()));
		}
		else
		{
			String message;

			if (argv->size() > 3)
				message = argv->at(3);

			chptr->sendlocalreply(uptr, CMD_KICK,
				String::format("%s :%s",
					tuptr->nick().c_str(),
					message.c_str()));

			tuptr->leaveChannel(chptr->name(), message, CMD_KICK);
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
	uc = new UnrealUserCommand(CMD_KICK, &uc_kick);

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
