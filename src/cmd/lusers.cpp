/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         lusers.cpp
 * Description  LUSERS command handler
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
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
	"LUSERS command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * LUSERS command handler for User connections.
 *
 * Message example:
 * LUSERS
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_lusers(UnrealUser* uptr, StringList* argv)
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
			st.users_local_cur,
			st.servers_local));
	uptr->sendreply(CMD_NOTICE,
		String::format(MSG_LUSERHIGHEST,
			st.connections_max,
			st.users_max,
			st.connections_total));
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
	uc = new UnrealUserCommand(CMD_LUSERS, &uc_lusers);

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
