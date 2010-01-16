/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         rehash.cpp
 * Description  REHASH command handler
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

#include <cmd/rehash.hpp>

/** class instance */
UnrealCH_rehash* handler = NULL;

/**
 * Unreal Command Handler for "REHASH" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_rehash::UnrealCH_rehash(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_REHASH, &UnrealCH_rehash::exec);
}

/**
 * Unreal Command Handler for "REHASH" - Destructor.
 */
UnrealCH_rehash::~UnrealCH_rehash()
{
	delete command_;
}

/**
 * REHASH command handler for User connections.
 *
 * Usage:
 * REHASH
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_rehash::exec(UnrealUser* uptr, StringList* argv)
{
	unreal->config.rehash();

	uptr->sendreply(RPL_REHASHING,
		String::format(MSG_REHASHING,
			unreal->config.fileName().c_str()));
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_rehash::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /REHASH command");
	inf->setName("UnrealCH_rehash");
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
	handler = new UnrealCH_rehash(mptr);
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
