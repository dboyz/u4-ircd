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

SocketEngine
SocketEngine::create(Module& module)
{
	SocketEngine &newEngine;
	
	newEngine = (SocketEngine&) module->getObject("SocketEngine");
	
}

void
SocketEngine::runEventHandler(int socket)
{
	for(std::vector<SocketInfo>::iterator i = sockets.begin; i; i ++)
		if(i->socket == socket)
			runEventHandler(*i);
}

void
SocketEngine::runEventHandler(SocketInfo& sock)
{
	sock.handler->handleEvent(sock.socket);
}

void
addSocket(int socket, SocketEventHandler *handler)
{
	/*
	 * TODO: make sure that each socket only has one 
	 * handler associated with it
	 */
	SocketInfo sockinfo;

	sockinfo->socket = socket;
	sockinfo->handler = handler;

	sockets.push_back(sockinfo);
	monitorSocket(sockinfo);
}

void
removeSocket(int socket)
{
	for(std::vector<SocketInfo>::iterator i = sockets.begin; i; i ++)
		if(i->socket == socket)
		{
			unmonitorSocket(*i);
			sockets.erase(i);
			return;
		}
}
