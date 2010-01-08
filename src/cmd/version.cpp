/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         version.cpp
 * Description  VERSION command handler
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
	"VERSION command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * VERSION command handler for User connections.
 *
 * Usage:
 * VERSION [<target>]
 *
 * Message example:
 * VERSION
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_version(UnrealUser* uptr, StringList* argv)
{
	String target = unreal->me.name();

	if (argv->size() >= 2)
		target = argv->at(1);

	if (target == unreal->me.name())
	{
		uptr->sendreply(RPL_VERSION,
			String::format(MSG_VERSION,
				PACKAGE_VERSTR,
				PACKAGE_PATCHLEVEL,
				unreal->me.name().c_str(),
				PACKAGE_CHANGESET));
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
	uc = new UnrealUserCommand(CMD_VERSION, &uc_version);

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
