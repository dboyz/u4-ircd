/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         away.cpp
 * Description  AWAY command handler
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
	"AWAY command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * AWAY command handler for User connections.
 *
 * Usage:
 * AWAY [:<message>]
 *
 * Message example:
 * AWAY :brb
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_away(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() == 1)
	{
		uptr->setAwayMessage("");
		uptr->sendreply(RPL_UNAWAY, MSG_UNAWAY);
	}
	else if (argv->size() >= 2)
	{
		String awayMessage = argv->at(1);
		size_t awaylen = unreal->config.get("Limits::Awaylen", "250")
			.toUInt();

		if (awayMessage.length() > awaylen)
			awayMessage = awayMessage.left(awaylen);

		/* update away message */
		uptr->setAwayMessage(awayMessage);
		uptr->sendreply(RPL_NOWAWAY,
			String::format(MSG_NOWAWAY,
				uptr->awayMessage().c_str()));
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
	uc = new UnrealUserCommand(CMD_AWAY, &uc_away);

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
