/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         lusers.cpp
 * Description  LUSERS command handler
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

#include <cmd/lusers.hpp>
#include <cmd/notice.hpp>

/** class instance */
UnrealCH_lusers* handler = NULL;

/**
 * Unreal Command Handler for "LUSERS" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_lusers::UnrealCH_lusers(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_LUSERS, &UnrealCH_lusers::exec);
}

/**
 * Unreal Command Handler for "LUSERS" - Destructor.
 */
UnrealCH_lusers::~UnrealCH_lusers()
{
	delete command_;
}

/**
 * LUSERS command handler for User connections.
 *
 * Message example:
 * LUSERS
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_lusers::exec(UnrealUser* uptr, StringList* argv)
{
	UnrealLocalStat& st = unreal->stats;

	uptr->sendreply(RPL_LUSERS,
		String::format(MSG_LUSERS,
			(unreal->users.size() - st.users_inv),
			st.users_inv,
			unreal->servers.size()));
	uptr->sendreply(RPL_LUSEROPS,
		String::format(MSG_LUSEROPS,
			st.operators));
	uptr->sendreply(RPL_LUSERUNKNOWN,
		String::format(MSG_LUSERUNKNOWN,
			st.connections_unk));
	uptr->sendreply(RPL_LUSERCHANNELS,
		String::format(MSG_LUSERCHANNELS,
				unreal->channels.size()));
	uptr->sendreply(RPL_LUSERME,
		String::format(MSG_LUSERME,
			unreal->local_users.size(),
			st.servers_local));
	uptr->sendreply(CMD_NOTICE,
		String::format(MSG_LUSERHIGHEST,
			st.connections_max,
			st.users_max,
			st.connections_total));
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_lusers::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /LUSERS command");
	inf->setName("UnrealCH_lusers");
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
	handler = new UnrealCH_lusers(mptr);
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
