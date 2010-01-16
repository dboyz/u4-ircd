/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         rehash.hpp
 * Description  REHASH command handler
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

#ifndef _UNREALIRCD_CMD_REHASH_HPP
#define _UNREALIRCD_CMD_REHASH_HPP

#include <module.hpp>

#define CMD_REHASH	"REHASH"
#define TOK_REHASH	"REHASH"

/**
 * Unreal Command Handler for "REHASH"
 */
class UnrealCH_rehash
{
public:
	UnrealCH_rehash(UnrealModule* mptr);
	~UnrealCH_rehash();

	static void exec(UnrealUser* uptr, StringList* argv);
	void setInfo(UnrealModuleInf* inf);

private:
	UnrealUserCommand* command_;
};

#endif /* _UNREALIRCD_CMD_REHASH_HPP */
