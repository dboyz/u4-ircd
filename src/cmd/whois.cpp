/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         whois.cpp
 * Description  WHOIS command handler
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

#include "base.hpp"
#include "cmdmap.hpp"
#include "command.hpp"
#include "module.hpp"
#include "stringlist.hpp"
#include <iostream>

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"WHOIS command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * WHOIS command handler for User connections.
 *
 * Usage:
 * WHOIS [server] <nick[,nick2[,...]]>
 *
 * Message example:
 * WHOIS ohnobinki
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_whois(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_WHOIS));
	}
	else
	{
		size_t nick_arg = 1;

		if (argv->size() > 2)
		{
			/* in that case, argv->at(1) may be the target server */
			nick_arg++;
		}

		StringList nicks = argv->at(nick_arg).split(",");

		if (nicks.size() == 0)
			nicks << argv->at(nick_arg);

		for (StringList::Iterator sit = nicks.begin();sit != nicks.end();sit++)
		{
			UnrealUser* tuptr = UnrealUser::find(*sit);

			if (!tuptr)
			{
				uptr->sendreply(ERR_NOSUCHNICK,
					String::format(MSG_NOSUCHNICK,
						(*sit).c_str()));
			}
			else
			{
				uptr->sendreply(RPL_WHOIS_USER,
					String::format(MSG_WHOIS_USER,
						tuptr->nick().c_str(),
						tuptr->ident().c_str(),
						tuptr->hostname().c_str(),
						tuptr->realname().c_str()));

				/* collect channel list */
				if (tuptr->channels.size() > 0)
				{
					String result;

					for (List<UnrealChannel*>::Iterator chan
							= tuptr->channels.begin();
							chan != tuptr->channels.end(); chan++)
					{
						UnrealChannel* chptr = *chan;
						UnrealChannel::Member* cmptr=chptr->findMember(tuptr);

						if (chptr->isSecret() && !uptr->isOper())
							continue; // don't show channels with secret flag

						if (cmptr)
						{
							if (!result.empty())
								result.append(1, ' ');
							if (cmptr->isChanOp())
								result.append(1, '@');
							else if (cmptr->isHalfOp())
								result.append(1, '%');
							else if (cmptr->isVoiced())
								result.append(1, '+');

							result << chptr->name();
						}

						if (result.length() >= 420)
							uptr->sendreply(RPL_WHOISCHANNELS,
								String::format(MSG_WHOISCHANNELS,
									tuptr->nick().c_str(),
									result.c_str()));
					}

					if (result.length() > 0)
						uptr->sendreply(RPL_WHOISCHANNELS,
							String::format(MSG_WHOISCHANNELS,
								tuptr->nick().c_str(),
								result.c_str()));
				}

				// TODO: replace server info with informations of target server
				uptr->sendreply(RPL_WHOIS_SERVER,
					String::format(MSG_WHOIS_SERVER,
						tuptr->nick().c_str(),
						unreal->me.name().c_str(),
						unreal->config.get("Me::Description",
							"Example IRC Server").c_str()));

				if (tuptr->isOper())
					uptr->sendreply(RPL_WHOIS_OPERATOR,
						String::format(MSG_WHOIS_OPERATOR,
							tuptr->nick().c_str()));

				if (tuptr->isAway())
					uptr->sendreply(RPL_AWAY,
						String::format(MSG_AWAY,
							tuptr->nick().c_str(),
							tuptr->awayMessage().c_str()));

				/* get the idle time */
				UnrealTime cur = UnrealTime::now();
				cur.addSeconds(-tuptr->connectionTime().toTS());

				uptr->sendreply(RPL_WHOIS_IDLE,
					String::format(MSG_WHOIS_IDLE,
						tuptr->nick().c_str(),
						cur.toTS(),
						tuptr->connectionTime().toTS()));

				uptr->sendreply(RPL_ENDOFWHOIS,
					String::format(MSG_ENDOFWHOIS,
						tuptr->nick().c_str()));
			}
		}
	}
}

/**
 * Module initialization function.
 * Called when the Module is loaded.
 *
 * @param module Reference to Module
 */
UNREAL_DLL UnrealModule::Result unrInit(UnrealModule& module)
{
	/* update module info */
	module.info = modinf;

	/* register command */
	uc = new UnrealUserCommand(CMD_WHOIS, &uc_whois);

	return UnrealModule::Success;
}

/**
 * Module close function.
 * It's called before the Module is unloaded.
 */
UNREAL_DLL UnrealModule::Result unrClose(UnrealModule& module)
{
	delete uc;

	return UnrealModule::Success;
}
