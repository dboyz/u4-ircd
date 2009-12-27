/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         listener.hpp
 * Description  Connection listener
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

#ifndef _UNREALIRCD_LISTENER_HPP
#define _UNREALIRCD_LISTENER_HPP

#include "list.hpp"
#include "reactor.hpp"
#include "signal.hpp"
#include "socket.hpp"
#include "string.hpp"
#include "stringlist.hpp"

class UnrealListener
{
public:
	/** Listener type */
	enum ListenerType { LClient, LServer };

public:
	UnrealListener(UnrealReactor& reactor, const String& address,
			const uint16_t& port);
	~UnrealListener();

	void addConnection(UnrealSocket* sptr);
	String bindAddress();
	uint16_t bindPort();
	uint32_t maxConnections();
	uint32_t pingFrequency();
	void removeConnection(UnrealSocket* sptr);
	void run();
	void setBindAddress(const String& address);
	void setBindPort(const uint16_t& port);
	void setMaxConnections(const uint32_t& max_conn);
	void setPingFrequency(const uint32_t& ping_freq);
	void setType(ListenerType lty);
	StringList splitLine(String& data);
	ListenerType type();
	void waitForAccept();

public:
	/** connections attached to this listener */
	List<UnrealSocket*> connections;

public:
	/** signal which is triggered on a new connection ready */
	UnrealSignal0<UnrealListener> onNewConnection;

private:
	void handleNewConnection();

private:
	/** listener type */
	ListenerType type_;

	/** bind address */
	String address_;

	/** bind port */
	uint16_t port_;

	/** ping frequency for this listener */
	uint32_t ping_freq_;

	/** max amount of connections allowed for this listener */
	uint32_t max_connections_;

	/** native file descriptor */
	int native_;
};

#endif /* _UNREALIRCD_LISTENER_HPP */
