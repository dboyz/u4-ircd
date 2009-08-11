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

#include "socketengine.h"
#include "misc.h"

SocketInfo::SocketInfo(int socket_, SocketEventHandler *handler_)
	: socket(socket_), 
	  handler(handler_)
{
}

bool
SocketEngine::SocketInfoRegistered(int socket)
{
	for(std::vector<SocketInfo>::iterator i = sockets.begin(); i != sockets.end(); i ++)
		if(i->socket == socket)
			return true;
	return false;
}

SocketInfo &
SocketEngine::getSocketInfo(int socket) throw(std::runtime_error)
{
	for(std::vector<SocketInfo>::iterator i = sockets.begin(); i != sockets.end(); i ++)
		if(i->socket == socket)
		{
			i->handler->handleEvent(socket);
			return *i;
		}
	throw std::runtime_error("socket " + ConvertIntToString(socket) + " requested but not found");
}

SocketEngine*
SocketEngine::create(Module& module) throw(std::bad_cast)
{
	SocketEngine *newEngine;
	
	newEngine = dynamic_cast<SocketEngine>(module.getObject("SocketEngine"));
	if(!newEngine)
		throw std::bad_cast();
	
	return newEngine;
}

void
SocketEngine::runEventHandler(int socket) throw(std::runtime_error)
{
	SocketInfo &sockinfo = getSocketInfo(socket);
	
	runEventHandler(sockinfo);
}

void
SocketEngine::runEventHandler(SocketInfo& sock)
{
	sock.handler->handleEvent(sock.socket);
}

void
SocketEngine::addSocket(int socket, SocketEventHandler *handler)
{
	/*
	 * TODO: Design decision:
	 * Should we allow multiple SocketEventHandlers to be
	 * registered per socket or only one? Currently, we
	 * only allow one per socket.
	 */
	if(SocketInfoRegistered(socket))
		throw std::runtime_error("socket " + ConvertIntToString(socket) + " already in socketengine, attempt to readd it");
	
	SocketInfo sockinfo(socket, handler);

	sockets.push_back(sockinfo);
	monitorSocket(sockinfo);
}

void
SocketEngine::removeSocket(int socket)
{
	for(std::vector<SocketInfo>::iterator i = sockets.begin(); i != sockets.end(); i ++)
		if(i->socket == socket)
		{
			unmonitorSocket(*i);
			sockets.erase(i);
			return;
		}
}
