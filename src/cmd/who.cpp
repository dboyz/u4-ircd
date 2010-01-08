/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         who.cpp
 * Description  WHO command handler
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

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"WHO command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * WHO command handler for User connections.
 *
 * Usage:
 * WHO [<mask>] ["o"]
 *
 * Message example:
 * WHO #test
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_who(UnrealUser* uptr, StringList* argv)
{
	bool want_oper = (argv->size() >= 3 && argv->at(2) == "o");
	String mask = "*";

	if (argv->size() >= 2)
		mask = argv->at(1);

	/* TODO: this is a very minimal WHO implementation. No mask matching
	 * has been added yet.
	 */
	if (mask.at(0) == '#')
	{
		UnrealChannel* chptr = UnrealChannel::find(mask);

		if (chptr)
		{
			/* define whether the user can receive invisible entries */
			bool can_recv_inv = (chptr->findMember(uptr) || uptr->isOper());

			for (UnrealChannel::MemberIterator i = chptr->members.begin();
					i != chptr->members.end(); ++i)
			{
				UnrealUser* tuptr = i->first;

				if (tuptr->isInvisible() && !can_recv_inv)
					continue;
				else if (want_oper && !tuptr->isOper())
					continue;

				String status = (tuptr->isAway() ? "G" : "H");

				if (tuptr->isOper())
					status.append(1, '*');

				uptr->sendreply(RPL_WHOREPLY,
					String::format(MSG_WHOREPLY,
						chptr->name().c_str(),
						tuptr->ident().c_str(),
						tuptr->hostname().c_str(),
						unreal->me.name().c_str(),
						tuptr->nick().c_str(),
						status.c_str(),
						0,
						tuptr->realname().c_str()));
			}
		}
	}

	uptr->sendreply(RPL_ENDOFWHO,
		String::format(MSG_ENDOFWHO,
			mask.c_str()));
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
	uc = new UnrealUserCommand(CMD_WHO, &uc_who);

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
