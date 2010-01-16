/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         info.cpp
 * Description  INFO command handler
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

#include <cmd/info.hpp>

/** class instance */
UnrealCH_info* handler = NULL;

/**
 * Unreal Command Handler for "INFO" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_info::UnrealCH_info(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_INFO, &UnrealCH_info::exec);
}

/**
 * Unreal Command Handler for "INFO" - Destructor.
 */
UnrealCH_info::~UnrealCH_info()
{
	delete command_;
}

/**
 * INFO command handler for User connections.
 *
 * Usage:
 * INFO [<target>]
 *
 * Message example:
 * INFO some.server.net
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_info::exec(UnrealUser* uptr, StringList* argv)
{
	String target = unreal->me->name();

	if (argv->size() >= 2)
		target = argv->at(1);

	if (target == unreal->me->name())
	{
		StringList output = readInfoFile();

		output << String(": ") << String(": ");
		output << String::format(":Revision: %s",
			UnrealVersion::package_changeset.c_str());
		output << String::format(":Birth date: %s %s", __DATE__, __TIME__);
		output << String::format(":On-line since %s",
			unreal->me->bootTime().toString("%Y-%M-%dT%H:%M:%S %Z").c_str());

		/* now send it */
		for (StringList::Iterator i = output.begin(); i != output.end(); ++i)
			uptr->sendreply(RPL_INFO, (*i).c_str());

		uptr->sendreply(RPL_ENDOFINFO, MSG_ENDOFINFO);
	}
}

StringList UnrealCH_info::readInfoFile()
{
	StringList sl;

	sl << ":UnrealIRCd, Version 4";
	sl << ": ";
	sl << ":This program is free software; you can redistribute it and/or";
	sl << ":modify it under the terms of the GNU General Public License as";
	sl << ":published by the Free Software Foundation; either version 3, or";
	sl << ":(at your option) any later version.";
	sl << ": ";
	sl << ":UnrealIRCd 4 was written in C++ with a new, modern code base - ";
	sl << ":to make your IRC experience even better.";
	sl << ": ";
	sl << ":Thanks to all people who made this possible.";

	return sl;
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_info::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /INFO command");
	inf->setName("UnrealCH_info");
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
	handler = new UnrealCH_info(mptr);
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
