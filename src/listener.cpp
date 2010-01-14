/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         listener.cpp
 * Description  Connection listener
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
#include "cmdmap.hpp"
#include "command.hpp"
#include "listener.hpp"
#include "numeric.hpp"
#include "user.hpp"
#include <iostream>
#include <boost/bind.hpp>

/**
 * UnrealListener constructor.
 *
 * @param address Address used for binding
 * @param port Port number used for binding
 */
UnrealListener::UnrealListener(const String& address, const uint16_t& port)
	: tcp::acceptor(unreal->reactor()), type_(LClient), address_(address),
	port_(port), ping_freq_(0), max_connections_(0)
{ }

/**
 * UnrealListener destructor.
 */
UnrealListener::~UnrealListener()
{
	for (List<UnrealSocket*>::Iterator i = connections.begin();
			i != connections.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Add an incoming connection.
 *
 * @param sptr Socket pointer
 */
void UnrealListener::addConnection(UnrealSocket* sptr)
{
	std::cout<<"addConnection()\n";
	if (connections.size() >= max_connections_)
	{
		/* all connection slots in use, drop the connection */
		sptr->write("ERROR :All connections in use");
		sptr->close();
		return;
	}

	sptr->onDisconnected.connect(
		boost::bind(&UnrealListener::removeConnection,
			this,
			_1,
			_2));

	sptr->onRead.connect(
		boost::bind(&UnrealListener::handleDataResponse,
			this,
			_1,
			_2));

	/* add to the connection list */
	connections << sptr;

	/* modify stats; every new connection is marked to be "unknown" */
	unreal->stats.connections_unk++;
	unreal->stats.connections_cur++;
	unreal->stats.connections_total++;

	switch (type_)
	{
		case LClient:
		{
			UnrealUser* uptr = new UnrealUser(sptr);
			uptr->setListener(this);

			/* add it into the userlist */
			unreal->users.add(sptr, uptr);

			/* initialize authentication process */
			uptr->auth();

			break;
		}

		case LServer:
		{
			// create Server instance here
			break;
		}

		default:
			break;
	}

	if (unreal->stats.connections_cur > unreal->stats.connections_max)
		unreal->stats.connections_max = unreal->stats.connections_cur;

	/* wait for lines to be read from the socket */
	sptr->waitForLine();
}

/**
 * Returns the address of the endpoint we're bound to.
 *
 * @return Local endpoint address
 */
String UnrealListener::bindAddress()
{
	return address_;
}

/**
 * Returns the port of the endpoint we're bound to.
 *
 * @return Local endpoint port
 */
uint16_t UnrealListener::bindPort()
{
	return port_;
}

/**
 * Acceptor callback.
 *
 * @param ec Error code
 * @param sptr Socket pointer
 */
void UnrealListener::handleAccept(const ErrorCode& ec, UnrealSocket* sptr)
{
	if (ec)
	{
		ErrorCode edupl = ec;
		
		unreal->log.write(UnrealLog::Error, "UnrealListener handleAccept(): %s",
			edupl.message().c_str());
	}
	else
	{
		addConnection(sptr);
		onNewConnection(this, sptr);
		
		/* wait for the next client */
		waitForAccept();
	}
}

/**
 * Socket notification callback for new data available.
 *
 * @param sptr Shared UnrealSocket pointer
 * @param data Last line read from Socket
 */
void UnrealListener::handleDataResponse(UnrealSocket* sptr, String& data)
{
	StringList tokens = splitLine(data);
	size_t shift = 0;

	unreal->log.write(UnrealLog::Debug, ">> %s", data.c_str());

	// check prefix
	if (tokens.at(0).at(0) == ':' && tokens.size() >= 2)
		shift++;

	String cmd = tokens.at(shift + 0).toUpper();

	if (type_ == LClient)
	{
		UnrealUser* uptr = UnrealUser::find(sptr);

		if (!uptr)
		{
			unreal->log.write(UnrealLog::Normal,
					"UnrealListener::handleDataResponse(): "
					"Error: Socket %d not assigned with User?",
					sptr->native());
		}
		else
		{
			UnrealUserCommand* ucptr = UnrealUserCommand::find(cmd);

			if (ucptr)
			{
				/* look if the command is suspended */
				if (!ucptr->isActive())
					uptr->sendreply(CMD_NOTICE,
							String::format(MSG_CMDNOTAVAILABLE,
									cmd.c_str()));
				/* ... or oper-only */
				else if (ucptr->isOperOnly() && !uptr->isOper())
					uptr->sendreply(ERR_NOPRIVILEGES, MSG_NOPRIVILEGES);
				else
				{
					UnrealUserCommand::Function fn = ucptr->fn();

					/* call the command function */
					fn(uptr, &tokens);
				}
			}
			else
				uptr->sendreply(ERR_UNKNOWNCOMMAND,
						String::format(MSG_UNKNOWNCOMMAND,
								cmd.c_str()));

			/* update last action timestamp */
			uptr->setLastActionTime(UnrealTime::now());
		}
	}
}

/**
 * Returns the maximum amount of connections permitted for this listener.
 *
 * @return Connection limit
 */
uint32_t UnrealListener::maxConnections()
{
	return max_connections_;
}

/**
 * Returns the ping frequency.
 *
 * @return Ping frequency in seconds
 */
uint32_t UnrealListener::pingFrequency()
{
	return ping_freq_;
}

/**
 * Remove a connection from the connection list.
 *
 * @param sptr Shared UnrealSocket pointer
 */
void UnrealListener::removeConnection(UnrealSocket* sptr,
	const UnrealSocket::ErrorCode& ec)
{
	std::cout<<String::format("removeConnection()");

	/* if an user, remove it from the userlist */
	if (unreal->users.contains(sptr))
	{
		UnrealUser* uptr = unreal->users[sptr];

		if (ec)
			uptr->exit( const_cast<UnrealSocket::ErrorCode&>(ec) );

		/* destroy user object */
		UnrealUser::destroy(uptr);
	}
	else
		unreal->stats.connections_unk--;

	unreal->stats.connections_cur--;

	connections.remove(sptr);
}

/**
 * Resolve the endpoint, bind to it and listen for incoming connections.
 */
void UnrealListener::run()
{
	UnrealResolver resolv;
	UnrealResolver::ErrorCode ec;
	UnrealResolver::Query query(address_, String(port_));
	UnrealResolver::Endpoint endpoint = *resolv.resolve(query, ec);
	
	if (ec)
	{
		unreal->log.write(UnrealLog::Fatal, "Can't resolve endpoint for %s:%d",
			address_.c_str(), port_);
	}
	
	try
	{
		/* open listener socket */
		open(endpoint.protocol());
		
		/* reuse address if it's anready in use */
		set_option(reuse_address(true), ec);
		
		/* bind acceptor to endpoint */
		bind(endpoint, ec);
		
		/* listen for connections */
		listen(socket_base::max_connections, ec);
	}
	catch (...)
	{
		unreal->log.write(UnrealLog::Fatal, "UnrealListener exception");
	}

	/* wait for connections to be accepted */
	waitForAccept();
}

/**
 * Set the address for the endpoint to bind to.
 *
 * @param address Address name or IPv4/IPv6 address notation
 */
void UnrealListener::setBindAddress(const String& address)
{
	address_ = address;
}

/**
 * Set the port number for the endpoint to bind to.
 *
 * @param port Port number
 */
void UnrealListener::setBindPort(const uint16_t& port)
{
	port_ = port;
}

/**
 * Set the maximum amount of connections permitted for the listener.
 *
 * @param max_conn Connection limit
 */
void UnrealListener::setMaxConnections(const uint32_t& max_conn)
{
	max_connections_ = max_conn;
}

/**
 * Set the ping frequency.
 *
 * @param ping_freq Ping frequency in seconds
 */
void UnrealListener::setPingFrequency(const uint32_t& ping_freq)
{
	ping_freq_ = ping_freq;
}

/**
 * Set the Listener type.
 *
 * @param lty Listener type to set
 */
void UnrealListener::setType(ListenerType lty)
{
	type_ = lty;
}

/**
 * Split an incoming line and return the tokens of it.
 *
 * @param data Incoming line
 * @return StringList with tokens
 */
StringList UnrealListener::splitLine(String& data)
{
	StringList tokens = data.split(" ");

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens.at(i).empty())
			continue;
		else if (tokens.at(i).at(0) == ':' && i > 0)
		{
			String long_arg = tokens.at(i).mid(1);

			for (size_t j = i + 1; j < tokens.size(); j++)
				long_arg = long_arg + " " + tokens.at(j);

			size_t max_size = tokens.size();
			for (size_t k = i + 1; k < max_size; k++)
				tokens.removeLast();

			tokens.at(i) = long_arg;
			break;
		}
	}

	if (tokens.size() == 0)
		tokens.add(data.trimmed());

	return tokens;
}

/**
 * Returns the Listener type.
 *
 * @return ListenerType
 */
UnrealListener::ListenerType UnrealListener::type()
{
	return type_;
}

/**
 * Wait for incoming connections.
 */
void UnrealListener::waitForAccept()
{
	UnrealSocket* sptr = new UnrealSocket();

	async_accept(*sptr,
		boost::bind(&UnrealListener::handleAccept,
			this,
			boost::asio::placeholders::error,
			sptr));
}
