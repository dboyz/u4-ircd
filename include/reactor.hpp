/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         reactor.hpp
 * Description  Generic event reactor interface
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

#ifndef _UNREALIRCD_REACTOR_HPP
#define _UNREALIRCD_REACTOR_HPP

#include "platform.hpp"
#include "string.hpp"

/**
 * Generic reactor interface.
 */
class UnrealGenericReactor
{
public:
	/**
	 * Generic flags to be used for event administration
	 */
	enum GenericFlagType
	{
		EventError	= 0x01,	// is set when an error occured
		EventHup	= 0x02, // is set when a FD got hung up
		EventIn		= 0x04, // is set when there is data available for reading
		EventOut	= 0x08  // is set when we can send data
	};

	/**
	 * Translation flags for translateEvents()
	 */
	enum TranslationFlag
	{
		Generic, // output type: generic flags
		Native   // output type: native flags
	};

	typedef void (CallbackType)(int, uint32_t);

public:
	virtual bool observe(int fd, uint32_t events)
	{ return false; }
	virtual bool stop(int fd)
	{ return false; }
	virtual uint32_t translateEvents(TranslationFlag ot, uint32_t ev)
	{ return -1; }
	virtual String type()
	{ return "(generic)"; }
};

#endif /* _UNREALIRCD_REACTOR_HPP */