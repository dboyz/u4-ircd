/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         user.cpp
 * Description  USER command handler
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
	"USER command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * USER command handler for User connections.
 *
 * Message example:
 * USER commx "commx" "commx.ws" :da real world
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_user(UnrealUser* uptr, StringList* argv)
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
			/* hyperion-style ident prefix */
			String ident = "u=" + argv->at(1);
			uptr->setIdent(ident);
		}

		size_t userlen = unreal->config.get("Limits/Userlen", "10").toUInt();

		/* trim the ident if it's too long */
		if (uptr->ident().length() > userlen)
		{
			String tmp = uptr->ident();
			uptr->setIdent(tmp.left(userlen));
		}

		uptr->setRealname(argv->at(argv->size() - 1));
		uptr->authflags().revoke(UnrealUser::AFUser);

		if (uptr->authflags().value() == 0)
			uptr->sendPing();
	}
}

/**
 * Module initialization function.
 * Called when the Module is loaded.
 *
 * @param module Reference to Module
 */
UNREAL_DLL UnrealModule::Result _init(UnrealModule& module)
{
	/* update module info */
	module.info = modinf;

	/* register command */
	uc = new UnrealUserCommand(CMD_USER, &uc_user);

	return UnrealModule::Success;
}

/**
 * Module close function.
 * It's called before the Module is unloaded.
 */
UNREAL_DLL UnrealModule::Result _close(UnrealModule& module)
{
	delete uc;
	return UnrealModule::Success;
}
