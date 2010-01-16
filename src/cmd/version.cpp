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

#include <base.hpp>
#include <command.hpp>
#include <stringlist.hpp>

#include <cmd/version.hpp>

/** class instance */
UnrealCH_version* handler = NULL;

/**
 * Unreal Command Handler for "VERSION" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_version::UnrealCH_version(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_VERSION, &UnrealCH_version::exec);
}

/**
 * Unreal Command Handler for "VERSION" - Destructor.
 */
UnrealCH_version::~UnrealCH_version()
{
	delete command_;
}

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
void UnrealCH_version::exec(UnrealUser* uptr, StringList* argv)
{
	String target = unreal->me->name();

	if (argv->size() >= 2)
		target = argv->at(1);

	if (target == unreal->me->name())
	{
		uptr->sendreply(RPL_VERSION,
			String::format(MSG_VERSION,
				PACKAGE_VERSTR,
				PACKAGE_PATCHLEVEL,
				unreal->me->name().c_str(),
				UnrealVersion::package_changeset.c_str() ));
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_version::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /VERSION command");
	inf->setName("UnrealCH_version");
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
	handler = new UnrealCH_version(mptr);
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
