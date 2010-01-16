/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         modebuf.hpp
 * Description  Buffer to hold mode change information
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

#ifndef _UNREALIRCD_MODEBUF_HPP
#define _UNREALIRCD_MODEBUF_HPP

#include <string.hpp>

class UnrealUser;

/**
 * Mode buffer type. Used to build MODE lines easily.
 */
template<typename _ModeType>
struct UnrealModeBufferType
{
	/** entry state */
	enum StateType { None, Add, Remove };

	/** entry state as string */
	String statestr()
	{
		switch (state)
		{
			case Add:
				return "+";

			case Remove:
				return "-";

			case None:
			default:
				return "";
		}
	}

	/** target user entry, if any */
	UnrealUser* uptr;

	/** additional arguments */
	String args;

	/** mode type */
	_ModeType mode;

	/** state */
	StateType state;
};

#endif /* _UNREALIRCD_MODEBUF_HPP */
