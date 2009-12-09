/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         nick.cpp
 * Description  NICK command handler
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
#include <iostream>

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"NICK command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * Checks whether the specified string represents a valid nick name.
 *
 * @return true when valid, otherwise false
 */
bool is_valid_nick(const String& str)
{
	String allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234"
					 "56789-_|^()[]{}\\`´'";

	String trstr = str;
	trstr = trstr.trimmed();

	if (trstr.length() > _U4_NICKLEN_)
		return false;

	for (size_t i = 0; i < trstr.length(); i++)
		if (allowed.find(trstr[i]) == String::npos)
			return false;

	return true;
}

/**
 * NICK command handler for User connections.
 *
 * Message example:
 * NICK newnick
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_nick(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
				String::format(MSG_NEEDMOREPARAMS,
						CMD_NICK));
	}
	else if (UnrealUser::find(argv->at(1)) != 0)
	{
		uptr->sendreply(ERR_NICKNAMEINUSE,
				String::format(MSG_NICKNAMEINUSE,
						argv->at(1).c_str()));
	}
	else if (!is_valid_nick(argv->at(1)))
	{
		uptr->sendreply(ERR_INVALIDNICK,
				String::format(MSG_INVALIDNICK,
						argv->at(1).c_str()));
	}
	/* handle this when not fully registered yet */
	else if (uptr->authflags().isset(UnrealUser::AFNick))
	{
		if (uptr->nick().empty())
			uptr->setNick(argv->at(1));

		uptr->authflags().revoke(UnrealUser::AFNick);

		if (uptr->authflags().value() == 0)
			uptr->sendPing();
	}
	else
	{
		uptr->sendlocalreply(CMD_NICK,
				String::format(":%s",
						argv->at(1).c_str()));

		uptr->setNick(argv->at(1));

		/* send this nick change to all users on common channels */
		if (uptr->channels.size() > 0)
		{
			for (List<UnrealChannel*>::Iterator uci = uptr->channels.begin();
					uci != uptr->channels.end(); uci++)
			{
				UnrealChannel* chptr = *uci;

				chptr->sendlocalreply(uptr, CMD_NICK,
						String::format(":%s",
							argv->at(1).c_str()),
						true);
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
	uc = new UnrealUserCommand(CMD_NICK, &uc_nick);

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
