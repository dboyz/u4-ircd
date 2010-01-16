/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         server.hpp
 * Description  Server representation
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

#ifndef _UNREALIRCD_SERVER_HPP
#define _UNREALIRCD_SERVER_HPP

#include <platform.hpp>
#include <string.hpp>
#include <time.hpp>

/**
 * A server representation on the network.
 */
class UnrealServer
{
public:
	UnrealServer(const String& sname = String());
	UnrealTime bootTime();
	UnrealTime linkTime();
	const String& name();
	const uint32_t& numeric();
	UnrealServer* uplink();
	void setBootTime(const UnrealTime& ts);
	void setLinkTime(const UnrealTime& ts);
	void setName(const String& sname);
	void setNumeric(const uint32_t& num);
	void setUplink(UnrealServer* ulptr);

private:
	/** server name */
	String name_;

	/** server numeric */
	uint32_t numeric_;

	/** server uplink */
	UnrealServer* uplink_;

	/** server boot timestamp */
	UnrealTime boot_time_;

	/** server link timestamp */
	UnrealTime link_time_;
};

#endif /* _UNREALIRCD_SERVER_HPP */
