/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         invite.cpp
 * Description  INVITE command handler
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
	"INVITE command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * INVITE command handler for User connections.
 *
 * Usage:
 * INVITE <nick> <channel>
 *
 * Message example:
 * INVITE binki #test
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_invite(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_INVITE));
		return;
	}

	UnrealUser *tuptr = UnrealUser::find(argv->at(1));
	UnrealChannel *chptr = UnrealChannel::find(argv->at(2));
	UnrealChannel::Member* cmptr;

	/* check for channel existance */
	if (!chptr)
	{
		uptr->sendreply(ERR_NOSUCHCHANNEL,
			String::format(MSG_NOSUCHCHANNEL,
				argv->at(2).c_str()));
	}

	/* invites just from users on the channel */
	else if (!(cmptr = chptr->findMember(uptr)))
	{
		chptr->sendreply(uptr, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL);
	}

	/* requester needs chanop */
	else if (!cmptr->isChanOp())
	{
		chptr->sendreply(uptr, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED);
	}

	/* check if inviting user is online */
	else if (!tuptr)
	{
		uptr->sendreply(ERR_NOSUCHNICK,
			String::format(MSG_NOSUCHNICK,
				argv->at(1).c_str()));
	}

	/* check if inviting user is already on channel */
	else if (chptr->findMember(tuptr))
	{
		uptr->sendreply(ERR_USERONCHANNEL,
			String::format(MSG_USERONCHANNEL,
				tuptr->nick().c_str(),
				chptr->name().c_str()));
	}
	else if (!chptr->invites.contains(tuptr->lowerNick()))
	{
		/* send invite notification to user we invite */
		uptr->sendreply(tuptr, CMD_INVITE, chptr->name());

		/* send notifcation to the requesting user */
		chptr->sendreply(uptr, RPL_INVITING, tuptr->nick());

		/* add to active invites */
		chptr->invites << tuptr->lowerNick();
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
	uc = new UnrealUserCommand(CMD_INVITE, &uc_invite);

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
