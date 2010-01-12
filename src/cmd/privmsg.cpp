/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         privmsg.cpp
 * Description  PRIVMSG command handler
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
	"PRIVMSG command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * PRIVMSG command handler for User connections.
 *
 * Usage:
 * PRIVMSG <target[,target2[,...]]> :<message>
 *
 * Message example:
 * PRIVMSG #channel1,#channel2 :You're not alone.
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_privmsg(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_PRIVMSG));
	}
	else if (argv->at(2).length() == 0)
	{
		uptr->sendreply(ERR_NOTEXTTOSEND, MSG_NOTEXTTOSEND);
	}
	else
	{
		StringList targets = argv->at(1).split(",");

		if (targets.size() == 0)
			targets << argv->at(1);

		for (StringList::Iterator sli = targets.begin(); sli != targets.end();
				++sli)
		{
			String& target = *sli;

			if (target.at(0) == '#' || target.at(0) == '&')
			{
				UnrealChannel* chptr = UnrealChannel::find(target);

				if (!chptr)
				{
					uptr->sendreply(ERR_NOSUCHCHANNEL,
						String::format(MSG_NOSUCHCHANNEL,
							target.c_str()));
				}
				else
				{
					if (!chptr->canSend(uptr, argv->at(2)))
						uptr->sendreply(ERR_CANNOTSENDTOCHAN,
							String::format(MSG_CANNOTSENDTOCHAN,
								chptr->name().c_str()));
					else if (uptr->isDeaf())
						return;
					else
						chptr->sendlocalreply(uptr, CMD_PRIVMSG,
							String::format(":%s",
								argv->at(2).c_str()),
							true);
				}
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
					uptr->sendreply(tuptr, CMD_PRIVMSG,
						String::format(":%s",
							argv->at(2).c_str()));
				}
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
	uc = new UnrealUserCommand(CMD_PRIVMSG, &uc_privmsg);

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
