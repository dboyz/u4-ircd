/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         list.cpp
 * Description  LIST command handler
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
	"LIST command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * LIST command handler for User connections.
 *
 * Usage:
 * LIST [<channel[,channel[,...]]>] [<server>]
 *
 * Message example:
 * LIST
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_list(UnrealUser* uptr, StringList* argv)
{
	// TODO: add options. The usage above is extracted from RFC 1459.
	// Syntax like >100 for listing channels with more than hundred users
	// seem to be an extension of newer RFCs

	uptr->sendreply(RPL_LISTSTART, MSG_LISTSTART);

	for (Map<String, UnrealChannel*>::Iterator ci = unreal->channels.begin();
			ci != unreal->channels.end(); ci++)
	{
		UnrealChannel* chptr = ci->second;

		if (chptr->isPrivate() || chptr->isSecret())
			continue; /* ignore these */

		uptr->sendreply(RPL_LIST,
			String::format(MSG_LIST,
				chptr->name().c_str(),
				chptr->members.size(),
				chptr->topic().c_str()));
	}

	uptr->sendreply(RPL_LISTEND, MSG_LISTEND);
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
	uc = new UnrealUserCommand(CMD_LIST, &uc_list);

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
