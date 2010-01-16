/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         mode.cpp
 * Description  MODE command handler
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

#include <cmd/mode.hpp>

/** class instance */
UnrealCH_mode* handler = NULL;

/**
 * Unreal Command Handler for "MODE" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_mode::UnrealCH_mode(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_MODE, &UnrealCH_mode::exec);
}

/**
 * Unreal Command Handler for "MODE" - Destructor.
 */
UnrealCH_mode::~UnrealCH_mode()
{
	delete command_;
}

/**
 * MODE command handler for User connections.
 *
 * Usage:
 * MODE <target> ['+'|'-'<flags> [param1 [param2 [...]]]]
 *
 * Message example:
 * MODE #channel +nt
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_mode::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_MODE));
	}
	else
	{
		String& target = argv->at(1);

		if (target.at(0) == '#' || target.at(0) == '&')
		{
			UnrealChannel* chptr = UnrealChannel::find(target);
			UnrealChannel::Member* cmptr = 0;

			if (!chptr)
			{
				uptr->sendreply(ERR_NOSUCHCHANNEL,
					String::format(MSG_NOSUCHCHANNEL,
						target.c_str()));
			}
			else if (!(cmptr = chptr->findMember(uptr)) && !uptr->isOper())
			{
				chptr->sendreply(uptr, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL);
			}
			else if (argv->size() == 2)
			{
				chptr->sendreply(uptr, RPL_CHANNELMODEIS,
					String::format(MSG_CHANNELMODEIS,
						chptr->modestr().c_str()));
				chptr->sendreply(uptr, RPL_CHANNELCREATED,
					String::format(MSG_CHANNELCREATED,
						chptr->creationTime().toTS()));
			}
			else
			{
				/* propagate mode change; removed unrequired stuff from argv */
				for (size_t i = 0; i < 2; i++)
					argv->removeFirst();

				chptr->parseModeChange(uptr, argv);
			}
		}
		else
		{
			UnrealUser* tuptr = UnrealUser::find(target);

			/* mode changes on users are permitted from the origin only */
			if (tuptr != uptr)
				uptr->sendreply(ERR_USERSDONTMATCH, MSG_USERSDONTMATCH);
			else if (argv->size() == 2)
			{
				/* print the modes */
				uptr->sendreply(RPL_UMODEIS,
					String::format(MSG_UMODEIS,
						uptr->modestr().c_str()));
			}
			else
			{
				/* propagate mode change; remove unrequired stuff from argv */
				for (size_t i = 0; i < 2; i++)
					argv->removeFirst();

				uptr->parseModeChange(argv);
			}
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_mode::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /MODE command");
	inf->setName("UnrealCH_mode");
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
	handler = new UnrealCH_mode(mptr);
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
