/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         kick.cpp
 * Description  KICK command handler
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

#include <cmd/kick.hpp>

/** class instance */
static UnrealCH_kick* handler = NULL;

/**
 * Unreal Command Handler for "KICK" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_kick::UnrealCH_kick(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_KICK, &UnrealCH_kick::exec);
}

/**
 * Unreal Command Handler for "KICK" - Destructor.
 */
UnrealCH_kick::~UnrealCH_kick()
{
	delete command_;
}

/**
 * KICK command handler for User connections.
 *
 * Usage:
 * KICK <channel> <nick> [:<comment>]
 *
 * Message example:
 * KICK #test WiZ :get out of here
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_kick::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_KICK));
	}
	else
	{
		UnrealChannel* chptr = UnrealChannel::find(argv->at(1));
		UnrealChannel::Member* cmptr = 0;
		UnrealUser* tuptr = UnrealUser::find(argv->at(2));

		if (!chptr)
		{
			uptr->sendreply(ERR_NOSUCHCHANNEL,
				String::format(MSG_NOSUCHCHANNEL,
					argv->at(1).c_str()));
		}
		else if (!tuptr)
		{
			uptr->sendreply(ERR_NOSUCHNICK,
				String::format(MSG_NOSUCHNICK,
					argv->at(2).c_str()));
		}
		else if (!(cmptr = chptr->findMember(uptr)))
		{
			chptr->sendreply(uptr, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL);
		}
		else if (!cmptr->isChanOp() && !uptr->isOper())
		{
			chptr->sendreply(uptr, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED);
		}
		else if (!(cmptr = chptr->findMember(tuptr)))
		{
			chptr->sendreply(uptr, ERR_USERNOTINCHANNEL,
				String::format(MSG_USERNOTINCHANNEL,
					tuptr->nick().c_str()));
		}
		else
		{
			String message;

			if (argv->size() > 3)
				message = argv->at(3);

			if(!message.length())
				message = uptr->nick();

			chptr->sendlocalreply(uptr, CMD_KICK,
				String::format("%s :%s",
					tuptr->nick().c_str(),
					message.c_str()));

			tuptr->leaveChannel(chptr->name(), message, CMD_KICK);
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_kick::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /KICK command");
	inf->setName("UnrealCH_kick");
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
	handler = new UnrealCH_kick(mptr);
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
