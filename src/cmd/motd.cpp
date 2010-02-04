/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         motd.cpp
 * Description  MOTD command handler
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
#include <module.hpp>
#include <stringlist.hpp>

#include <cmd/motd.hpp>

/** class instance */
static UnrealCH_motd* handler = NULL;

/**
 * Unreal Command Handler for "MOTD" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_motd::UnrealCH_motd(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_MOTD, &UnrealCH_motd::exec);
}

/**
 * Unreal Command Handler for "MOTD" - Destructor.
 */
UnrealCH_motd::~UnrealCH_motd()
{
	delete command_;
}

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
void UnrealCH_motd::exec(UnrealUser* uptr, StringList* argv)
{
	String target;

	if (argv && argv->size() >= 2)
		target = argv->at(1);

	String motd_file = unreal->config.get("Me::MOTD");
	String tmp;

	if (!motd_file.empty())
	{
		std::ifstream is(motd_file.c_str());

		if (is.good())
		{
			uptr->sendreply(RPL_MOTDSTART,
				String::format(MSG_MOTDSTART,
					unreal->me->name().c_str()));

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
	else
		uptr->sendreply(ERR_NOMOTD, MSG_NOMOTD);
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_motd::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /MOTD command");
	inf->setName("UnrealCH_motd");
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
	handler = new UnrealCH_motd(mptr);
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
