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

#include <base.hpp>
#include <command.hpp>
#include <stringlist.hpp>

#include <cmd/who.hpp>

/** class instance */
static UnrealCH_who* handler = NULL;

/**
 * Unreal Command Handler for "WHO" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_who::UnrealCH_who(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_WHO, &UnrealCH_who::exec);
}

/**
 * Unreal Command Handler for "WHO" - Destructor.
 */
UnrealCH_who::~UnrealCH_who()
{
	delete command_;
}

/**
 * Get a common channel with the user specified.
 *
 * @return Channel or 0 if no common channels found
 */
UnrealChannel* UnrealCH_who::commonChannel(UnrealUser* uptr, UnrealUser* tuptr)
{
	foreach (List<UnrealChannel*>::Iterator, ci, uptr->channels)
	{
		UnrealChannel* chptr = *ci;
		
		if (chptr->findMember(tuptr))
			return chptr;
	}
	
	return 0;
}

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
void UnrealCH_who::exec(UnrealUser* uptr, StringList* argv)
{
	bool want_oper = (argv->size() >= 3 && argv->at(2) == "o");
	String mask = "*";

	if (argv->size() >= 2)
		mask = argv->at(1);

	/* TODO: this is a very minimal WHO implementation. No mask matching
	 * has been added yet.
	 */
	if (mask.at(0) == '#' || mask.at(0) == '&')
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
						unreal->me->name().c_str(),
						tuptr->nick().c_str(),
						status.c_str(),
						0,
						tuptr->realname().c_str()));
			}
		}
	}
	else
	{
		UnrealChannel* chptr;
		bool can_recv_inv;
	

		foreach (List<UnrealUser*>::Iterator, ui, unreal->users)
		{
			UnrealUser* tuptr = *ui;
			
			/* just list that user if it's visible to the other user in some
			 * way; IRC operators can always see anyone
			 */
			can_recv_inv = ((chptr = UnrealCH_who::commonChannel(uptr, tuptr))
				|| uptr->isOper());

			if (tuptr->isInvisible() && !can_recv_inv && tuptr != uptr)
				continue;
			else if (want_oper && !tuptr->isOper())
				continue;
			else if (!tuptr->match(mask) && tuptr != uptr)
				continue;

			String status = (tuptr->isAway() ? "G" : "H");

			if (tuptr->isOper())
				status.append(1, '*');
			
			uptr->sendreply(RPL_WHOREPLY,
				String::format(MSG_WHOREPLY,
					chptr ? chptr->name().c_str() : "*",
					tuptr->ident().c_str(),
					tuptr->hostname().c_str(),
					unreal->me->name().c_str(),
					tuptr->nick().c_str(),
					status.c_str(),
					0,
					tuptr->realname().c_str()));
		}
	}

	uptr->sendreply(RPL_ENDOFWHO,
		String::format(MSG_ENDOFWHO,
			mask.c_str()));
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_who::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /WHO command");
	inf->setName("UnrealCH_who");
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
	handler = new UnrealCH_who(mptr);
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
