/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         join.cpp
 * Description  JOIN command handler
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

#include <cmd/join.hpp>
#include <cmd/part.hpp>

/** class instance */
static UnrealCH_join* handler = NULL;

/**
 * Unreal Command Handler for "JOIN" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_join::UnrealCH_join(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_JOIN, &UnrealCH_join::exec);
}

/**
 * Unreal Command Handler for "JOIN" - Destructor.
 */
UnrealCH_join::~UnrealCH_join()
{
	delete command_;
}

/**
 * JOIN command handler for User connections.
 *
 * Usage:
 * JOIN #channel[,#channel2[,...]] [key1[,key2[,...]]]
 * JOIN 0
 *
 * Message example:
 * JOIN #dev,#home
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_join::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_JOIN));
	}
	else
	{
		StringList cl, kl;

		if (argv->at(1).contains(","))
			cl = argv->at(1).split(",");
		else
			cl << argv->at(1);

		/* get keys if any */
		if (argv->size() > 2)
		{
			if (argv->at(2).contains(","))
				kl = argv->at(2).split(",");
			else
				kl << argv->at(2);
		}

		for (StringList::Iterator chan = cl.begin(); chan != cl.end(); ++chan)
		{
			String tmp_chan = *chan;
			String key;

			if (tmp_chan.empty())
				continue;
			else if (tmp_chan == "0")
			{
				/* leave all channels */
				for (List<UnrealChannel*>::Iterator i = uptr->channels.begin();
						i != uptr->channels.end();)
				{
					uptr->leaveChannel((*i)->name(), String(), CMD_PART);

					/* reset the iterator */
					i = uptr->channels.begin();
				}

				return;
			}

			/* if a key has been specified, extract it */
			if (kl.size() > 0)
				key = kl.takeFirst();

			/* if no channel prefix was provided, add it */
			if (tmp_chan.at(0) != '#' && tmp_chan.at(0) != '&')
				tmp_chan.prepend("#");

			/*
			 * Support for local channels is not provided anymore.
			 */

			if (uptr->channels.size() >= unreal->config.get(
				"Limits::MaxChansPerUser", "20").toSize())
			{
				uptr->sendreply(ERR_TOOMANYCHANNELS,
					String::format(MSG_TOOMANYCHANNELS,
						tmp_chan.c_str()));
			}
			else if (!UnrealChannel::find(tmp_chan) &&
					(!unreal->config.get("Features::ChannelCreation", "true")
						.toBool() && !uptr->isOper()))
			{
				uptr->sendreply(ERR_BANNEDFROMCHAN,
					String::format(MSG_BANNEDFROMCHAN,
						tmp_chan.c_str()));
			}
			else
				uptr->joinChannel(tmp_chan, key);
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_join::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /JOIN command");
	inf->setName("UnrealCH_join");
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
	handler = new UnrealCH_join(mptr);
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
