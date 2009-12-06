/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         client.hpp
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

#ifndef _UNREALIRCD_USER_HPP
#define _UNREALIRCD_USER_HPP

#include "bitmask.hpp"
#include "listener.hpp"
#include "mode.hpp"
#include "numeric.hpp"
#include "platform.hpp"
#include "resolver.hpp"
#include "string.hpp"
#include "time.hpp"
#include <boost/asio.hpp>

/**
 * Representation of an user entry.
 */
class UnrealUser
{
public:
	/**
	 * Enumeration of authentification flags. They're used to find out if we
	 * got all necessary informations to register a user.
	 */
	enum AuthFlagType
	{
		/** awaiting NICK message for initial user registration */
		AFNick	= 0x01,

		/** awaiting USER message for initial user registration */
		AFUser	= 0x02,

		/** awaiting DNS lookup response */
		AFDNS	= 0x04,

		/** awaiting ident request response */
		AFIdent = 0x08
	};

	/**
	 * Enumeration of user mode flags.
	 */
	enum ModeFlagType
	{
		/** user will not receive channel messages */
		mode_deaf 		= 0x0001,

		/** user is marked as being invisible */
		mode_invisible 	= 0x0002,

		/** user is marked as being an IRC operator */
		mode_operator 	= 0x0004,

		/** user receives wallop messages */
		mode_wallops 	= 0x0008
	};

public:
	UnrealUser(UnrealSocket* sptr = 0);
	~UnrealUser();
	void auth();
	Bitmask<uint8_t>& authflags();
	const String& awayMessage();
	UnrealTime connectionTime();
	void exitClient(const String& message);
	static UnrealUser* find(UnrealSocket* sptr);
	static UnrealUser* find(const String& nickname);
	const String& hostname();
	const String& ident();
	bool isAway();
	bool isOper();
	UnrealTime lastActionTime();
	UnrealTime lastPongTime();
	UnrealListener* listener();
	String lowerNick();
	Bitmask<uint16_t>& modes();
	String modestr();
	const String& nick();
	const String& realHostname();
	const String& realname();
	void registerUser();
	void send(const String& data);
	void sendISupport();
	void sendlocalreply(const String& cmd, const String& data);
	void sendreply(IRCNumeric numeric, const String& data);
	void sendreply(const String& cmd, const String& data);
	void sendreply(UnrealUser* uptr, const String& cmd,
			const String& data);
	void sendPing();
	void setAwayMessage(const String& msg);
	void setHostname(const String& newhost);
	void setIdent(const String& newident);
	void setLastActionTime(const UnrealTime& ts);
	void setLastPongTime(const UnrealTime& ts);
	void setListener(UnrealListener* lptr);
	void setNick(const String& newnick);
	void setRealHostname(const String& newhost);
	void setRealname(const String& rn);
	UnrealSocket* socket();

private:
	void checkAuthTimeout(const UnrealSocket::ErrorCode& ec);
	void checkPingTimeout(const UnrealSocket::ErrorCode& ec);
	void handleResolveResponse(const UnrealResolver::ErrorCode& ec,
			UnrealResolver::Iterator response);
	void resolveHostname();
	void scheduleAuthTimeout();
	void schedulePingTimeout();

private:
	/** authentication flags */
	Bitmask<uint8_t> auth_flags_;

	/** user mode flags */
	Bitmask<uint16_t> modes_;

	/** socket of this user; this is set to zero if it's not a real user */
	UnrealSocket* socket_;

	/** listener where this user is attached to */
	UnrealListener* listener_;

	/** connection timestamp */
	UnrealTime connection_time_;

	/** last pong timestamp */
	UnrealTime last_pong_time_;

	/** last action time */
	UnrealTime last_action_time_;

	/** nick name */
	String nickname_;

	/** user name (ident) */
	String ident_;

	/** visible host name */
	String hostname_;

	/** real host name */
	String real_hostname_;

	/** real name */
	String realname_;

	/** away message */
	String away_message_;

	/** timeout timer */
	boost::asio::deadline_timer timer_;
};

namespace UnrealUserProperties
{
	extern UnrealUserMode Deaf;
	extern UnrealUserMode Invisible;
	extern UnrealUserMode Operator;
	extern UnrealUserMode Wallops;
	extern UnrealUserModeTable ModeTable;
}

#endif /* _UNREALIRCD_USER_HPP */
