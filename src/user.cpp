/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         user.cpp
 * Description  User representation
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

#include <base.hpp>
#include <limits.hpp>
#include <user.hpp>

#include <cmd/join.hpp>
#include <cmd/lusers.hpp>
#include <cmd/mode.hpp>
#include <cmd/motd.hpp>
#include <cmd/names.hpp>
#include <cmd/notice.hpp>
#include <cmd/part.hpp>
#include <cmd/quit.hpp>
#include <cmd/topic.hpp>

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

/** ident check query map */
Map<UnrealUser*, UnrealSocket*> icheck_queries;

/** a list with user entries that should be destroyed once all additional
  * operations have finished
  */
List<UnrealUser*> user_destructs;

/** special signals */
boost::signal<void(UnrealUser*)> UnrealUser::onCreate;
boost::signal<void(UnrealUser*)> UnrealUser::onDestroy;

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
	: score(0), socket_(sptr), connection_time_(UnrealTime::now())
{
	UnrealUser::onCreate(this);
}

/**
 * UnrealUser destructor.
 */
UnrealUser::~UnrealUser()
{
	if (unreal->stats.users_local_cur > 0)
		unreal->stats.users_local_cur--;

	if (isInvisible() && unreal->stats.users_inv > 0)
		unreal->stats.users_inv--;
	if (auth_flags_.value() != 0 && unreal->stats.connections_unk > 0)
		unreal->stats.connections_unk--;
	if (isOper() && unreal->stats.operators > 0)
		unreal->stats.operators--;

	if (icheck_queries.contains(this))
		icheck_queries.free(this);

	UnrealUser::onDestroy(this);
}

/**
 * Start authentication process.
 */
