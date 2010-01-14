/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         command.cpp
 * Description  Command property classes for both user/server cmds
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
#include "command.hpp"

/**
 * UnrealUserCommand constructor.
 *
 * @param name Command name to register
 * @param cfn Command function
 * @param oper_only Optional, defines whether the command requires
 * operator privileges
 */
UnrealUserCommand::UnrealUserCommand(const String& name, Function cfn,
		bool oper_only, bool reg_only)
	: name_(name), fn_(cfn)
{
	if (oper_only)
		flags_ << OperOnly;
	if (reg_only)
		flags_ << Registered;

	unreal->user_commands.add(name, this);
}

/**
 * UnrealUserCommand destructor.
 */
UnrealUserCommand::~UnrealUserCommand()
{
	unreal->user_commands.remove(name_);
}

/**
 * Lookup a command.
 *
 * @return UnrealUserCommand pointer, or `0' when not found
 */
UnrealUserCommand* UnrealUserCommand::find(const String& name)
{
	if (unreal->user_commands.contains(name))
		return unreal->user_commands[name];
	else
		return 0;
}

/**
 * Returns the command name.
 *
 * @return Command name
 */
String UnrealUserCommand::name()
{
	return name_;
}

/**
 * Returns the command function.
 *
 * @return Function
 */
UnrealUserCommand::Function UnrealUserCommand::fn()
{
	return fn_;
}

/**
 * Returns whether the command is available for use.
 *
 * @return true when available, false when not
 */
bool UnrealUserCommand::isActive()
{
	return !flags_.isset(Suspended);
}

/**
 * Returns whether the command is available to IRC operators only.
 *
 * @return true when oper-only, otherwise false
 */
bool UnrealUserCommand::isOperOnly()
{
	return flags_.isset(OperOnly);
}

/**
 * Returns whether the command requires IRC users to be registered to use them.
 *
 * @return true when registered, otherwise false
 */
bool UnrealUserCommand::isRegistered()
{
	return flags_.isset(Registered);
}

/**
 * Set the command activeness state.
 *
 * @param state New state
 */
void UnrealUserCommand::setActive(bool state)
{
	if (state && flags_.isset(Suspended))
		flags_.revoke(Suspended);
	else if (!state && !flags_.isset(Suspended))
		flags_ << Suspended;
}

/**
 * Set whether the command requires IRC operator privileges.
 *
 * @param state New state
 */
void UnrealUserCommand::setOperOnly(bool state)
{
	if (state && !flags_.isset(OperOnly))
		flags_ << OperOnly;
	else if (!state && flags_.isset(OperOnly))
		flags_.revoke(OperOnly);
}

/**
 * Set whether the command requires IRC users to be registered to use them.
 *
 * @param state New state
 */
void UnrealUserCommand::setRegistered(bool state)
{
	if (state && !flags_.isset(Registered))
		flags_ << Registered;
	else if (!state && flags_.isset(Registered))
		flags_.revoke(Registered);
}
