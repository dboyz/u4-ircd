/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         nick.cpp
 * Description  NICK command handler
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

#include <cmd/nick.hpp>

/** class instance */
static UnrealCH_nick* handler = NULL;

/** allowed characters for nicks */
const String allowed_nick_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789-_|^()[]{}\\`´'";

/**
 * Unreal Command Handler for "NICK" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_nick::UnrealCH_nick(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_NICK, &UnrealCH_nick::exec,
		false, false);
}

/**
 * Unreal Command Handler for "NICK" - Destructor.
 */
UnrealCH_nick::~UnrealCH_nick()
{
	delete command_;
}

/**
 * Checks whether the specified string represents a valid nick name.
 *
 * @return true when valid, otherwise false
 */
bool UnrealCH_nick::isValidNick(const String& str)
{
	static String allowed("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"0123456789-_|^()[]{}\\`´'");

	String trstr = str;

	trstr = trstr.trimmed();

	if (trstr.length() > U4_NICKLEN)
		return false;

	for (String::Iterator ch = trstr.begin(); ch != trstr.end(); ++ch)
		if (allowed.find(*ch) == String::npos)
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
void UnrealCH_nick::exec(UnrealUser* uptr, StringList* argv)
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
	else if (!UnrealCH_nick::isValidNick(argv->at(1)))
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
					uci != uptr->channels.end(); ++uci)
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
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_nick::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /NICK command");
	inf->setName("UnrealCH_nick");
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
	handler = new UnrealCH_nick(mptr);
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