void UnrealUser::auth()
{
	auth_flags_ << AFNick
				<< AFUser
				<< AFDNS
				<< AFRDNS;

	send(":%s NOTICE AUTH :*** Looking up your hostname",
	    unreal->me->name().c_str());

	/* resolve remote host */
	resolveHostname();

	/* give the user some time to register */
	scheduleAuthTimeout();

	/* remote ident check, if enabled */
	if (unreal->config.get("Features::EnableIdentCheck", "true").toBool())
		checkRemoteIdent();
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
void UnrealUser::checkAuthTimeout(const UnrealTimer::ErrorCode& ec)
{
	if (!ec)
	{
		if (auth_flags_.isset(AFNick) || auth_flags_.isset(AFUser))
		{
			/* haven't received USER and NICK within auth timeout */
			exit("Authorization timeout");
		}
		else if (!auth_flags_.isset(AFNick) && !auth_flags_.isset(AFUser)
				&& last_pong_time_.toTS() == 0)
		{
			if (auth_flags_.isset(AFIdent))
			{
				if (icheck_queries.contains(this))
				{
					UnrealSocket* sptr = icheck_queries[this];
					sptr->cancel();
				}

				destroyIdentRequest();
				sendPing();
			}
			else /* got NICK and USER, but no valid PONG reply */
				exit("Ping timeout");
		}
	}
}

/**
 * Checks for ping timeout.
 */
void UnrealUser::checkPingTimeout(const UnrealTimer::ErrorCode& ec)
{
	if (!ec)
	{
		UnrealTime now = UnrealTime::now();

		if (last_pong_time_ < now.addSeconds(-120))
		{
			/* no PONG reply within two minutes */
			exit("Ping timeout");
		}
		else
		{
			sendPing();
			schedulePingTimeout();
		}
	}
}

/**
 * Initiates remote ident check.
 */
void UnrealUser::checkRemoteIdent()
{
	UnrealSocket* sptr = new UnrealSocket();

	send(":%s NOTICE AUTH :*** Checking Ident",
		unreal->me->name().c_str());

	sptr->onConnected.connect(
		boost::bind(&UnrealUser::handleIdentCheckConnected,
			this,
			_1));
	sptr->onDisconnected.connect(
		boost::bind(&UnrealUser::handleIdentCheckDisconnected,
			this,
			_1,
			_2));
	sptr->onError.connect(
		boost::bind(&UnrealUser::handleIdentCheckError,
			this,
			_1,
			_2));
	sptr->onRead.connect(
		boost::bind(&UnrealUser::handleIdentCheckRead,
			this,
			_1,
			_2));

	/* validate socket */
	UnrealSocket::ErrorCode ec;
	UnrealResolver::Endpoint endpoint = socket_->remote_endpoint(ec);

	if (ec)
	{
		unreal->log.write(UnrealLog::Debug, "checkRemoteIdent: Client socket "
			"disappeared before we could get remote endpoint");

		delete sptr;
		return;
	}

	endpoint.port(113);

	/* connect to remote ident server */
	sptr->connectTo(endpoint);

	/* add ident check to query map */
	icheck_queries.add(this, sptr);

	auth_flags_ << AFIdent;
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
 * Destroy the user object once pending operations have been finished.
 *
 * @param uptr User pointer
 */
void UnrealUser::destroy(UnrealUser* uptr)
{
	/* check for pending requests. when there are none, destruct the user object
	 * immediately
	 */
	if (!uptr->havePendingRequests())
	{
		if (user_destructs.contains(uptr))
		{
			unreal->log.write(UnrealLog::Debug,
				"UnrealUser::destruct(): fd %d - destroy user found on "
				"destruction list",
					uptr->socket()->native());
			
			user_destructs.remove(uptr);
		}

		/* remove from nick list, if found */
		String ln = uptr->lowerNick();

		if (!ln.empty() && unreal->nicks.contains(ln))
			unreal->nicks.remove(ln);

		/* remove from local user list */
		unreal->local_users.remove(uptr->socket());

		/* and global user list */
		unreal->users.remove(uptr);

		delete uptr;
	}
	else
	{
		/* add to the destruct list. callback handlers of pending requests
		 * have to check if there are user destructions in the list,
		 * and if so - call this function again to free the user object.
		 */
		user_destructs << uptr;
		
		unreal->log.write(UnrealLog::Debug,
			"UnrealUser::destruct(): fd %d - adding to destruction list",
				uptr->socket()->native());
	}
}

/**
 * Destroy the remote ident request session.
 *
 * @param sptr Socket pointer
 */
void UnrealUser::destroyIdentRequest()
{
	auth_flags_.revoke(AFIdent);

	if (auth_flags_.value() == 0)
		sendPing();
}

/**
 * Drop client from the network. Generates a quit message to all channels
 * and close the socket connection.
 *
 * @param message Quit message
 */
void UnrealUser::drop(const String& message)
{
	UnrealSocket::ErrorCode ec;

	/* propagate QUIT message to all channels the user may be on */
	exit(ec, message);

	/* close socket */
	exit(message);
}

/**
 * Exit client with an specific error code as given by the networking
 * subsystem.
 *
 * @param ec Error code
 */
void UnrealUser::exit(UnrealSocket::ErrorCode& ec, const String& msg)
{
	String message;

	if (ec)
	{
		message.sprintf("Read error: %d (%s)", ec.value(),
			ec.message().c_str());
	}
	else if (!msg.empty())
		message = msg;
	else
		message = "Exiting";

	/* broadcast quit on all channels */
	while (channels.size() > 0)
	{
		UnrealChannel* chptr = channels.at(0);
		leaveChannel(chptr->name(), message, CMD_QUIT);
	}
}

/**
 * Exit client with specified message and close the socket.
 *
 * @param message Message to send for closing link
 */
void UnrealUser::exit(const String& message)
{
	/* close socket */
	if (socket_->is_open())
	{
		String reply;
		UnrealSocket::ErrorCode ec;

		reply.sprintf("ERROR :Closing link: %s by %s (%s)",
			nickname_.empty() ? "*" : nickname_.c_str(),
			unreal->me->name().c_str(),
			message.c_str());

		send(reply);
		socket_->close(ec);

		if (ec)
		{
			unreal->log.write(UnrealLog::Debug, "UnrealUser::exitClient(): "
					"close() returned with errcode %d (%s)",
					ec.value(),
					ec.message().c_str());
		}
	}
}

/**
 * Lookup a user entry by socket.
 * Note: This works for local users only, as we don't know about the file
 *       descriptors for remote servers.
 *
 * @param sptr UnrealSocket pointer
 */
UnrealUser* UnrealUser::find(UnrealSocket* sptr)
{
	if (unreal->local_users.contains(sptr))
		return unreal->local_users[sptr];
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

	if (unreal->nicks.contains(lower))
		return unreal->nicks[lower];
	else
		return 0;
}

/**
 * Asyncronous callback that is called once the remote ident check
 * socket is connected and ready to send the request.
 *
 * @param sptr Pointer to Socket
 */
void UnrealUser::handleIdentCheckConnected(UnrealSocket* sptr)
{
	String request_str;

	request_str.sprintf("%d, %d",
		socket_->remote_endpoint().port(),
		socket_->local_endpoint().port());

	/* send ident request */
	sptr->write(request_str);
}

/**
 * Asyncronous callback that indicates that an disconnected happened while
 * trying to fetch data from the ident request socket.
 *
 * @param sptr Pointer to Socket
 */
void UnrealUser::handleIdentCheckDisconnected(UnrealSocket* sptr,
	const UnrealSocket::ErrorCode& ec)
{
	icheck_queries.remove(this);
	delete sptr;
	destroyIdentRequest();

	/* check for destruction request, as ident request is done */
	if (user_destructs.contains(this))
		UnrealUser::destroy(this);
}

/**
 * Asyncronous callback that indicates that an error occured on the ident check
 * socket.
 *
 * @param sptr Pointer to Socket
 * @param ec Error code
 */
void UnrealUser::handleIdentCheckError(UnrealSocket* sptr,
	const UnrealSocket::ErrorCode& ec)
{
	send(":%s NOTICE AUTH :*** No ident response",
	    unreal->me->name().c_str());

	UnrealSocket::ErrorCode err = ec;

	if (err.value() == boost::asio::error::operation_aborted)
		icheck_queries.free(this);

	destroyIdentRequest();
}

/**
 * Asyncronous callback that indicates that we received an response message
 * from the remote ident server.
 *
 * @param sptr Pointer to Socket
 * @param data Last line read from socket
 */
void UnrealUser::handleIdentCheckRead(UnrealSocket* sptr, String& data)
{
	StringList tokens = data.split(":");
	bool haveError = false;

	if (tokens.size() >= 3)
	{
		StringList ports = tokens.at(0).split(",");
		uint16_t local_port = 0, remote_port = 0;

		unreal->log.write(UnrealLog::Debug, "icheck_read: ports.size() = %u",
			ports.size());
	
		if (ports.size() >= 2)
		{
			remote_port = ports.at(0).trimmed().toUInt16();
			local_port = ports.at(1).trimmed().toUInt16();

			String replCmd = tokens.at(1).trimmed();

			unreal->log.write(UnrealLog::Debug, "icheck_read: replCmd = [%s]",
				replCmd.c_str());

			if (replCmd == "USERID" && tokens.size() >= 4)
			{
				String username = tokens.at(3).trimmed();

				/* ident reply was OK, set the username */
				setIdent(username);
			}
			else
			{
				haveError = true;
			}
		}
		else
		{
			haveError = true;
		}
	}

	if (haveError)
		send(":%s NOTICE AUTH :*** No valid ident response",
		    unreal->me->name().c_str());
	else
		send(":%s NOTICE AUTH :*** Got ident response",
		    unreal->me->name().c_str());

	destroyIdentRequest();
}

/**
 * Callback for resolver replies.
 *
 * @param ec Error code
 * @param response Response iterator
 */
void UnrealUser::handleResolveResponse(const UnrealResolver::ErrorCode& ec,
	UnrealResolver::Iterator response)
{
	String address = socket_->remote_endpoint().address().to_string();

	if (ec)
	{
		send(":%s NOTICE AUTH :Couldn't look up your hostname",
		    unreal->me->name().c_str());
		auth_flags_.revoke(AFRDNS);
	}
	else
	{
		UnrealResolver::Endpoint endpoint = *response;

		if (auth_flags_.isset(AFDNS))
		{
			UnrealResolver* rq = resolver_queries[socket_];

			/* store the resolved hostname somewhere */
			setHostname((*response).host_name());
			setRealHostname(hostname_);

			auth_flags_.revoke(AFDNS);

			if (!rq)
			{
				unreal->log.write(UnrealLog::Error, "handleResolveResponse: "
					"Can't locate resolver query");
			}
			else
			{
				/* query hostname -> address */
				rq->query((*response).host_name(), endpoint.port());
				
				return;
			}
		}
		else
		{
			if (endpoint.address() != socket_->remote_endpoint().address())
			{
				send(":%s NOTICE AUTH :*** Forward and reverse hostname do not "
					"match, using IP-Address instead",
					unreal->me->name().c_str());
			}
			else
			{
				send(":%s NOTICE AUTH :*** Retrieved hostname (%s)",
                	unreal->me->name().c_str(),
					hostname_.c_str());
				
				address = hostname_;
			}

			auth_flags_.revoke(AFRDNS);
		}
	}

	/* update entries */
	setHostname(address);
	setRealHostname(address);

	/* remove the previously allocated resolver */
	socket_->destroyResolverQuery();

	/* check for destruction request, as asyncronous DNS request is done */
	if (user_destructs.contains(this))
	{
		UnrealUser::destroy(this);
		return;
	}

	if (auth_flags_.value() == 0)
		sendPing();
}

/**
 * Returns whether there is a pending DNS or Ident request. Used to track
 * user entries that can't be destroyed immediately, otherwise asyncronous
 * callback signals may emit on non-existing objects.
 *
 * @return True on pending requests, otherwise false
 */
bool UnrealUser::havePendingRequests()
{
	return (auth_flags_.isset(AFDNS) || auth_flags_.isset(AFIdent));
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
 * Returns whether the user has the deaf usermode set.
 */
bool UnrealUser::isDeaf()
{
	using namespace UnrealUserProperties;
	
	if (ModeTable.contains(Deaf)
			&& modes_.isset(ModeTable.value(Deaf)))
		return true;
	else
		return false;
}

/**
 * Returns whether the user has been registered to the network.
 */
bool UnrealUser::isIntroduced()
{
	return auth_flags_.value() == 0;
}

/**
 * Returns whether the user has the invisible mode flag set.
 */
bool UnrealUser::isInvisible()
{
	using namespace UnrealUserProperties;

	if (ModeTable.contains(Invisible)
			&& modes_.isset(ModeTable.value(Invisible)))
		return true;
	else
		return false;
}

/**
 * Returns whether the user has IRC operator status.
 */
bool UnrealUser::isOper()
{
	using namespace UnrealUserProperties;

	if (ModeTable.contains(Operator)
			&& modes_.isset(ModeTable.value(Operator)))
		return true;
	else
		return false;
}

/**
 * Join user into a channel.
 *
 * @param chname Channel name to enter
 * @param key (optional) channel key
 */
void UnrealUser::joinChannel(const String& chname, const String& key)
{
	UnrealChannel* chptr = UnrealChannel::find(chname);
	uint8_t flags = 0;

	if (!chptr)
	{
		chptr = new UnrealChannel(chname);
		flags = UnrealChannel::Member::ChanOp;
	}

	/* invited users may override almost anything */
	bool force = chptr->isInvited(this);
	/**
	 * Order of things to check when joining channel:
	 * This list is structured so that channels with multiple modes
	 * will give the least amount of information regarding that channel
	 * should a user be unable to join said channel
	 * 1. check for invite-only channel
	 * 2. check for channel key
	 * 3. check for channel limit
	 * 4. check for ban
	 * We insert a check to see if the user is already in the channel
	 * before all of the above so that we don't print out error messages
	 * for a user already in a channel trying to join it again
	 */
	if (chptr->findMember(this))
	{
		return; /* ignore this join-attempt */
	}
	else if (chptr->isInviteOnly() && !isOper() && !force)
	{
		sendreply(ERR_INVITEONLYCHAN,
			String::format(MSG_INVITEONLYCHAN,
				chptr->name().c_str()));
	}
	else if (chptr->isKey() && key != chptr->key() && !isOper() && !force)
	{
		sendreply(ERR_BADCHANNELKEY,
			String::format(MSG_BADCHANNELKEY,
				chptr->name().c_str()));
	}
	else if (chptr->isLimit() && chptr->members.size() >= chptr->limit()
		&& !isOper() && !force)
	{
		sendreply(ERR_CHANNELISFULL,
			String::format(MSG_CHANNELISFULL,
				chptr->name().c_str()));
	}
	else if (chptr->isBanned(this) && !force)
	{
		sendreply(ERR_BANNEDFROMCHAN,
			String::format(MSG_BANNEDFROMCHAN,
				chptr->name().c_str()));
	}
	else
	{
		/* add us to the channel */
		chptr->addMember(this, flags);

		/* let anyone know that we're joining */
		chptr->sendlocalreply(this, CMD_JOIN, String());

		if (chptr->creationTime().toTS() > 0)
			chptr->sendreply(this, RPL_CHANNELCREATED,
				String::format(MSG_CHANNELCREATED,
					chptr->creationTime().toTS()));

		StringList tmp_strl;
		tmp_strl << "0" << chptr->name();

		/* look up some commands */
		UnrealUserCommand* ucptr;
		UnrealUserCommand::Function fn;

		/* sending names */
		if ((ucptr = UnrealUserCommand::find(CMD_NAMES)))
		{
			fn = ucptr->fn();
			fn(this, &tmp_strl);
		}

		/* and the topic */
		if ((ucptr = UnrealUserCommand::find(CMD_TOPIC)))
		{
			fn = ucptr->fn();
			fn(this, &tmp_strl);
		}

		/* if the user was invited, remove the invite entry */
		if (chptr->invites.contains(this))
			chptr->invites.remove(this);
	}
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
 * Leave user from channel.
 *
 * @param chname Channel name to leave
 * @param message Leave message
 * @param type Specify QUIT, KICK or PART
 */
void UnrealUser::leaveChannel(const String& chname, const String& message,
	const String& type)
{
	UnrealChannel* chptr = UnrealChannel::find(chname);

	if (chptr)
	{
		UnrealChannel::Member* cmptr = chptr->findMember(this);

		if (!cmptr)
			chptr->sendreply(this, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL);
		else
		{
			String msg;

			if (!message.empty())
				msg = " :" + message;

			if (type == CMD_PART)
			{
				chptr->sendlocalreply(this, type, msg);
			}
			else if (type == CMD_QUIT)
			{
				String reply;

				chptr->removeMember(this);
				
				if (chptr->members.size() == 0)
				{
					delete chptr;
					return;
				}

				/* build custom message for QUIT */
				reply.sprintf(":%s!%s@%s %s%s",
					nickname_.c_str(),
					ident_.c_str(),
					hostname_.c_str(),
					type.c_str(),
					msg.c_str());

				for (UnrealChannel::MemberIterator cm = chptr->members.begin();
						cm != chptr->members.end(); ++cm)
				{
					UnrealUser* uptr = cm->first;
					uptr->send(reply);
				}
				return;
			}

			/* common stuff */
			chptr->removeMember(this);

			if(chptr->members.size() == 0)
				delete chptr;
		}
	}
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
 * Returns the full mask (format nick!ident@host) for the user.
 *
 * @return Full mask notation
 */
String UnrealUser::mask()
{
	String mask;
	
	mask.sprintf("%s!%s@%s",
		nickname_.c_str(),
		ident_.c_str(),
		hostname_.c_str());

	return mask;
}

/**
 * Returns whether the mask of the user matches the specified pattern.
 *
 * @param pattern Glob-pattern to check matching
 * @return True if matching, otherwise false
 */
bool UnrealUser::match(const String& pattern)
{
	String m = mask();

	return m.match(pattern);
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
	String result;

	/* user mode table */
	UnrealUserModeTable& modetab = UnrealUserProperties::ModeTable;

	for (UnrealUserModeTable::Iterator umi = modetab.begin();
			umi != modetab.end(); ++umi)
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
 * Sends a notice to all IRC operators.
 *
 * @param msg Message to send
 */
void UnrealUser::notifyOpers(const String& msg)
{
	List<UnrealUser*>::Iterator ui;
	UnrealUser* uptr;
	String preparedMsg = ":*** Notice -- " + msg;

	for (ui = unreal->users.begin(); ui != unreal->users.end(); ++ui)
	{
		uptr = *ui;

		if (!uptr->isOper())
			continue;
		else
			uptr->sendreply(CMD_NOTICE, preparedMsg);
	}
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

	for (String::Iterator ch = flagset.begin(); ch != flagset.end(); ++ch)
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
					else if (umo == UnrealUserProperties::Operator)
					{
						unreal->stats.operators--;
					}
				}
			}
		}
	}

	/* send mode changes */
	if (changeset.length() > 1)
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
	String version = String(PACKAGE_VERSTR);

	/* collect user mode flags */
	String user_modes;

	{
		using namespace UnrealUserProperties;

		for (UnrealUserModeTable::Iterator i = ModeTable.begin();
				i != ModeTable.end(); ++i)
		{
			user_modes.append(1, (i->first).mode_char);
		}
	}

	/* collect channel mode flags */
	String chan_modes;

	{
		using namespace UnrealChannelProperties;

		for (UnrealChannelModeTable::Iterator i = ModeTable.begin();
				i != ModeTable.end(); ++i)
		{
			chan_modes.append(1, (i->first).mode_char);
		}
	}

	sendreply(RPL_WELCOME,
		String::format(MSG_WELCOME,
			cfg.get("Me::Network", "ExampleNet").c_str(),
			nickname_.c_str()));
	sendreply(RPL_YOURHOST,
		String::format(MSG_YOURHOST,
			unreal->me->name().c_str(),
			version.c_str()));
	sendreply(RPL_CREATED,
		String::format(MSG_CREATED,
			unreal->me->bootTime().toString("%Y-%M-%dT%H:%M:%S %Z").c_str()));

	sendreply(RPL_MYINFO,
		String::format(MSG_MYINFO,
			unreal->me->name().c_str(),
			version.c_str(),
			user_modes.c_str(),
			chan_modes.c_str()));

	/* send server features */
	sendISupport();

	/* modify stats */
	if (unreal->stats.connections_unk > 0)
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

	/* message of the day */
	if ((ucptr = UnrealUserCommand::find(CMD_MOTD)))
	{
		if (unreal->config.get("Features::ShortMOTD", "false").toBool())
		{
			sendreply(CMD_NOTICE, ":You can get the message of the day "
				"by using /MOTD");
		}
		else
			ucptr->fn()(this, 0);
	}
	else
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
	UnrealSocket::ErrorCode ec;
	UnrealResolver::Endpoint endpoint = socket_->remote_endpoint(ec);

	if (ec)
	{
		unreal->log.write(UnrealLog::Debug, "resolveHostname: Client socket "
			"disappeared when getting remote endpoint");

		auth_flags_.revoke(AFDNS);
		return;
	}

	UnrealResolver* rq = new UnrealResolver();

	rq->onResolve.connect(
		boost::bind(&UnrealUser::handleResolveResponse,
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
	int authTimeout = unreal->config.get("Limits::AuthTimeout", "12").toInt();
	
	timer_.expires_from_now(boost::posix_time::seconds(authTimeout));
	timer_.async_wait(
		boost::bind(&UnrealUser::checkAuthTimeout,
			this,
			boost::asio::placeholders::error));
}

/**
 * Schedule the ping timeout check.
 */
void UnrealUser::schedulePingTimeout()
{
	int pingFreq = static_cast<int>(listener_->pingFrequency());

	timer_.expires_from_now(boost::posix_time::seconds(pingFreq));
	timer_.async_wait(
		boost::bind(&UnrealUser::checkPingTimeout,
			this,
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
 * Send data to the client, printf-style version.
 *
 * @param fmt Formatted data to send
 */
void UnrealUser::send(const char* fmt, ...)
{
	char buffer[U4_BUFSIZE];
	va_list ap;

	va_start(ap, fmt);
	std::vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	socket_->write(buffer);
}

/**
 * Tell the client the features we support.
 */
void UnrealUser::sendISupport()
{
	String buf;

	for (UnrealISupport::Iterator i = unreal->isupport.begin();
			i != unreal->isupport.end(); ++i)
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
			unreal->me->name().c_str(),
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
			unreal->me->name().c_str(),
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
			unreal->me->name().c_str());

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
