/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         socket.hpp
 * Description  Socket wrapper class definition
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

#ifndef _UNREALIRCD_SOCKET_HPP
#define _UNREALIRCD_SOCKET_HPP

#include "limits.hpp"
#include "map.hpp"
#include "platform.hpp"
#include "reactor.hpp"
#include "resolver.hpp"
#include "string.hpp"

/**
 * Define a traffic type, that is used to measure how many data
 * is being transmitted/received through a specific socket.
 */
struct UnrealSocketTrafficType
{
	UnrealSocketTrafficType() { reset(); }
	void reset() { in = out = 0; }

	/** number of bytes incoming */
	uint64_t in;

	/** number of bytes outgoing */
	uint64_t out;
};

class UnrealSocket
{
public:
	UnrealSocket(UnrealReactor& reactor);
	~UnrealSocket();
	bool closeSafe();
	void connect(const String& hostname, const uint16_t& portnum);
	void destroyResolverQuery();
	UnrealSocketTrafficType traffic();
	void waitForLine();
	void write(const String& data);

private:
	/** traffic on the socket */
	UnrealSocketTrafficType traffic_;
};

extern Map<UnrealSocket*, UnrealResolver*> resolver_queries;

#endif /* _UNREALIRCD_SOCKET_HPP */
