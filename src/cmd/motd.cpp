/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         motd.cpp
 * Description  MOTD command handler
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
	"MOTD command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * MOTD command handler for User connections.
 *
 * Usage:
 * MOTD [<target>]
 *
 * Message example:
 * MOTD
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_motd(UnrealUser* uptr, StringList* argv)
{
	String target;

	if (argv && argv->size() >= 2)
		target = argv->at(1);

	String motd_file = unreal->config.get("Me/MOTD");
	String tmp;

	if (!motd_file.empty())
	{
		std::ifstream is(motd_file.c_str());

		if (is.good())
		{
			uptr->sendreply(RPL_MOTDSTART,
				String::format(MSG_MOTDSTART,
					unreal->config.get("Me/ServerName").c_str()));

			while (is.good())
			{
				std::getline(is, tmp);

				uptr->sendreply(RPL_MOTD,
					String::format(MSG_MOTD,
						tmp.c_str()));
			}

			uptr->sendreply(RPL_ENDOFMOTD, MSG_ENDOFMOTD);

			return;
		}
	}

	uptr->sendreply(ERR_NOMOTD, MSG_NOMOTD);
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
	uc = new UnrealUserCommand(CMD_MOTD, &uc_motd);

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
