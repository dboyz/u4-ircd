/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         info.cpp
 * Description  INFO command handler
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
	"INFO command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

StringList readInfoFile()
{
	StringList sl;

	sl << ":UnrealIRCd, Version 4";
	sl << ": ";
	sl << ":This program is free software; you can redistribute it and/or";
	sl << ":modify it under the terms of the GNU General Public License as";
	sl << ":published by the Free Software Foundation; either version 3, or";
	sl << ":(at your option) any later version.";
	sl << ": ";
	sl << ":UnrealIRCd 4 was written in C++ with a new, modern code base - ";
	sl << ":to make your IRC experience even better.";
	sl << ": ";
	sl << ":Thanks to all people who made this possible.";

	return sl;
}
/**
 * INFO command handler for User connections.
 *
 * Usage:
 * INFO [<target>]
 *
 * Message example:
 * INFO some.server.net
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_info(UnrealUser* uptr, StringList* argv)
{
	String target = unreal->me.name();

	if (argv->size() >= 2)
		target = argv->at(1);

	if (target == unreal->me.name())
	{
		StringList output = readInfoFile();

		output << String(": ") << String(": ");
		output << String::format(":Revision: %s", UnrealVersion::package_changeset.c_str());
		output << String::format(":Birth date: %s %s", __DATE__, __TIME__);
		output << String::format(":On-line since %s",
			unreal->me.bootTime().toString("%Y-%M-%dT%H:%M:%S %Z").c_str());

		/* now send it */
		for (StringList::Iterator i = output.begin(); i != output.end(); ++i)
			uptr->sendreply(RPL_INFO, (*i).c_str());

		uptr->sendreply(RPL_ENDOFINFO, MSG_ENDOFINFO);
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
	uc = new UnrealUserCommand(CMD_INFO, &uc_info);

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
