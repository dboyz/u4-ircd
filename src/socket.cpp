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
#include "ioservice.hpp"
#include "resolver.hpp"
#include "socket.hpp"
#include <iostream>

/** active resolver queries map */
Map<UnrealSocket*, UnrealResolver*> resolver_queries;

/**
 * UnrealSocket constructor.
 *
 * @param io_service I/O service to attach event notification calls to
 */
UnrealSocket::UnrealSocket(UnrealIOService& ios)
	: tcp::socket(ios), strand_(ios)
{ }

/**
 * UnrealSocket destructor.
 */
UnrealSocket::~UnrealSocket()
{
	destroyResolverQuery();
}

/**
 * Close the socket safely.
 *
 * @return True if the close succeed, otherwise false
 */
bool UnrealSocket::closeSafe()
{
	onDisconnected(this);

	if (is_open())
	{
		ErrorCode errc;
		close(errc);

		return (errc.value() == 0);
	}
	else
		return true;
}

/**
 * Inititate a asyncronous connection to an external host using an endpoint.
 *
 * @param endpoint Endpoint to connect to
 */
void UnrealSocket::connect(Endpoint& endpoint)
{
	// XXX: boost examples use shared_from_this() instead of "this"
	// looks like it's because of shared_ptr's
	async_connect(endpoint,
		strand_.wrap(boost::bind(&UnrealSocket::handleConnect,
			this,
			boost::asio::placeholders::error,
			UnrealResolver::Iterator())));
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
	UnrealResolver* rq = new UnrealResolver((UnrealIOService&)get_io_service());

	rq->onResolve.connect(boost::bind(&UnrealSocket::handleResolveResponse,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));

	rq->query(hostname, portnum);

	/* store it somewhere */
	resolver_queries.add(this, rq);
}

/**
 * Destroy resolver query.
 */
void UnrealSocket::destroyResolverQuery()
{
	if (resolver_queries.contains(this))
		resolver_queries.free(this);
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
	if (!ec)
	{
		onConnected(this);

		/* wait for lines to be read */
		waitForLine();
	}
	else if (ep_iter != UnrealResolver::Iterator())
	{
		/* next endpoint */
		Endpoint endpoint = *ep_iter;
		async_connect(endpoint,
			strand_.wrap(boost::bind(&UnrealSocket::handleConnect,
				this,
				boost::asio::placeholders::error,
				++ep_iter)));
	}
	else
		onError(this, ec);
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

	if (ec)
	{
		ErrorCode edupl = ec;
		String errmsg = edupl.message();

		unreal->log.write(UnrealLog::Normal, "Error handling async_read on "
				"socket %d: %s (%d)", native(), errmsg.c_str(), edupl.value());

		if (unreal->users.contains(this))
			unreal->users[this]->exit(ec);

		closeSafe();
		onError(this, ec);
	}
	else
	{
		String buffer;
		std::istream is(&streambuf_);
		std::getline(is, buffer);
		size_t trpos = buffer.find_last_not_of("\r\n");

		if (trpos != String::npos)
			buffer.erase(trpos + 1);

		if (buffer.length() > 0)
			onRead(this, buffer);

		/* wait for the next line */
		waitForLine();
	}
}

void UnrealSocket::handleResolveResponse(const ErrorCode& ec,
	UnrealResolver::Iterator ep_iter)
{
	if (!ec)
	{
		onConnecting(this);

		// connect to endpoint
		Endpoint endpoint = *ep_iter;
		async_connect(endpoint,
			strand_.wrap(boost::bind(&UnrealSocket::handleConnect,
				this,
				boost::asio::placeholders::error,
				++ep_iter)));
	}
	else
	{
		onError(this, ec);
	}

	/* remove the resolver query */
	destroyResolverQuery();
}

/**
 * Callback for asyncronous writing to the socket.
 *
 * @param ec boost error_code
 * @param bytes_written Number of bytes written to the socket
 */
void UnrealSocket::handleWrite(const ErrorCode& ec, size_t bytes_written)
{
	if (ec)
	{
		ErrorCode edupl = ec;
		String errmsg = edupl.message();

		unreal->log.write(UnrealLog::Normal, "Error handling async_write on "
				"socket %d: %s (%d)", native(), errmsg.c_str(), edupl.value());

		if (unreal->users.contains(this))
			unreal->users[this]->exit(ec);

		closeSafe();
		onError(this, ec);
	}

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
	boost::asio::async_read_until(*this,
		streambuf_,
		'\n',
		strand_.wrap(boost::bind(&UnrealSocket::handleRead,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)));
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

	boost::asio::async_write(*this,
		boost::asio::buffer(buffer.c_str(), buffer.length()),
			strand_.wrap(boost::bind(&UnrealSocket::handleWrite,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));

	/* debug message */
	unreal->log.write(UnrealLog::Debug, "<< %s", data.c_str());
}
