/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         insmod.cpp
 * Description  INSMOD command handler
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

#include <cmd/insmod.hpp>
#include <cmd/notice.hpp>

/** class instance */
static UnrealCH_insmod* handler = NULL;

/**
 * Unreal Command Handler for "INSMOD" - Constructor.
 *
 * @param mptr Module pointer
 */
UnrealCH_insmod::UnrealCH_insmod(UnrealModule* mptr)
{
	setInfo(&mptr->inf);
	
	/* allocate additional contents */
	command_ = new UnrealUserCommand(CMD_INSMOD, &UnrealCH_insmod::exec, true);
}

/**
 * Unreal Command Handler for "INSMOD" - Destructor.
 */
UnrealCH_insmod::~UnrealCH_insmod()
{
	delete command_;
}

/**
 * INSMOD command handler for User connections.
 *
 * INSMOD allows IRC operators to (re-)load modules dynamically while
 * the server is running.
 *
 * Usage:
 * INSMOD <filename> ["reload"]
 *
 * Message example:
 * INSMOD /home/chris/lib/cmd/help.so
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void UnrealCH_insmod::exec(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 2)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_INSMOD));
	}
	else
	{
		UnrealModule* mptr = UnrealModule::find(argv->at(1));
		bool do_reload = (argv->size() > 2 && argv->at(2) == "reload");
		String errStr;

		if (mptr)
		{
			if (do_reload)
			{
				/* destroy module object */
				unreal->modules.remove(mptr);
				delete mptr;

				/* and load it again */
				mptr = new UnrealModule(argv->at(1));

				if (!mptr->isLoaded())
				{
					errStr = mptr->errorString();
					delete mptr;
				}
				else
					unreal->modules << mptr;
			}
			else
				errStr = "Module is already loaded.";

			if (errStr.empty())
				uptr->sendreply(CMD_NOTICE,
					String::format(MSG_INSMODOK,
						mptr->inf.name().c_str(),
						mptr->inf.version().c_str()));
			else
				uptr->sendreply(CMD_NOTICE,
					String::format(MSG_INSMODFAILED,
						errStr.c_str()));
		}
		else
		{
			mptr = new UnrealModule(argv->at(1));

			if (!mptr->isLoaded())
			{
				uptr->sendreply(CMD_NOTICE,
					String::format(MSG_INSMODFAILED,
						mptr->errorString().c_str()));
				delete mptr;
			}
			else
			{
				uptr->sendreply(CMD_NOTICE,
					String::format(MSG_INSMODOK,
						mptr->inf.name().c_str(),
						mptr->inf.version().c_str()));

				unreal->modules << mptr;
			}
		}
	}
}

/**
 * Updates the module information.
 *
 * @param inf Module information object pointer
 */
void UnrealCH_insmod::setInfo(UnrealModuleInf* inf)
{
	inf->setAPIVersion( MODULE_API_VERSION );
	inf->setAuthor("UnrealIRCd Development Team");
	inf->setDescription("Command Handler for the /INSMOD command");
	inf->setName("UnrealCH_insmod");
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
	handler = new UnrealCH_insmod(mptr);
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
