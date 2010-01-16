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

#include <limits.hpp>
#include <map.hpp>
#include <platform.hpp>
#include <reactor.hpp>
#include <resolver.hpp>
#include <string.hpp>

#include <boost/asio.hpp>
#include <boost/signal.hpp>

using namespace boost::asio::ip;

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
	: public tcp::socket
{
public:
	typedef boost::system::error_code ErrorCode;

public:
	UnrealSocket();
	~UnrealSocket();
	void connectTo(UnrealResolver::Endpoint& ep);
	void connectTo(const String& hostname, const uint16_t& portnum);
	void destroyResolverQuery();
	UnrealSocketTrafficType traffic();
	void waitForLine();
	void write(const String& data);

public:
	boost::signal<void(UnrealSocket*)> onConnected;
	boost::signal<void(UnrealSocket*, const ErrorCode&)> onDisconnected;
	boost::signal<void(UnrealSocket*, const ErrorCode&)> onError;
	boost::signal<void(UnrealSocket*, String&)> onRead;

private:
	void handleConnect(const ErrorCode& ec,
		UnrealResolver::Iterator ep_iter);
	void handleRead(const ErrorCode& ec, size_t bytes_read);
	void handleResolveResponse(const ErrorCode& ec,
		UnrealResolver::Iterator ep_iter);
	void handleWrite(const ErrorCode& ec, size_t bytes_written);

private:
	/** stream buffer */
	boost::asio::streambuf streambuf_;

	/** traffic on the socket */
	UnrealSocketTrafficType traffic_;
};

extern Map<UnrealSocket*, UnrealResolver*> resolver_queries;

#endif /* _UNREALIRCD_SOCKET_HPP */
