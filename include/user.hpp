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
#include "platform.hpp"
#include "resolver.hpp"
#include "string.hpp"
#include "time.hpp"

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

public:
	UnrealUser(UnrealSocket* sptr = 0);
	~UnrealUser();
	void auth();
	Bitmask<uint8_t>& authflags();
	UnrealTime connectionTime();
	const String& hostname();
	const String& ident();
	UnrealListener* listener();
	const String& nick();
	const String& realHostname();
	void send(const String& data);
	void setHostname(const String& newhost);
	void setIdent(const String& newident);
	void setListener(UnrealListener* lptr);
	void setNick(const String& newnick);
	void setRealHostname(const String& newhost);
	UnrealSocket* socket();

private:
	void handleResolveResponse(const UnrealResolver::ErrorCode& ec,
			UnrealResolver::Iterator response);
	String lowerNick();
	void resolveHostname();

private:
	/** authentication flags */
	Bitmask<uint8_t> auth_flags_;

	/** socket of this user; this is set to zero if it's not a real user */
	UnrealSocket* socket_;

	/** listener where this user is attached to */
	UnrealListener* listener_;

	/** connection timestamp */
	UnrealTime connection_time_;

	/** nick name */
	String nickname_;

	/** user name (ident) */
	String ident_;

	/** visible host name */
	String hostname_;

	/** real host name */
	String real_hostname_;
};

#endif /* _UNREALIRCD_USER_HPP */
