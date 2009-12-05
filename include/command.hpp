/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         command.hpp
 * Description  Command property classes for both user/server cmds
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

#ifndef _UNREALIRCD_COMMAND_HPP
#define _UNREALIRCD_COMMAND_HPP

#include "user.hpp"
#include "string.hpp"
#include "stringlist.hpp"

class UnrealUserCommand
{
public:
	/** create an function type for the commands itself */
	typedef void (*Function)(UnrealUser*, StringList*);

public:
	UnrealUserCommand(const String& name, Function cfn,
		bool oper_only = false);
	~UnrealUserCommand();
	static UnrealUserCommand* find(const String& name);
	String name();
	Function fn();
	bool isActive();
	bool isOperOnly();
	void setActive(bool state);
	void setFn(const Function& fn);
	void setName(const String& name);
	void setOperOnly(bool state);

private:
	/** command name */
	String name_;

	/** command function */
	Function fn_;

	/** defines whether the command requires operator privileges */
	bool oper_only_;

	/** defines whether the command is active */
	bool active_;
};

#endif /* _UNREALIRCD_COMMAND_HPP */
