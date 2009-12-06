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
#include "cmdmap.hpp"
#include "user.hpp"
#include <iostream>

/** active resolver queries map */
Map<UnrealSocket*, UnrealResolver*> resolver_queries;

/** user mode definitions */
namespace UnrealUserProperties
{
	/** user will not receive channel messages */
	UnrealUserMode Deaf('d');

	/** user is marked as being invisible */
	UnrealUserMode Invisible('i');

	/** user is marked as being IRC operator */
	UnrealUserMode Operator('o');

	/** user is receiving wallop messages */
	UnrealUserMode Wallops('w');

	/** user mode table */
	UnrealUserModeTable ModeTable;
}

/**
 * UnrealUser constructor.
 *
 * @param sptr Socket pointer if attached to the server directly.
 */
UnrealUser::UnrealUser(UnrealSocket* sptr)
	: socket_(sptr), connection_time_(UnrealTime::now()),
	  timer_(unreal->ios_pool.getIOService())
{ }

/**
 * UnrealUser destructor.
 */
UnrealUser::~UnrealUser()
{
	if (resolver_queries.contains(socket_))
	{
		UnrealResolver* rq = resolver_queries[socket_];

		resolver_queries.remove(socket_);

		delete rq;
	}

	unreal->stats.users_local_cur--;
	//TODO decrease stats, according to if the user is visible or not
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
 * Returns the away message.
 */
const String& UnrealUser::awayMessage()
{
	return away_message_;
}

/**
 * Checks for authorization timeout.
 */
void UnrealUser::checkAuthTimeout(const UnrealSocket::ErrorCode& ec)
{
	if (!ec)
	{
		if (auth_flags_.isset(AFNick) || auth_flags_.isset(AFUser))
		{
			/* haven't received USER and NICK within auth timeout */
			exitClient("Authorization timeout");
		}
		else if (!auth_flags_.isset(AFNick) && !auth_flags_.isset(AFUser)
				&& last_pong_time_.toTS() == 0)
		{
			/* got NICK and USER, but no valid PONG reply */
			exitClient("Ping timeout");
		}
	}
}

/**
 * Checks for ping timeout.
 */
void UnrealUser::checkPingTimeout(const UnrealSocket::ErrorCode& ec)
{
	if (!ec)
	{
		UnrealTime now = UnrealTime::now();

		if (last_pong_time_ < now.addSeconds(-120))
		{
			/* no PONG reply within two minutes */
			exitClient("Ping timeout");
		}
		else
		{
			sendPing();
			schedulePingTimeout();
		}
	}
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
 * Exit client with specified message and close the socket.
 *
 * @param message Message to send for closing link
 */
void UnrealUser::exitClient(const String& message)
{
	String reply;

	reply.sprintf("ERROR :Closing link: %s by %s (%s)",
		nickname_.empty() ? "*" : nickname_.c_str(),
		unreal->config.get("Me/ServerName", "not.configured").c_str(),
		message.c_str());

	send(reply);

	/* close socket */
	if (socket_->is_open())
		socket_->close();
}

/**
 * Lookup a user entry by socket.
 *
 * @param sptr UnrealSocket pointer
 */
UnrealUser* UnrealUser::find(UnrealSocket* sptr)
{
	if (unreal->users.contains(sptr))
		return unreal->users[sptr];
	else
		return 0;
}

/**
 * Lookup a user entry by nick.
 *
 * @param nickname Nick name
 */
UnrealUser* UnrealUser::find(const String& nickname)
{
	String lower = const_cast<String&>(nickname).toLower();
std::cout<<"UnrealUser::find("<<nickname<<")\n";
	if (unreal->nicks.contains(lower))
	{
		std::cout<<"yes, contained by nicks, size="<<unreal->nicks.size()<<"\n";
		return unreal->nicks[lower];
	}
	else
	{
		std::cout<<"no, not found, size="<<unreal->nicks.size()<<"\n";
		return 0;
	}
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
 * Returns whether the user is marked as away.
 */
bool UnrealUser::isAway()
{
	return !away_message_.empty();
}

/**
 * Returns whether the user has IRC operator status.
 */
bool UnrealUser::isOper()
{
	return false;
}

/**
 * Returns the last action timestamp.
 *
 * @return UnrealTime
 */
UnrealTime UnrealUser::lastActionTime()
{
	return last_action_time_;
}

/**
 * Returns the last pong timestamp.
 *
 * @return UnrealTime
 */
UnrealTime UnrealUser::lastPongTime()
{
	return last_pong_time_;
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
 * Returns the user modes bitmask.
 *
 * @return Bitmask
 */
Bitmask<uint16_t>& UnrealUser::modes()
{
	return modes_;
}

/**
 * Returns a readable version of the user modes.
 *
 * @return User mode string w/ params
 */
String UnrealUser::modestr()
{
	UnrealUserModeTable::Iterator umi;
	String result;

	/* user mode table */
	UnrealUserModeTable& modetab = UnrealUserProperties::ModeTable;

	for (umi = modetab.begin(); umi != modetab.end(); umi++)
	{
		UnrealUserMode mo = umi->first;

		if (modes_.isset(umi->second))
			result.append(1, mo.mode_char);
	}

	return result;
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
 * Parse user mode changes.
 *
 * @param args Arguments
 */
void UnrealUser::parseModeChange(StringList* argv)
{
	ModeBuf::StateType state = ModeBuf::Add, last_state = ModeBuf::None;
	String flagset = argv->at(0);
	String changeset;

	/* mode table */
	UnrealUserModeTable& modetab = UnrealUserProperties::ModeTable;

	std::cout << "-- parseModeChange()\n";
	std::cout << "   print modetab:\n";

	for (UnrealUserModeTable::Iterator i = modetab.begin(); i != modetab.end(); i++)
	{
		std::cout << "      iter ch=" << (i->first).mode_char
				<< " fl=" << i->second << "\n";
	}

	std::cout << "   print current modes <" << modes_.value() << ">\n";

	for (UnrealUserModeTable::Iterator i = modetab.begin(); i != modetab.end(); i++)
	{
		if (modes_.isset(i->second))
			std::cout << "      has set ch=" << (i->first).mode_char
				<< " fl=" << i->second << "\n";
	}

	for (String::Iterator ch = flagset.begin(); ch != flagset.end(); ch++)
	{
		if (*ch == '+' || *ch == '-')
			state = (*ch == '+') ? ModeBuf::Add : ModeBuf::Remove;
		else
		{
			if (!modetab.hasFlag(*ch))
			{
				sendreply(ERR_UMODEUNKNOWNFLAG,
					String::format(MSG_UMODEUNKNOWNFLAG,
						*ch));

				continue;
			}

			UnrealUserMode umo = modetab.lookup(*ch);
			uint16_t fl = modetab.value(umo);

			std::cout<<"parseModeChange() ch="<<umo.mode_char
					<< " fl=" << fl << "\n";

			if (last_state != state)
			{
				last_state = state;

				String change_str;

				switch (state)
				{
					case ModeBuf::Add:
						change_str = "+";
						break;
					default:
						change_str = "-";
						break;
				}

				changeset << change_str;
			}

			if (state == ModeBuf::Add)
			{
				/* apply mode flag */
				if (!modes_.isset(fl) && umo != UnrealUserProperties::Operator)
				{
					modes_ << fl;
					changeset.append(1, *ch);

					if (umo == UnrealUserProperties::Invisible)
					{
						unreal->stats.users_inv++;
					}
				}
			}
			else if (state == ModeBuf::Remove)
			{
				/* apply mode flag */
				if (modes_.isset(fl))
				{
					modes_.revoke(fl);
					changeset.append(1, *ch);

					if (umo == UnrealUserProperties::Invisible)
					{
						unreal->stats.users_inv--;
					}
				}
			}
		}
	}

	/* send mode changes */
	if (changeset.length() > 0)
		sendreply(CMD_MODE, changeset);
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
 * Returns the realname for this user.
 *
 * @return Realname
 */
const String& UnrealUser::realname()
{
	return realname_;
}

/**
 * Once any necessary informations like NICK, USER and an valid PONG reply have
 * been received, we can fully register the user.
 */
void UnrealUser::registerUser()
{
	UnrealConfig& cfg = unreal->config;
	static String version = "unrealircd-4.0.0hgal";

	sendreply(RPL_WELCOME,
		String::format(MSG_WELCOME,
			cfg.get("Me/Network", "ExampleNet").c_str(),
			nickname_.c_str()));
	sendreply(RPL_YOURHOST,
		String::format(MSG_YOURHOST,
			cfg.get("Me/ServerName", "not.configured").c_str(),
			version.c_str()));
	sendreply(RPL_CREATED,
		String::format(MSG_CREATED,
			unreal->starttime.toString("%Y-%M-%dT%H:%M:%S/%Z").c_str()));
	sendreply(RPL_MYINFO,
		String::format(MSG_MYINFO,
			cfg.get("Me/ServerName", "not.configured").c_str(),
			version.c_str(),
			"diow", /* to be changed .. */
			"biklmnps",
			"blkov"));

	/* send server features */
	sendISupport();

	/* modify stats */
	unreal->stats.connections_unk--;
	unreal->stats.users_local_cur++;

	if (unreal->users.size() > unreal->stats.users_max)
		unreal->stats.users_max = unreal->users.size();
	if (unreal->stats.users_local_cur > unreal->stats.users_local_max)
		unreal->stats.users_local_max = unreal->stats.users_local_cur;

	/* lusers */
	UnrealUserCommand* ucptr = UnrealUserCommand::find(CMD_LUSERS);

	if (ucptr)
	{
		UnrealUserCommand::Function fn = ucptr->fn();
		fn(this, 0);
	}

	// no motd yet
	sendreply(ERR_NOMOTD, MSG_NOMOTD);

	/*
	 * RPL_WELCOME
	 * RPL_YOURHOST
	 * RPL_CREATED
	 * RPL_MYINFO
	 * send_supported()
	 * Lusers()
	 * motds..
	 *
	 * propagate this message to other servers
	 */
	schedulePingTimeout();
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
 * Schedule the auth timeout check.
 */
void UnrealUser::scheduleAuthTimeout()
{
	std::cout<<"scheduleAuthTimeout()\n";
	int authTimeout = unreal->config.get("Limits/AuthTimeout", "12").toInt();
	timer_.expires_from_now(boost::posix_time::seconds(authTimeout));
	timer_.async_wait(boost::bind(&UnrealUser::checkAuthTimeout, this,
			boost::asio::placeholders::error));
}

/**
 * Schedule the ping timeout check.
 */
void UnrealUser::schedulePingTimeout()
{
	std::cout<<"schedulePingTimeout()\n";
	int ping_freq = static_cast<int>(listener_->pingFrequency());
	timer_.expires_from_now(boost::posix_time::seconds(ping_freq));
	timer_.async_wait(boost::bind(&UnrealUser::checkPingTimeout, this,
			boost::asio::placeholders::error));
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
 * Tell the client the features we support.
 */
void UnrealUser::sendISupport()
{
	String buf;

	for (UnrealISupport::Iterator i = unreal->isupport.begin();
			i != unreal->isupport.end(); i++)
	{
		buf += i->first + "=" + i->second + " ";

		if (buf.length() > 400)
			sendreply(RPL_ISUPPORT, buf);
	}

	if (!buf.empty())
		sendreply(RPL_ISUPPORT, buf);
}

/**
 * Send a reply originating from the client itself using a command.
 *
 * @param cmd Command to send
 * @param data Data block
 */
void UnrealUser::sendlocalreply(const String& cmd, const String& data)
{
	String reply;

	reply.sprintf(":%s!%s@%s %s %s",
			nickname_.c_str(),
			ident_.c_str(),
			hostname_.c_str(),
			cmd.c_str(),
			data.c_str());

	send(reply);
}

/**
 * Send a reply originating from the server using a numeric reply.
 *
 * @param numeric Numeric reply to send
 * @param data Data block
 */
void UnrealUser::sendreply(IRCNumeric numeric, const String& data)
{
	int num = static_cast<int>(numeric);
	String reply;

	reply.sprintf(":%s %03d %s %s",
			unreal->config.get("Me/ServerName", "not.configured").c_str(),
			num,
			nickname_.empty() ? "*" : nickname_.c_str(),
			data.c_str());

	send(reply);
}

/**
 * Send a reply originating from the server using a command.
 *
 * @param cmd Command to send
 * @param data Data block
 */
void UnrealUser::sendreply(const String& cmd, const String& data)
{
	String reply;

	reply.sprintf(":%s %s %s %s",
			unreal->config.get("Me/ServerName", "not.configured").c_str(),
			cmd.c_str(),
			nickname_.empty() ? "*" : nickname_.c_str(),
			data.c_str());

	send(reply);
}

/**
 * Send a reply to another User, originating from the client itself using
 * a command.
 *
 * @param uptr User entry to relay message to
 * @param cmd Command to send
 * @param data Data block
 */
void UnrealUser::sendreply(UnrealUser* uptr, const String& cmd,
		const String& data)
{
	String reply;

	reply.sprintf(":%s!%s@%s %s %s %s",
		nickname_.c_str(),
		ident_.c_str(),
		hostname_.c_str(),
		cmd.c_str(),
		uptr->nick().c_str(),
		data.c_str());

	uptr->send(reply);
}

/**
 * Send a ping request.
 */
void UnrealUser::sendPing()
{
	String request_str;

	request_str.sprintf("PING :%s",
			unreal->config.get("Me/ServerName", "not.configured").c_str());

	send(request_str);
}

/**
 * Update the away message.
 *
 * @param msg New message
 */
void UnrealUser::setAwayMessage(const String& msg)
{
	away_message_ = msg;
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
 * Set the last action timestamp.
 *
 * @param ts Timestamp
 */
void UnrealUser::setLastActionTime(const UnrealTime& ts)
{
	last_action_time_ = ts;
}

/**
 * Set the last pong timestamp.
 *
 * @param ts Timestamp
 */
void UnrealUser::setLastPongTime(const UnrealTime& ts)
{
	last_pong_time_ = ts;
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
	std::cout << "UnrealUser::setNick() to <"<<newnick<<">\n";
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
 * Update the realname.
 *
 * @param rn Real name
 */
void UnrealUser::setRealname(const String& rn)
{
	realname_ = rn;
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

