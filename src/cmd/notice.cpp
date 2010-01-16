/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         notice.cpp
 * Description  NOTICE command handler
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

#include <cmd/notice.hpp>

/** class instance */
UnrealCH_notice* handler = NULL;

/**
 * Unreal Command Handler for "NOTICE" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_notice::UnrealCH_notice(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_NOTICE, &UnrealCH_notice::exec);
}

/**
 * Unreal Command Handler for "NOTICE" - Destructor.
 */
UnrealCH_notice::~UnrealCH_notice()
{
	delete command_;
}

/**
 * NOTICE command handler for User connections.
 *
 * Usage:
 * NOTICE <target> :<message>
 *
 * Message example:
 * NOTICE #channel :hi world!
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_notice::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_NOTICE));
	}
	else if (argv->at(2).length() > 0)
	{
		String target = argv->at(1);

		if (target.at(0) == '#' || target.at(0) == '&')
		{
			UnrealChannel* chptr = UnrealChannel::find(target);

			if (!chptr)
				return; /* ignore the message */
			else
			{
				if (!chptr->canSend(uptr, argv->at(2)) || uptr->isDeaf())
					return; /* can't send, so ignore it */
				else
					chptr->sendlocalreply(uptr, CMD_NOTICE,
						String::format(":%s",
							argv->at(2).c_str()),
						true);
			}
		}
		else
		{
			UnrealUser* tuptr = UnrealUser::find(target);

			if (tuptr)
			{
				uptr->sendreply(tuptr, CMD_NOTICE,
					String::format(":%s",
						argv->at(2).c_str()));
			}
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_notice::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /NOTICE command");
	inf->setName("UnrealCH_notice");
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
	handler = new UnrealCH_notice(mptr);
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
