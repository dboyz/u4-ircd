/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         privmsg.cpp
 * Description  PRIVMSG command handler
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

#include <cmd/privmsg.hpp>

/** class instance */
static UnrealCH_privmsg* handler = NULL;

/**
 * Unreal Command Handler for "PRIVMSG" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_privmsg::UnrealCH_privmsg(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_PRIVMSG, &UnrealCH_privmsg::exec);
}

/**
 * Unreal Command Handler for "PRIVMSG" - Destructor.
 */
UnrealCH_privmsg::~UnrealCH_privmsg()
{
	delete command_;
}

/**
 * PRIVMSG command handler for User connections.
 *
 * Usage:
 * PRIVMSG <target[,target2[,...]]> :<message>
 *
 * Message example:
 * PRIVMSG #channel1,#channel2 :You're not alone.
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_privmsg::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_PRIVMSG));
	}
	else if (argv->at(2).length() == 0)
	{
		uptr->sendreply(ERR_NOTEXTTOSEND, MSG_NOTEXTTOSEND);
	}
	else
	{
		StringList targets = argv->at(1).split(",");

		if (targets.size() == 0)
			targets << argv->at(1);

		for (StringList::Iterator sli = targets.begin(); sli != targets.end();
				++sli)
		{
			String& target = *sli;

			if (target.at(0) == '#' || target.at(0) == '&')
			{
				UnrealChannel* chptr = UnrealChannel::find(target);

				if (!chptr)
				{
					uptr->sendreply(ERR_NOSUCHCHANNEL,
						String::format(MSG_NOSUCHCHANNEL,
							target.c_str()));
				}
				else
				{
					if (!chptr->canSend(uptr, argv->at(2)))
						uptr->sendreply(ERR_CANNOTSENDTOCHAN,
							String::format(MSG_CANNOTSENDTOCHAN,
								chptr->name().c_str()));
					else if (uptr->isDeaf())
						return;
					else
						chptr->sendlocalreply(uptr, CMD_PRIVMSG,
							String::format(":%s",
								argv->at(2).c_str()),
							true);
				}
			}
			else if (target.at(0) == '$' && uptr->isOper())
			{
				/* mask sending - IRC operators only */
				Map<UnrealSocket*, UnrealUser*>::Iterator ui;
				String pattern = target.mid(1);
				String mask;

				foreach (List<UnrealUser*>::Iterator, ui, unreal->users)
				{
					UnrealUser* user = *ui;

					if (user->match(pattern))
					{
						uptr->sendreply(user, CMD_PRIVMSG,
							String::format(":%s",
								argv->at(2).c_str()));
					}
				}					
			}
			else
			{
				UnrealUser* tuptr = UnrealUser::find(target);

				if (!tuptr)
				{
					uptr->sendreply(ERR_NOSUCHNICK,
						String::format(MSG_NOSUCHNICK,
							target.c_str()));
				}
				else
				{
					uptr->sendreply(tuptr, CMD_PRIVMSG,
						String::format(":%s",
							argv->at(2).c_str()));
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
void UnrealCH_privmsg::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /PRIVMSG command");
	inf->setName("UnrealCH_privmsg");
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
	handler = new UnrealCH_privmsg(mptr);
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
