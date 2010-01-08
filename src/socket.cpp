/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         socket.cpp
 * Description  Socket wrapper class
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

#include "base.hpp"
#include "resolver.hpp"
#include "reactor.hpp"
#include "socket.hpp"
#include <iostream>
#include <boost/bind.hpp>

/** active resolver queries map */
Map<UnrealSocket*, UnrealResolver*> resolver_queries;

/**
 * UnrealSocket constructor.
 */
UnrealSocket::UnrealSocket()
	: boost::asio::ip::tcp::socket(unreal->reactor())
{ }

/**
 * UnrealSocket destructor.
 */
UnrealSocket::~UnrealSocket()
{ }

/**
 * Connect to an external host using the specified endpoint.
 *
 * @param ep Endpoint to connect to
 */
void UnrealSocket::connectTo(UnrealResolver::Endpoint& ep)
{
	async_connect(ep,
		boost::bind(&UnrealSocket::handleConnect,
			this,
			boost::asio::placeholders::error,
			UnrealResolver::Iterator()));
}

/**
 * Inititate a asyncronous connection to an external host using hostname
 * and port.
 *
 * @param hostname Hostname or IP address to connect to
 * @param port Port number to connect to
 */
void UnrealSocket::connectTo(const String& hostname, const uint16_t& port)
{
	UnrealResolver* rq = new UnrealResolver();
	
	rq->onResolve.connect(
		boost::bind(&UnrealSocket::handleResolveResponse,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));

	rq->query(hostname, port);
	
	/* store it somewhere */
	resolver_queries.add(this, rq);
}

/**
 * Destroy pending resolver query object.
 */
void UnrealSocket::destroyResolverQuery()
{
	if (resolver_queries.contains(this))
	{
		resolver_queries.free(this);
	}
}

/**
 * Callback for asyncronous connecting to an remote host.
 *
 * @param ec Error code, if any set
 * @param ep_iter Endpoint iterator
 */
void UnrealSocket::handleConnect(const ErrorCode& ec,
	UnrealResolver::Iterator ep_iter)
{
	if (!ec)
	{
		onConnected(this);
		destroyResolverQuery();
		waitForLine();
	}
	else if (ep_iter != UnrealResolver::Iterator())
	{
		/* try next endpoint */
		UnrealResolver::Endpoint endpoint = *ep_iter;
		
		async_connect(endpoint,
			boost::bind(&UnrealSocket::handleConnect,
				this,
				boost::asio::placeholders::error,
				++ep_iter));
	}
	else
	{
		onError(this, ec);
	}
}

/**
 * Callback for asyncronous reading on the socket.
 * It's called when a new line has arrived for reading or the particular socket
 * throws an error.
 *
 * @param ec error code
 * @param bytes_read Number of bytes read from the socket
 */
void UnrealSocket::handleRead(const ErrorCode& ec, size_t bytes_read)
{
	traffic_.in += static_cast<uint64_t>(bytes_read);
	
	if (ec)
	{
		ErrorCode edupl = ec;

		unreal->log.write(UnrealLog::Error, "Socket read on fd %d failed with "
			"error: %s", native(), edupl.message().c_str());

		onDisconnected(this);
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

/**
 * Callback for resolver query.
 * Called once the resolver gave an response and we can check whether it was
 * valid or not.
 *
 * @param ec Error code
 * @param ep_iter Endpoint iterator
 */
void UnrealSocket::handleResolveResponse(const ErrorCode& ec,
	UnrealResolver::Iterator ep_iter)
{
	if (!ec)
	{
		/* initiate connection */
		UnrealResolver::Endpoint endpoint = *ep_iter;
		
		async_connect(endpoint,
			boost::bind(&UnrealSocket::handleConnect,
				this,
				boost::asio::placeholders::error,
				++ep_iter));
	}
	else
	{
		onError(this, ec);
	}
	
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
		
		unreal->log.write(UnrealLog::Error, "Socket write on fd %d failed with "
			"error: %s", native(), edupl.message().c_str());

		onDisconnected(this);
	}

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
		boost::bind(&UnrealSocket::handleRead,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
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
		boost::bind(&UnrealSocket::handleWrite,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

	/* debug message */
	unreal->log.write(UnrealLog::Debug, "<< %s", data.c_str());
}
