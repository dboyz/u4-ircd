/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         names.cpp
 * Description  NAMES command handler
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

#include <cmd/names.hpp>

/** class instance */
UnrealCH_names* handler = NULL;

/**
 * Unreal Command Handler for "NAMES" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_names::UnrealCH_names(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_NAMES, &UnrealCH_names::exec);
}

/**
 * Unreal Command Handler for "NAMES" - Destructor.
 */
UnrealCH_names::~UnrealCH_names()
{
	delete command_;
}

/**
 * NAMES command handler for User connections.
 *
 * Usage:
 * NAMES <#channel[,#channel2[,...]]>
 *
 * Message example:
 * NAMES #test
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_names::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_NAMES));
	}
	else
	{
		StringList chlist = argv->at(1).split(",");

		if (chlist.size() == 0)
			chlist << argv->at(1);

		for (StringList::Iterator chan = chlist.begin(); chan != chlist.end();
				++chan)
		{
			UnrealChannel* chptr = UnrealChannel::find(argv->at(1));

			if (chptr)
			{
				String buf;
				char prefix;

				if (chptr->isSecret())
					prefix = '@';
				else if (chptr->isPrivate())
					prefix = '*';
				else
					prefix = '=';

				for (UnrealChannel::MemberIterator cm = chptr->members.begin();
						cm != chptr->members.end(); ++cm)
				{
					UnrealChannel::Member* cmptr = cm->second;

					if (cmptr->flags.isset(UnrealChannel::Member::ChanOp))
						buf.append(1, '@');
					else if (cmptr->flags.isset(UnrealChannel::Member::Voice))
						buf.append(1, '+');

					buf += cmptr->user->nick();
					buf += " ";

					/*
					 * It seems that the "=" in the NAMREPLY is for public
					 * channels, "@" for secret channels.
					 */
					if (buf.length() > 420)
					{
						uptr->sendreply(RPL_NAMREPLY,
							String::format(MSG_NAMREPLY,
								prefix,
								chptr->name().c_str(),
								buf.trimmed().c_str()));

						buf.clear();
						continue;
					}
				}

				String trbuf = buf.trimmed();
				if (trbuf.length() > 0)
					uptr->sendreply(RPL_NAMREPLY,
						String::format(MSG_NAMREPLY,
							prefix,
							chptr->name().c_str(),
							trbuf.c_str()));

				uptr->sendreply(RPL_ENDOFNAMES,
					String::format(MSG_ENDOFNAMES,
						chptr->name().c_str()));
			}
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_names::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /NAMES command");
	inf->setName("UnrealCH_names");
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
	handler = new UnrealCH_names(mptr);
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
