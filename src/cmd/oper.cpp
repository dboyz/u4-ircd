/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         oper.cpp
 * Description  OPER command handler
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
#include "hash.hpp"
#include "module.hpp"
#include "stringlist.hpp"

#include <crypto++/sha.h>

/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"OPER command handler",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* uc = 0;

/**
 * Operator type.
 */
struct OperatorType
{
	/**
	 * Lookup an Operator entry.
	 *
	 * @param name Name to lookup
	 * @return Operator entry
	 */
	static OperatorType find(const String& oname)
	{
		OperatorType ret;
		UnrealConfig& config = unreal->config;

		for (size_t i = 1; i <= config.sequenceCount("Operator"); i++)
		{
			String name = config.getSeqVal("Operator", i, "Name", "");
			String mask = config.getSeqVal("Operator", i, "Mask", "*!*@*");
			String pass = config.getSeqVal("Operator", i, "Password", "");

			if (name.empty() || mask.empty() || pass.empty())
				continue;

			if (oname == name)
			{
				ret.mask = mask;
				ret.name = name;
				ret.password = pass;
				break;
			}
		}

		return ret;
	}

	/**
	 * Check the operator password.
	 *
	 * @param pw Plain password to check
	 * @return true when the password is correct, otherwise false
	 */
	bool checkPassword(const String& pw)
	{
		UnrealHash::Type hash_type = UnrealHash::Invalid;
		size_t pos_s, pos_e;

		if ((pos_s = password.find_first_of("$")) != String::npos)
		{
			if ((pos_e = password.find_first_of("$", pos_s + 1))
					!= String::npos)
			{
				String typestr = password.mid(pos_s + 1, (pos_e - pos_s - 1));

				if ((hash_type = UnrealHash::strtotype(typestr))
						!= UnrealHash::Invalid)
				{
					String real_pw = password.mid(pos_e + 1);
					String hash = UnrealHash::calculate(pw, hash_type);
					
					return (hash.toUpper() == real_pw.toUpper());
				}
			}
		}

		return password == pw;
	}

	/** operator mask, if any */
	String mask;

	/** operator name */
	String name;

	/** operator password */
	String password;
};

/**
 * OPER command handler for User connections.
 *
 * Usage:
 * OPER <name> <password>
 *
 * Message example:
 * OPER opername shh!
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_oper(UnrealUser* uptr, StringList* argv)
{
	if (argv->size() < 3)
	{
		uptr->sendreply(ERR_NEEDMOREPARAMS,
			String::format(MSG_NEEDMOREPARAMS,
				CMD_OPER));
	}
	else
	{
		OperatorType oper = OperatorType::find(argv->at(1));

		if (oper.name.empty() || (!oper.mask.empty() 
				&& !uptr->match(oper.mask)))
			uptr->sendreply(ERR_NOOPERHOST, MSG_NOOPERHOST);
		else if (!oper.checkPassword(argv->at(2)))
			uptr->sendreply(ERR_PASSWDMISMATCH, MSG_PASSWDMISMATCH);
		else
		{
			uptr->sendreply(RPL_YOUREOPER, MSG_YOUREOPER);

			/* apply operator flag if necessary */
			if (!uptr->isOper())
			{
				using namespace UnrealUserProperties;

				uptr->modes().add(ModeTable.value(Operator));
				unreal->stats.operators++;
			}

			/* send new modes back */
			uptr->sendlocalreply(CMD_MODE,
				String::format("%s +%s",
					uptr->nick().c_str(),
					uptr->modestr().c_str()));

			/* let other operators know about this */
			uptr->notifyOpers(String::format("%s (%s@%s) is now an IRC operator",
				uptr->nick().c_str(),
				uptr->ident().c_str(),
				uptr->hostname().c_str()));
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
	uc = new UnrealUserCommand(CMD_OPER, &uc_oper);

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
