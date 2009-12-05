/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         client.cpp
 * Description  User representation
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
#include "user.hpp"

/** active resolver queries map */
Map<UnrealSocket*, UnrealResolver*> resolver_queries;

/**
 * UnrealUser constructor.
 *
 * @param sptr Socket pointer if attached to the server directly.
 */
UnrealUser::UnrealUser(UnrealSocket* sptr)
	: socket_(sptr), connection_time_(UnrealTime::now())
{ }

/**
 * UnrealUser destructor.
 */
UnrealUser::~UnrealUser()
{
	/* remove nick from nicklist */
	if (!nickname_.empty())
	{
		String lower_nick = lowerNick();

		if (unreal->nicks.contains(lower_nick))
			unreal->nicks.remove(lower_nick);
	}
}

/**
 * Start authentication process.
 */
void UnrealUser::auth()
{
	auth_flags_ << AFNick
				<< AFUser
				<< AFDNS;

	send("NOTICE AUTH :*** Looking up your hostname");

	/* resolve remote host */
	resolveHostname();
}

/**
 * Returns the authflags Bitmask for this user.
 *
 * @return Bitmask
 */
Bitmask<uint8_t>& UnrealUser::authflags()
{
	return auth_flags_;
}

/**
 * Returns the connection time for this user.
 *
 * @return UnrealTime
 */
UnrealTime UnrealUser::connectionTime()
{
	return connection_time_;
}

/**
 * Callback for resolver response.
 *
 * @param ec boost error_code
 * @param response Resolver iterator
 */
void UnrealUser::handleResolveResponse(const UnrealResolver::ErrorCode& ec,
	UnrealResolver::Iterator response)
{
	if (ec)
	{
		send("NOTICE AUTH :Couldn't look up your hostname");
	}
	else
	{
		String tmp = (*response).host_name();

		setHostname(tmp);
		setRealHostname(tmp);

		/* let the user know about the success */
		send(tmp.sprintf("NOTICE AUTH :*** Retrieved hostname (%s)",
				hostname_.c_str()));
	}

	/* remove the previously allocated resolver */
	if (!resolver_queries.contains(socket_))
	{
		unreal->log.write(UnrealLog::Normal, "Error: Can't find resolver "
				"allocated for socket %d", socket_->native());
	}
	else
	{
		UnrealResolver* rq = resolver_queries.value(socket_);

		resolver_queries.remove(socket_);

		delete rq;
	}

	/* revoke auth flag for DNS lookup */
	auth_flags_.revoke(AFDNS);
}

/**
 * Returns the visible hostname for this user.
 *
 * @return Visible hostname
 */
const String& UnrealUser::hostname()
{
	return hostname_;
}

/**
 * Returns the username (ident) for this user.
 *
 * @return Ident
 */
const String& UnrealUser::ident()
{
	return ident_;
}

/**
 * Returns the listener, where this user is attached to (if any).
 *
 * @return UnrealListener pointer
 */
UnrealListener* UnrealUser::listener()
{
	return listener_;
}

/**
 * Returns a lowercase version of the nickname.
 *
 * @return Nickname in lowercase
 */
String UnrealUser::lowerNick()
{
	return nickname_.toLower();
}

/**
 * Returns the nickname for this user.
 *
 * @return Nickname
 */
const String& UnrealUser::nick()
{
	return nickname_;
}

/**
 * Returns the real hostname for this user.
 *
 * @return Real hostname
 */
const String& UnrealUser::realHostname()
{
	return real_hostname_;
}

/**
 * Initialize asyncronous hostname resolving.
 */
void UnrealUser::resolveHostname()
{
	tcp::endpoint endpoint = socket_->remote_endpoint();
	UnrealResolver* rq = new UnrealResolver();

	rq->onResolve.connect(boost::bind(&UnrealUser::handleResolveResponse,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));

	rq->query(endpoint);

	/* be sure we don't forget it */
	resolver_queries.add(socket_, rq);
}

/**
 * Send data to the client.
 *
 * @param data Data to send
 */
void UnrealUser::send(const String& data)
{
	socket_->write(data);
}

/**
 * Update the visible hostname.
 *
 * @param newhost New hostname
 */
void UnrealUser::setHostname(const String& newhost)
{
	hostname_ = newhost;
}

/**
 * Update the username (ident).
 *
 * @param newident New ident
 */
void UnrealUser::setIdent(const String& newident)
{
	ident_ = newident;
}

/**
 * Set the listener.
 *
 * @param lptr Listener pointer
 */
void UnrealUser::setListener(UnrealListener* lptr)
{
	listener_ = lptr;
}

/**
 * Update the nickname.
 *
 * @param newnick New nickname to set
 */
void UnrealUser::setNick(const String& newnick)
{
	/* if there is already a nickname set, remove it from the nicklist */
	if (!nickname_.empty())
	{
		String lower_nick = lowerNick();

		if (unreal->nicks.contains(lower_nick))
			unreal->nicks.remove(lower_nick);
	}

	nickname_ = newnick;

	/* add the new nick into the nick map */
	unreal->nicks.add(lowerNick(), this);
}

/**
 * Update the real hostname.
 *
 * @param newhost New hostname to set
 */
void UnrealUser::setRealHostname(const String& newhost)
{
	real_hostname_ = newhost;
}

/**
 * Returns the socket for this user. It may be `0' if no real socket
 * connection is attached (virtual user).
 *
 * @return UnrealSocket pointer
 */
UnrealSocket* UnrealUser::socket()
{
	return socket_;
}

