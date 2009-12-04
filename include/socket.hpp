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

#include "platform.hpp"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

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
	/** alias error code class */
	typedef boost::system::error_code ErrorCode;

	/** alias the socket class for use with shared_ptr */
	typedef boost::shared_ptr<UnrealSocket> SocketPtr;

public:
	UnrealSocket();
	UnrealSocketTrafficType traffic();
	void waitForLine();
	void write(const String& data);

public:
	/** emitted when a Socket starts connecting */
	boost::signals2::signal<void(UnrealSocket&)> onConnecting;

	/** emitted when a Socket is connected to the Endpoint */
	boost::signals2::signal<void(UnrealSocket&)> onConnected;

	/** emitted when a Socket lost the connection */
	boost::signals2::signal<void(UnrealSocket&)> onDisconnected;

	/** emitted when an Error occured */
	boost::signals2::signal<void(UnrealSocket&, const ErrorCode&)> onError;

	/** emitted when new data is available for reading */
	boost::signals2::signal<void(UnrealSocket&, String&)> onRead;

private:
	void handleRead(const ErrorCode& ec, size_t bytes_read);
	void handleWrite(const ErrorCode& ec, size_t bytes_written);

private:
	/** stream buffer */
	boost::asio::streambuf streambuf_;

	/** traffic on the socket */
	UnrealSocketTrafficType traffic_;
};

#endif /* _UNREALIRCD_SOCKET_HPP */
