/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         restart.cpp
 * Description  RESTART command handler
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
#include <hash.hpp>
#include <stringlist.hpp>

#include <cmd/restart.hpp>

/** class instance */
static UnrealCH_restart* handler = NULL;

/**
 * Unreal Command Handler for "RESTART" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_restart::UnrealCH_restart(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_RESTART, &UnrealCH_restart::exec);
}

/**
 * Unreal Command Handler for "RESTART" - Destructor.
 */
UnrealCH_restart::~UnrealCH_restart()
{
	delete command_;
}

/**
 * Check the restart password.
 *
 * @param pw Plain password to check
 * @return true when the password is correct, otherwise false
 */
bool UnrealCH_restart::checkPassword(const String& pw, String& password)
{
	UnrealHash::Type hash_type = UnrealHash::Invalid;
	size_t pos_s, pos_e;

	if ((pos_s = password.find_first_of("$")) != String::npos)
	{
		if ((pos_e = password.find_first_of("$", pos_s + 1))
				!= String::npos)
		{
			String typestr = password.mid(pos_s + 1, (pos_e - pos_s - 1));

			if ((hash_type = UnrealHash::strtotype(typestr))
					!= UnrealHash::Invalid)
			{
				String real_pw = password.mid(pos_e + 1);
				String hash = UnrealHash::calculate(pw, hash_type);

				return (hash.toUpper() == real_pw.toUpper());
			}
		}
	}

	return password == pw;
}

/**
 * RESTART command handler for User connections.
 *
 * Usage:
 * RESTART <password>
 *
 * Message example:
 * RESTART secret
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_restart::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_RESTART));
	}
	else
	{
		String restart_password = unreal->config.get("Me::RestartPassword");

		/* check for valid password */
		if (UnrealCH_restart::checkPassword(argv->at(1), restart_password))
		{
			unreal->restart();
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_restart::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /RESTART command");
	inf->setName("UnrealCH_restart");
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
	handler = new UnrealCH_restart(mptr);
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
