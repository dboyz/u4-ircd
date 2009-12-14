/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         part.cpp
 * Description  PART command handler
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
	"PART command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * PART command handler for User connections.
 *
 * Note: RFC 1459 does not define a PART message.
 *
 * Usage:
 * PART <#channel[,#channel2[,...]]> [:<message>]
 *
 * Message example:
 * PART #dev :message here
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_part(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_PART));
	}
	else
	{
		StringList cl;
		String msg;

		if (argv->size() > 2)
			msg = argv->at(2);

		if (argv->at(1).contains(","))
			cl = argv->at(1).split(",");
		else
			cl << argv->at(1);

		for (StringList::Iterator chan = cl.begin(); chan != cl.end(); chan++)
		{
			String tmp_chan = *chan;

			if (tmp_chan.at(0) != '#')
				tmp_chan.prepend("#");

			uptr->leaveChannel(tmp_chan, msg, CMD_PART);
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
	uc = new UnrealUserCommand(CMD_PART, &uc_part);

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