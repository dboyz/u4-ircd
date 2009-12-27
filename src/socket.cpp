/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         socket.cpp
 * Description  Socket wrapper class
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

#include "base.hpp"
#include "resolver.hpp"
#include "reactor.hpp"
#include "socket.hpp"
#include <iostream>

/** active resolver queries map */
Map<UnrealSocket*, UnrealResolver*> resolver_queries;

/**
 * UnrealSocket constructor.
 */
UnrealSocket::UnrealSocket()
	: native_(-1)
{ }

/**
 * UnrealSocket destructor.
 */
UnrealSocket::~UnrealSocket()
{
	destroyResolverQuery();
}

/**
 * Close the socket.
 *
 * @return True if the close succeed, otherwise false
 */
bool UnrealSocket::close()
{
	if (native_ != -1)
		::close(native_);

	onDisconnected(this);
}

/**
 * Inititate a asyncronous connection to an external host using hostname
 * and port.
 *
 * @param hostname Hostname or IP address to connect to
 * @param port Port number to connect to
 */
void UnrealSocket::connect(const String& hostname, const uint16_t& portnum)
{
//..
}

/**
 * Asyncronous connect callback.
 * This tries to connect to requested host and starts asyncronous reading.
 * Once connect failed, it tries the next endpoint returned from the resolver.
 *
 * @param ec boost error code
 * @param ep_iter Resolver endpoint iterator
 */
void UnrealSocket::handleConnect(const ErrorCode& ec,
	UnrealResolver::Iterator ep_iter)
{
}

/**
 * Callback for asyncronous reading on the socket.
 * It's called when a new line has arrived for reading or the particular socket
 * throws an error.
 *
 * @param ec boost error_code
 * @param bytes_read Number of bytes read from the socket
 */
void UnrealSocket::handleRead(const ErrorCode& ec, size_t bytes_read)
{
	traffic_.in += static_cast<uint64_t>(bytes_read);
}

/**
 * Callback for asyncronous writing to the socket.
 *
 * @param ec boost error_code
 * @param bytes_written Number of bytes written to the socket
 */
void UnrealSocket::handleWrite(const ErrorCode& ec, size_t bytes_written)
{
	/* add actual amount of data that has been written on the socket */
	traffic_.out += static_cast<uint64_t>(bytes_written);
}

/**
 * Returns the traffic object, which holds the number of bytes read and written
 * on the socket.
 *
 * @return UnrealSocketTrafficType
 */
UnrealSocketTrafficType UnrealSocket::traffic()
{
	return traffic_;
}

/**
 * Starts asyncronous reading for a line to be read.
 */
void UnrealSocket::waitForLine()
{
}

/**
 * Write a line to the socket.
 * This automatically appends CRLF to the data.
 *
 * @param data String to be written
 */
void UnrealSocket::write(const String& data)
{
	String buffer = data + "\r\n";

	/* debug message */
	unreal->log.write(UnrealLog::Debug, "<< %s", data.c_str());
}
