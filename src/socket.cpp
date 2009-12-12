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
#include "socket.hpp"
#include <iostream>

/** active resolver queries map */
Map<UnrealSocket*, UnrealResolver*> resolver_queries;

/**
 * UnrealSocket constructor.
 *
 * @param io_service I/O service to attach event notification calls to
 */
UnrealSocket::UnrealSocket()
	: tcp::socket(unreal->ios_pool.getIOService())
{ }

/**
 * UnrealSocket destructor.
 */
UnrealSocket::~UnrealSocket()
{
	destroyResolverQuery();

	if (is_open())
	{
		ErrorCode ec;
		close(ec);

		if (ec)
		{
			unreal->log.write(UnrealLog::Debug, "~UnrealSocket(): close() "
					"failed: %d (%s)", ec.value(), ec.message().c_str());
		}
	}
}

/**
 * Inititate a asyncronous connection to an external host using an endpoint.
 *
 * @param endpoint Endpoint to connect to
 */
void UnrealSocket::connect(Endpoint& endpoint)
{
	async_connect(endpoint,
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
void UnrealSocket::connect(const String& hostname, const uint16_t& portnum)
{
	UnrealResolver* rq = new UnrealResolver();

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
	{
		UnrealResolver* rq = resolver_queries[this];

		resolver_queries.remove(this);

		delete rq;
	}
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
		if (is_open())
		{
			ErrorCode edupl;
			close(edupl);

			if (edupl)
			{
				unreal->log.write(UnrealLog::Debug, "UnrealSocket::handle"
						"Connect(): close() failed: %d (%s)",
						edupl.value(),
						edupl.message().c_str());
			}
		}

		/* next endpoint */
		Endpoint endpoint = *ep_iter;
		async_connect(endpoint,
			boost::bind(&UnrealSocket::handleConnect,
				this,
				boost::asio::placeholders::error,
				++ep_iter));
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

		onError(this, ec);

		if (is_open())
		{
			ErrorCode edupl;
			close(edupl);

			if (edupl)
			{
				unreal->log.write(UnrealLog::Normal, "UnrealSocket::handle"
						"Read(): Error on close(): %d (%s)",
						edupl.value(),
						edupl.message().c_str());
			}
		}

		/* notify on connection loss */
		onDisconnected(this);
	}
	else
	{
		static String buffer;
		std::istream is(&streambuf_);
		std::getline(is, buffer);

		/* clean whitespace */
		buffer = buffer.trimmed();

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
			boost::bind(&UnrealSocket::handleConnect,
				this,
				boost::asio::placeholders::error,
				++ep_iter));
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

		onError(this, ec);

		if (is_open())
		{
			ErrorCode edupl;
			close(edupl);

			if (edupl)
			{
				unreal->log.write(UnrealLog::Normal, "UnrealSocket::handle"
						"Write(): Error on close(): %d (%s)",
						edupl.value(),
						edupl.message().c_str());
			}
		}

		/* notify on connection loss */
		onDisconnected(this);
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
		"\n",
		boost::bind(&UnrealSocket::handleRead, this,
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
			boost::bind(&UnrealSocket::handleWrite, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

	/* debug message */
	unreal->log.write(UnrealLog::Debug, "<< %s", data.c_str());
}
