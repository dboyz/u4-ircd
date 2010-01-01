/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         socket.hpp
 * Description  Socket wrapper class definition
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

#ifndef _UNREALIRCD_SOCKET_HPP
#define _UNREALIRCD_SOCKET_HPP

#include "limits.hpp"
#include "map.hpp"
#include "platform.hpp"
#include "reactor.hpp"
#include "resolver.hpp"
#include "signal.hpp"
#include "string.hpp"
#include <errno.h>

/**
 * Error code definitions for networking.
 */
namespace ErrorCode
{
	namespace Socket
	{
		enum Type
		{
			// bad file descriptor
			BadFd = EBADFD,

			// protocol not supported
			ProtocolNotSupported = EPROTONOSUPPORT,

			// address is already in use
			AddressInUse = EADDRINUSE,

			// network is down
			NetworkDown = ENETDOWN,

			// network is unreachable
			NetworkUnreachable = ENETUNREACH,

			// network dropped connection because of reset
			NetworkReset = ENETRESET,

			// software caused connection abort
			ConnectionAborted = ECONNABORTED,

			// connection reset by peer
			ConnectionReset = ECONNRESET,

			// endpoint is already connected
			AlreadyConnected = EISCONN,

			// endpoint is not connected
			NotConnected = ENOTCONN,

			// connection timed out
			TimedOut = ETIMEDOUT,

			// connection refused
			ConnectionRefused = ECONNREFUSED,

			// host is down
			HostIsDown = EHOSTDOWN,

			// no route to host
			HostUnreachable = EHOSTUNREACH,

			// operation already in progress
			AlreadyInProgress = EALREADY,

			// operation now in progress
			InProgress = EINPROGRESS,
		};
	}
}

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
	typedef ErrorCode::Socket::Type Error;

public:
	UnrealSocket();
	~UnrealSocket();
	bool close();
	void connect(const String& hostname, const uint16_t& portnum);
	int native();
	UnrealSocketTrafficType traffic();
	void waitForLine();
	void write(const String& data);

public:
	UnrealSignal1<void(UnrealSocket*), UnrealSocket*> onConnected;
	UnrealSignal1<void(UnrealSocket*), UnrealSocket*> onDisconnected;

private:
	/** native file descriptor */
	int native_;

	/** traffic on the socket */
	UnrealSocketTrafficType traffic_;
};

extern Map<UnrealSocket*, UnrealResolver*> resolver_queries;

#endif /* _UNREALIRCD_SOCKET_HPP */
