/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         topic.cpp
 * Description  TOPIC command handler
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

#include <cmd/topic.hpp>

/** class instance */
UnrealCH_topic* handler = NULL;

/**
 * Unreal Command Handler for "TOPIC" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_topic::UnrealCH_topic(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_TOPIC, &UnrealCH_topic::exec);
}

/**
 * Unreal Command Handler for "TOPIC" - Destructor.
 */
UnrealCH_topic::~UnrealCH_topic()
{
	delete command_;
}

/**
 * TOPIC command handler for User connections.
 *
 * Usage:
 * TOPIC <channel> [:<message>]
 *
 * Message example:
 * TOPIC #dev :my new topic
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_topic::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_TOPIC));
	}
	else if (argv->size() >= 2)
	{
		UnrealChannel* chptr = UnrealChannel::find(argv->at(1));

		if (!chptr)
		{
			uptr->sendreply(ERR_NOSUCHCHANNEL,
				String::format(MSG_NOSUCHCHANNEL,
					argv->at(1).c_str()));
		}
		else
		{
			/* display topic */
			if (argv->size() == 2)
			{
				String topic = chptr->topic();

				if (topic.length() == 0)
				{
					chptr->sendreply(uptr, RPL_NOTOPIC, MSG_NOTOPIC);
				}
				else
				{
					chptr->sendreply(uptr, RPL_TOPIC,
						String::format(MSG_TOPIC,
							topic.c_str()));

					chptr->sendreply(uptr, RPL_TOPICLASTCHANGED,
						String::format(MSG_TOPICLASTCHANGED,
							chptr->topicMask().c_str(),
							chptr->topicTime().toTS()));
				}
			}
			else
			{
				/* change the topic */
				UnrealChannel::Member* cmptr = chptr->findMember(uptr);

				if (!cmptr)
					chptr->sendreply(uptr, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL);
				else if (chptr->isTopicOpsOnly() && !cmptr->isChanOp()
						&& cmptr->isHalfOp())
					chptr->sendreply(uptr, ERR_CHANOPRIVSNEEDED,
						MSG_CHANOPRIVSNEEDED);
				else
				{
					chptr->setTopic(argv->at(2));
					chptr->setTopicMask(uptr->nick());
					chptr->setTopicTime(UnrealTime::now());

					/* let anyone know about the new topic */
					chptr->sendlocalreply(uptr, CMD_TOPIC,
						String::format(MSG_TOPIC,
							chptr->topic().c_str()));
				}
			}
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_topic::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /TOPIC command");
	inf->setName("UnrealCH_topic");
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
	handler = new UnrealCH_topic(mptr);
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
