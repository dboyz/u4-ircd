/*  UnrealIRCd -- The Next Generation IRCd.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 1, or (at your option)
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _UNREALIRCD_SOCKETENGINE_H
#define _UNREALIRCD_SOCKETENGINE_H

#include "modules.h"

#include <vector>
#include <stdexcept>
#include <typeinfo>

/*
 * This class is used to define different actions for when
 * activity is detected on a socket.
 */
class SocketEventHandler
{
 public:
	/*
	 * Maybe a type of event can be passed... but in any case,
	 * the handleEvent() function would have to double-check
	 * that the event happened.
	 */
	virtual void handleEvent(int socket) = 0;

	virtual ~SocketEventHandler();
};

/*
 * For SocketEngine
 * A datatype to store necessary information about
 * a monitored socket.
 */
class SocketInfo
{
public:
	int socket;
	SocketEventHandler *handler;
	
	SocketInfo(int socket, SocketEventHandler *handler);
};


/*
 * The point of SocketEngine is to facilitate listen()ing for
 * connections and handling received data.
 */

class SocketEngine 
	: public ModularObject
{
private:
	/*
	 * tells if a given socket is registered with the SocketEngine
	 */
	bool SocketInfoRegistered(int socket);
	
	/* 
	 * Gets a SocketInfo from std::vector<SocketInfo> sockets
	 * based on the socket.
	 */
	SocketInfo &getSocketInfo(int socket) throw(std::runtime_error);
	
	
protected:
	/*
	 * Only children may be instantiated (unless if this class
	 * becomes a portable, default SocketEngine)
	 */
	SocketEngine();

	/*
	 * Stores all monitored sockets
	 */
	std::vector<SocketInfo> sockets;

	/*
	 * Methods to be called by the underlying module
	 * when something happens to a socket.
	 */
	void runEventHandler(int socket) throw(std::runtime_error);
	void runEventHandler(SocketInfo& socket);

	/*
	 * Overridable functions:
	 */
	virtual void monitorSocket(SocketInfo& info) = 0;
	virtual void unmonitorSocket(SocketInfo& info) = 0;
 public:
	/*
	 * Pass this the module that will act as the SocketEngine.
	 */
	static SocketEngine *create(Module& module) throw (std::bad_cast);

	/*
	 * Adds a socket to be monitored and an associated event handler.
	 * This will add the information to the this->sockets list and
	 * follow up by calling module-specific code.
	 *
	 * The socket will be monitored by the time this method returns.
	 * As soon as monitoring is in place, the handler will be awoken;
	 * it should check for events that may have happened while
	 * monitoring was being initialized.
	 *
	 * handler must be constructed using new, as removeSocket
	 * will run the descructer and delete it.
	 */
	void addSocket(int socket, SocketEventHandler *handler);

	/*
	 * Stops monitoring socket. This causes the associated
	 * SocketEventHandler to be deleted/freed
	 */
	void removeSocket(int socket);

	virtual ~SocketEngine();
};

#endif
